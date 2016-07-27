/********************************************************************
	File :			PhysicalConnection.cpp
	Creation date :	2010/6/27

	License :			Copyright 2010 Ahmed Charfeddine, http://www.pushframework.com

				   Licensed under the Apache License, Version 2.0 (the "License");
				   you may not use this file except in compliance with the License.
				   You may obtain a copy of the License at

					   http://www.apache.org/licenses/LICENSE-2.0

				   Unless required by applicable law or agreed to in writing, software
				   distributed under the License is distributed on an "AS IS" BASIS,
				   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
				   See the License for the specific language governing permissions and
				   limitations under the License.


*********************************************************************/
#include "StdAfx.h"
#include "PhysicalConnection.h"

#ifdef Plateform_Windows
#include "IOQueue_Win.h"
#else

#ifdef Plateform_Linux
#include "IOQueue_Linux.h"
#endif
#endif


#include "../include/ConnectionContext.h"
#include "../include/ServerOptions.h"
#include "../include/Server.h"
#include "ServerImpl.h"
#include "Dispatcher.h"
#include "ServerStats.h"
#include "ScopedLock.h"
#include "Acceptor.h"
#include "MonitorAcceptor.h"
#include "ProtocolManager.h"
#include "MonitorsMsgFactory.h"
#include "LogicalConnectionImpl.h"
#include "ConnectionContextPool.h"

namespace PushFramework
{
PhysicalConnection::PhysicalConnection():
	oBuffer(RecyclableBuffer::Multiple)
#ifdef Plateform_Windows
	,readIoBuffer(IORead)
	,writeIoBuffer(IOWrite)
#endif
{
	pListenerOptions = NULL;
	pConnectionContext = NULL;

    ::InitializeCriticalSection(&csLock);
}

void PhysicalConnection::reset( SOCKET s, SOCKADDR_IN address, bool bIsObserver, ListenerOptions* pListenerOptions )
{
	this->socket = s;

	rPeerPort = address.sin_port;
	rPeerIP = inet_ntoa(address.sin_addr);

	this->pListenerOptions = pListenerOptions;

	dtCreationTime = time(NULL);

	bWriteInProgress = FALSE;

	this->bIsObserver = bIsObserver;

	status = Connected;

	ioWorkersReferenceCounter = 0;
}


void PhysicalConnection::Recycle()
{
	oBuffer.clearBytes();

#ifdef Plateform_Windows

	readIoBuffer.clearBytes();
	writeIoBuffer.clearBytes();
	
#endif

	if (pConnectionContext)
	{
		connectionContextPool.returnObject(pConnectionContext);
		pConnectionContext = NULL;
	}

	Connection::Recycle();

	pListenerOptions = NULL;
}

PhysicalConnection::~PhysicalConnection(void)
{
    ::DeleteCriticalSection(&csLock);
	delete pConnectionContext;
}

int PhysicalConnection::getStatus()
{
    return status;
}

double PhysicalConnection::getLifeDuration()
{
    time_t current = time(NULL);
    return difftime(current, dtCreationTime);
}
SendResult::Type PhysicalConnection::tryPushPacket( OutgoingPacket* pPacket )
{
    if (TryEnterCriticalSection(&csLock))
    {
        SendResult::Type result = pushPacketCommon(pPacket);
        LeaveCriticalSection(&csLock);
        return result;
    }
	return SendResult::Retry;
}
SendResult::Type PhysicalConnection::pushPacket( OutgoingPacket* pPacket )
{
    ScopedLock lock(csLock);
    return pushPacketCommon(pPacket);

}

void PhysicalConnection::SetConnectionContext( ConnectionContext* pConnectionContext )
{
    this->pConnectionContext = pConnectionContext;
}

ConnectionContext* PhysicalConnection::GetConnectionContext()
{
    return pConnectionContext;
}

void PhysicalConnection::attachToClient( LogicalConnectionImpl* pLogicalConnection )
{
    this->pLogicalConnection = pLogicalConnection;
    status = Attached;
}

bool PhysicalConnection::isObserverChannel()
{
    return bIsObserver;
}

UINT PhysicalConnection::getPeerPort()
{
    return rPeerPort;
}

std::string PhysicalConnection::getPeerIP()
{
    return rPeerIP;
}

Protocol* PhysicalConnection::getProtocol()
{
	return pListenerOptions->pProtocol;
}

LogicalConnectionImpl* PhysicalConnection::getLogicalConnectionImpl()
{
    return pLogicalConnection;
}

void PhysicalConnection::incrementIoWorkersReferenceCounter()
{
    AtomicIncrement(&ioWorkersReferenceCounter);
}

void PhysicalConnection::decrementIoWorkersReferenceCounter()
{
    AtomicDecrement(&ioWorkersReferenceCounter);
}

int PhysicalConnection::getIoWorkersReferenceCounter() const
{
    return ioWorkersReferenceCounter;
}

SOCKET PhysicalConnection::getSocket()
{
    return socket;
}


int PhysicalConnection::pushBytes( Buffer& buffer, Protocol* pProtocol )
{
	// This is called by protocol. There is no need to do statistics.
	if (status < Connected)
	{
		return SendResult::NotOK;
	}

	SerializeData serializeData(pProtocol);
	NetworkSerializeResult::Type result = theProtocolManager.serializeOutgoinBytes(*this, buffer, oBuffer, serializeData);
	
	if (result != NetworkSerializeResult::Success)
	{
		return result==NetworkSerializeResult::Retry ? SendResult::Retry : SendResult::NotOK;
	}

	if (!bWriteInProgress)
	{
		return WriteBytes() ? SendResult::OK : SendResult::NotOK;
	}

	return SendResult::OK;
}


SendResult::Type PhysicalConnection::pushPacketCommon( OutgoingPacket* pPacket )
{
    if (status < Connected)
    {
		return SendResult::NotOK;
    }

	double nBytesWritten = oBuffer.getDataSize();

	SerializeData serializeData(getProtocol());
	NetworkSerializeResult::Type result = theProtocolManager.serializeOutgoingPacket(*this, *pPacket, oBuffer, serializeData);

    if (result != NetworkSerializeResult::Success)
    {
		return result==NetworkSerializeResult::Retry ? SendResult::Retry : SendResult::NotOK;
    }

	nBytesWritten = oBuffer.getDataSize() - nBytesWritten;
    stats.addToCumul(ServerStats::BandwidthOutstanding, nBytesWritten);
    std::string serviceName;
    if (dispatcher.getCurrentService(serviceName))
    {
        stats.addToDistribution(ServerStats::BandwidthOutboundVolPerRequest, serviceName, nBytesWritten);
    }

    if (!bWriteInProgress)
    {
		return WriteBytes() ? SendResult::OK : SendResult::NotOK;
    }
	return SendResult::OK;
}

double PhysicalConnection::getTimeToLastReceive()
{
	time_t current = time(NULL);
	return difftime(current, dtLastReceiveTime);
}

Buffer& PhysicalConnection::GetSendBuffer()
{
	return oBuffer;
}

bool PhysicalConnection::IsWriteInProgress()
{
	return bWriteInProgress;
}

void PhysicalConnection::InitializeConnection()
{
	if (isObserverChannel())
	{
		return;
	}

	if (options.challengeClients)
	{
		pConnectionContext = (ConnectionContext*) connectionContextPool.borrowObject();
	}

	OutgoingPacket* pMsg = pServerImpl->getFacade()->getChallenge(pConnectionContext);

	if (pMsg)
	{
		pushPacket(pMsg);
		pServerImpl->getMessageFactory().disposeOutgoingPacket(pMsg); // We call the msg factory of clients not monitors.
	}
}

MessageFactory& PhysicalConnection::getMessageFactory()
{
	return isObserverChannel() ? theMonitorsMsgFactory : pServerImpl->getMessageFactory();
}






}
