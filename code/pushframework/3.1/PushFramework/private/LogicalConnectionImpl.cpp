#include "StdAfx.h"
#include "LogicalConnectionImpl.h"

#include "../include/LogicalConnection.h"
#include "../include/ServerOptions.h"
#include "../include/Common.h"

#include "PhysicalConnection.h"
#include "ServerImpl.h"
#include "ClientFactory.h"
#include "BroadcastManager.h"
#include "BroadcastQueueContext.h"
#include "BroadcastQueueGroupContext.h"
#include "BroadcastQueue.h"
#include "BroadcastStreamer.h"
#include "PhysicalConnectionPool.h"

namespace PushFramework
{
	LogicalConnectionImpl::LogicalConnectionImpl(LogicalConnection* facade)
	{
		this->facade = facade;
		pRootQueueGroupContext = NULL;
		pPhysicalConnection = NULL;
	}
	
	void LogicalConnectionImpl::Recycle()
	{
		if (pRootQueueGroupContext != NULL)
		{
			delete pRootQueueGroupContext;
			pRootQueueGroupContext = NULL;
		}

		if (facade)
		{
			facade->Recycle();
		}

		if (pPhysicalConnection)
		{
			thePhysicalConnectionPool.returnObject(pPhysicalConnection);
			pPhysicalConnection	= NULL;
		}
	}

	LogicalConnectionImpl::~LogicalConnectionImpl(void)
	{
	}

	SendResult::Type LogicalConnectionImpl::PushPacket( OutgoingPacket* pPacket )
	{
		SendResult::Type result = pPhysicalConnection->pushPacket(pPacket);
		if (result == SendResult::NotOK)
		{
			clientFactoryImpl.disconnect(this, false, DisconnectionReason::PeerClosure);
		}
		return result;
	}

	SendResult::Type LogicalConnectionImpl::TryPushPacket( OutgoingPacket* pPacket )
	{
		SendResult::Type result = pPhysicalConnection->tryPushPacket(pPacket);
		if (result == SendResult::NotOK)
		{
			clientFactoryImpl.disconnect(this, false, DisconnectionReason::PeerClosure);
		}
		return result;
	}

	double LogicalConnectionImpl::getVisitDuration()
	{
		time_t current = time(NULL);
		return difftime(current, dtConnectTime);
	}

	bool LogicalConnectionImpl::CanDelete()
	{
		return nRefrenceCounter==0 && pPhysicalConnection->checkIfUnusedByIOWorkers();
	}

	void LogicalConnectionImpl::IncrementUsage()
	{
		AtomicIncrement(&nRefrenceCounter);
	}

	void LogicalConnectionImpl::DecrementUsage()
	{
		AtomicDecrement(&nRefrenceCounter);
	}

	bool LogicalConnectionImpl::IsInactive()
	{
		double uInactivityTime = pPhysicalConnection->getTimeToLastReceive();

		if (uInactivityTime > options.uMaxClientIdleTime)
		{
			return true;
		}

		return false;
	}

	bool LogicalConnectionImpl::SubscribeToQueue( BROADCASTQUEUE_NAME queueName, bool ignorePreviousPackets /*= false*/ )
	{
		ScopedReadWriteLock(subscriptionDataLock, true);

		// Locate the queue.
		BroadcastQueue* pQueue = broadcastManager.getQueue(queueName);
		if (!pQueue)
		{
			return false;
		}

		// Create a queue context to hold our last sent packet Id etc.
		BroadcastQueueContext* pInfo = new BroadcastQueueContext(pQueue, ignorePreviousPackets ? pQueue->getLastGeneratedId() : 0);

		// Insert this context in the ordered chain of all contexts (based on queue priority and quota attributes):
		if (!pRootQueueGroupContext || !pRootQueueGroupContext->insert(pInfo))
		{
			BroadcastQueueGroupContext* pGroup = new BroadcastQueueGroupContext(pInfo);
			pGroup->pNextGroup = pRootQueueGroupContext;
			pRootQueueGroupContext = pGroup;
		}

		return true;
	}

	bool LogicalConnectionImpl::CheckAndProcessPendingBroadcast( bool somethingHasHappened )
	{
		ScopedReadWriteLock(subscriptionDataLock, false);

		bool ret = true;

		if (somethingHasHappened)
		{
			skipCheckNextPacket = false;
		}

		if (skipCheckNextPacket)
		{
			return ret;
		}

		// Covers the case when connection is not subscribed to any queue.
		if (!pRootQueueGroupContext)
		{
			return ret;
		}

		if (pPhysicalConnection->IsWriteInProgress())
		{
			return ret; // We know that this method will be called again shortly.
		}

		if (!CompareAndSwap(&broadcastRunFlag, 1, 0))
		{
			return ret; //Someone else is running here, we do not want to send duplicate messages.
		}
		bool isPacketFound = false;

		BroadcastQueueGroupContext* pGroup = pRootQueueGroupContext;

		while (pGroup)
		{
			PacketInfo* pPacketInfo = NULL;
			OutgoingPacket* outgoingPacket = pGroup->getNextPacket(pPacketInfo);
			if (!outgoingPacket)
			{
				pGroup = pGroup->pNextGroup;
				continue;
			}

			isPacketFound = true;

			int result = PushPacket(outgoingPacket);

			bool isSent = result == SendResult::OK;

			pGroup->returnPacket(pPacketInfo, isSent);

			if (!isSent) //(ret == SendResult::Retry || ret == SendResult::NotOK)
			{
				if(result == SendResult::NotOK)
					ret = false;
				break;
			}
		}

		if (!isPacketFound)
		{
			skipCheckNextPacket = true;
		}

		broadcastRunFlag = 0;
		return ret;
	}

	bool LogicalConnectionImpl::IsMonitor()
	{
		return false;
	}

	void LogicalConnectionImpl::UnSubscribeFromQueue( BROADCASTQUEUE_NAME queueName )
	{
		ScopedReadWriteLock(subscriptionDataLock, true);

		if (pRootQueueGroupContext)
		{
			pRootQueueGroupContext->remove(queueName);
			if (pRootQueueGroupContext->pRootItem == NULL)
			{
				BroadcastQueueGroupContext* pTemp = pRootQueueGroupContext;

				pRootQueueGroupContext = pTemp->pNextGroup;
				pTemp->pNextGroup = NULL;

				delete pTemp;
			}
		}
	}

	void LogicalConnectionImpl::UnSubscribeFromAll()
	{
		ScopedReadWriteLock(subscriptionDataLock, true);

		if (pRootQueueGroupContext)
			delete pRootQueueGroupContext;
	}

	void LogicalConnectionImpl::initializeIntern( PhysicalConnection* physicalConnection )
	{
		pPhysicalConnection = physicalConnection;
		physicalConnection->attachToClient(this);

		dtConnectTime = time(NULL);
		nRefrenceCounter = 0;
		pStreamer = NULL;
		pRootQueueGroupContext = NULL;
		skipCheckNextPacket = false;
		broadcastRunFlag = 0;
	}

	LogicalConnection* LogicalConnectionImpl::getFacade()
	{
		return facade;
	}

	void LogicalConnectionImpl::OnReadyForSend( unsigned int nAvailableSpace )
	{
		return facade->OnReadyForSend(nAvailableSpace);
	}

	void LogicalConnectionImpl::OnDisconnected( DisconnectionReason::Type closeReason )
	{
		return facade->OnDisconnected(closeReason);
	}

	void LogicalConnectionImpl::OnConnected()
	{
		return facade->OnConnected();
	}

	bool LogicalConnectionImpl::OnBeforeIdleClose()
	{
		return facade->OnBeforeIdleClose();
	}

	Login::Type LogicalConnectionImpl::processLogin( LoginData& loginData )
	{
		return facade->processLogin(loginData);
	}

	void LogicalConnectionImpl::Disconnect( bool waitForPendingPackets )
	{
		clientFactoryImpl.disconnect(this, waitForPendingPackets, DisconnectionReason::RequestedClosure);
	}
}