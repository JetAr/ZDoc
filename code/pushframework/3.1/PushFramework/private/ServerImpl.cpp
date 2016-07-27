/********************************************************************
	File :			ServerImpl.cpp
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
#include "ServerImpl.h"

#include "../include/Server.h"
#include "../include/ServerOptions.h"
#include "LogicalConnectionImpl.h"
#include "LogicalConnectionPool.h"

#ifdef Plateform_Windows
#include "IOQueue_Win.h"
#else
#ifdef Plateform_Linux
#include "IOQueue_Linux.h"
#endif
#endif


#include "Demultiplexor.h"
#include "Dispatcher.h"
#include "ServerStats.h"
#include "Acceptor.h"
#include "MonitorAcceptor.h"
#include "ClientFactory.h"
#include "BroadcastManager.h"
#include "MonitorsBroadcastManager.h"
#include "GarbageCollector.h"
#include "BroadcastStreamer.h"
#include "PhysicalConnectionPool.h"
#include "ConnectionContextPool.h"


#ifdef Plateform_Windows
#include <mmsystem.h>
#endif
#include "Utilities.h"


namespace PushFramework
{

ServerImpl *pServerImpl = NULL;

#ifdef Plateform_Windows
void ServerImpl::eternal_loop()
{
     pFacade->OnStarted();

    //Periodic Timer
    m_hEventTimer = CreateEvent(NULL, TRUE, FALSE, NULL);
    ResetEvent(m_hEventTimer);

    //Abort event. (set by stop function)
    m_hEventAbort = CreateEvent(NULL, TRUE, FALSE, NULL);
    ResetEvent(m_hEventAbort);
    //

    HANDLE			hWaits[2];
    hWaits[0]		= m_hEventTimer;
    hWaits[1]		= m_hEventAbort;

    UINT			sleepTimeMs = 1000; //1 second.

    nSecsGCCount=0;
    nSecsPerfObservCount=0;
    //

    while(true)
    {
        MMRESULT result = timeSetEvent(sleepTimeMs, 1,
                                       (LPTIMECALLBACK) m_hEventTimer, 0, TIME_ONESHOT|TIME_CALLBACK_EVENT_SET);
        _ASSERT(result != NULL);

        // Sleep here until the timer interval ends or abort occurs
        if(WaitForMultipleObjects(2, hWaits, FALSE, INFINITE)==0)
        {

            DoPeriodicJobs();
            ResetEvent(m_hEventTimer);
        }
        else
        {
            //m_hEventAbort is set : break from while.
            break;
        }
        //
    }

    CloseHandle(m_hEventAbort);
    CloseHandle(m_hEventTimer);
    //quit blocking.
}

DWORD WINAPI ServerImpl::threadProc( LPVOID WorkContext )
{
    ServerImpl* pMe = reinterpret_cast<ServerImpl*>(WorkContext);
    pMe->eternal_loop();
    return 0;
}
#endif


#ifdef Plateform_Linux
void ServerImpl::eternal_loop()
{
    pFacade->OnStarted();

    int sec = 1;
    struct timespec ts;
    pthread_mutex_t cs;
    pthread_mutex_init(&cs, NULL);
    pthread_mutex_lock(&cs);
    pthread_cond_init(&abortCond, NULL);

    nSecsGCCount = 0;
    nSecsPerfObservCount = 0;

    while (true)
    {
        ::clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += sec;

        int rc = pthread_cond_timedwait(&abortCond, &cs, &ts);
        if (rc == ETIMEDOUT)
        {
           DoPeriodicJobs();
        }
        else
        {
            //Server is to die.
            break;
        }

    }


    pthread_mutex_unlock(&cs);

}
void* ServerImpl::threadProc(void* WorkContext)
{
    ServerImpl* pMe = reinterpret_cast<ServerImpl*> (WorkContext);
    pMe->eternal_loop();
    return 0;
}
#endif


bool ServerImpl::start( bool startInSeparateThread )
{
	unsigned int nMaxPoolConnections = options.nMaxConnections + 10;
	unsigned int nMaxThreads =  options.nWorkersCount + 20;

	//
	unsigned int nMessageSize = calculateMaxBufferSizePerMessage();
	unsigned int nMaxAdditionalMsgBuffersForProtocols = calculateAdditionalBuffersForProtocols(nMaxPoolConnections, nMaxThreads);
	if (!pool.create(RecyclableBuffer::Single, nMaxThreads*4 + nMaxAdditionalMsgBuffersForProtocols, nMessageSize))
		return false;

	if(!pool.create(RecyclableBuffer::Double, nMaxPoolConnections, nMessageSize*2))
		return false;

	if(!pool.create(RecyclableBuffer::Multiple, nMaxPoolConnections, options.nMaxPendingOutgoingMessages*nMessageSize))
		return false;
	
	if(!pool.create(RecyclableBuffer::Socket, nMaxPoolConnections*2, options.nSocketBufferSize))
		return false;

	if (!thePhysicalConnectionPool.initialize(nMaxPoolConnections))
		return false;

	if (!logicalConnectionPool.initialize(nMaxPoolConnections))
		return false;

	if (options.challengeClients)
	{
		if(!connectionContextPool.initialize(nMaxPoolConnections))
			return false;
	}

	if(!initializeProtocolContextPools(nMaxPoolConnections))
		return false;

    if (!ioQueue.create())
        return false;

    if (!demux.start())
    {
        ioQueue.free();
        return false;
    }

    if (!StartListening())
    {
        demux.stop();
        ioQueue.free();
		return false;
    }


    if ( options.isMonitorEnabled)
    {
		monitorAcceptor.setListeningPort(options.monitorPort);
		if (!monitorAcceptor.startListening())
		{
			StopListening();
			demux.stop();
			ioQueue.free();
			return false;
		}

		monitorsStreamer = new BroadcastStreamer();
		monitorsStreamer->start();
    }

    startStreamers();

    if (startInSeparateThread)
    {
#ifdef Plateform_Windows
        DWORD  nThreadID;

        hThread = CreateThread(NULL,// Security
                                         0,						// Stack size - use default
                                         threadProc,     		// Thread fn entry point
                                         (void*) this,			// Param for thread
                                         0,						// Init flag
                                         &nThreadID);			// Thread address
#else
#ifdef Plateform_Linux
        pthread_create(&hThread, NULL, threadProc, (void*) this);
#endif
#endif
    }
    else
        eternal_loop();

	return true;
}

void ServerImpl::stop()
{
#ifdef Plateform_Windows
    SetEvent(m_hEventAbort);//stops eternal_loop
    WaitForSingleObject(hThread, INFINITE);
#else
#ifdef Plateform_Linux
    pthread_cond_signal(&abortCond);
#endif
#endif

    if (options.isMonitorEnabled)
	{
        monitorAcceptor.stopListening();
		monitorsStreamer->stop();
		delete monitorsStreamer;
	}

    StopListening();

    demux.stop();

    clientFactoryImpl.stop();

	stopStreamers();

    broadcastManager.disposeAllPackets();
	monitorBroadcastManager.disposeAllPackets();

    ioQueue.free();

    garbageCollector.activate(true);
}

void ServerImpl::registerService(Service* pService)
{
    dispatcher.registerService(pService);
}

ServerImpl::ServerImpl( Server* pFacade )
{
#ifdef Plateform_Windows
    //Initialize WinSock :
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
#endif

    this->pFacade = pFacade;

    uGCPeriod = 10;

    pServerImpl = this;

	monitorsStreamer = NULL;

    nSecsGCCount=0;
    nSecsPerfObservCount=0;
}

ServerImpl::~ServerImpl( void )
{

}

Server* ServerImpl::getFacade()
{
    return pFacade;
}

void ServerImpl::setServeInfos( const char* serverInfos )
{
    this->serverInfos = serverInfos;
}

std::string ServerImpl::getServerInfos()
{
    return serverInfos;
}

void ServerImpl::pause()
{
	StopListening();
}

void ServerImpl::resume()
{
	StartListening();
}

bool ServerImpl::createListener( int port, ListenerOptions* pOptions /*= NULL*/ )
{
	listenersMapT::iterator it = listenersMap.find(port);
	if (it != listenersMap.end())
	{
		return false;
	}

	Acceptor* pAcceptor = new Acceptor();
	pAcceptor->setListeningPort(port);
	pAcceptor->setOptions(*pOptions);

	listenersMap[port] = pAcceptor;
    return true;
}

void ServerImpl::DoPeriodicJobs()
{
    nSecsGCCount++;
    nSecsPerfObservCount++;

    if(nSecsGCCount == uGCPeriod)
    {
        nSecsGCCount = 0;
        garbageCollector.activate();
        clientFactoryImpl.scrutinize();
    }

    if(nSecsPerfObservCount == options.samplingRate && options.isProfilingEnabled)
    {
        nSecsPerfObservCount=0;
        OutgoingPacket* pPacket = stats.getPerformancePacket();
        monitorBroadcastManager.pushPacket(pPacket, "stats");
    }
}

void ServerImpl::setMessageFactory( MessageFactory* pMsgFactory )
{
	this->pMsgFactory = pMsgFactory;
}

MessageFactory& ServerImpl::getMessageFactory()
{
	return *pMsgFactory;
}

void ServerImpl::startStreamers()
{
	for (unsigned int i=0; i<options.nStreamers; i++)
	{
		BroadcastStreamer* pStreamer = new BroadcastStreamer();
		streamersList.push_back( pStreamer);
		pStreamer->start();
	}
}

void ServerImpl::stopStreamers()
{
	streamersListT::iterator it = streamersList.begin();
	while (it!=streamersList.end())
	{
		BroadcastStreamer* pStreamer = *it;
		pStreamer->stop();
		delete pStreamer;
		it = streamersList.erase(it);
	}
}

void ServerImpl::reshuffleStreamers()
{
	streamersListT::iterator it = streamersList.begin();
	while (it!=streamersList.end())
	{
		BroadcastStreamer* pStreamer = *it;
		pStreamer->awakeAll();
		it++;
	}
}

void ServerImpl::addClientToStreamers( LogicalConnectionImpl* pLogicalConnection )
{
	BroadcastManagerBase& broadcastManagerRef = pLogicalConnection->IsMonitor() ? (BroadcastManagerBase&) monitorBroadcastManager :  (BroadcastManagerBase&) broadcastManager;
	vector<string>& globalQueues = broadcastManagerRef.getGlobalQueues();

	for (int i=0;i<globalQueues.size();i++)
	{
		pLogicalConnection->SubscribeToQueue(globalQueues[i].c_str());
	}

	if (pLogicalConnection->IsMonitor())
	{
		monitorsStreamer->addItem(pLogicalConnection);
		return;
	}

	BroadcastStreamer* pLessBusy = NULL;

	streamersListT::iterator it = streamersList.begin();
	while (it!=streamersList.end())
	{
		if (pLessBusy)
		{
			if (pLessBusy->getItemsCount() > (*it)->getItemsCount())
			{
				pLessBusy = (*it);
			}
		}
		else
			pLessBusy = (*it);
		it++;
	}

	if (pLessBusy)
	{
		pLessBusy->addItem(pLogicalConnection);
		pLogicalConnection->pStreamer = pLessBusy;
	}
}

void ServerImpl::CreateQueue( BROADCASTQUEUE_NAME queueName, QueueOptions& queueOptions )
{
	broadcastManager.createBroadcastQueue(queueName, queueOptions);
}

void ServerImpl::RemoveQueue( BROADCASTQUEUE_NAME queueName )
{
	broadcastManager.removeBroadcastQueue(queueName);
}

bool ServerImpl::PushPacket( OutgoingPacket* pPacket, BROADCASTQUEUE_NAME queueName )
{
	return broadcastManager.pushPacket(pPacket, queueName, "", 0);
}

bool ServerImpl::PushPacket( OutgoingPacket* pPacket, BROADCASTQUEUE_NAME queueName, BROADCASTPACKET_KEY killKey, int objectCategory )
{
	return broadcastManager.pushPacket(pPacket, queueName, killKey, objectCategory);
}

void ServerImpl::RemovePacketFromQueue( BROADCASTPACKET_KEY killKey, int objectCategory, BROADCASTQUEUE_NAME queueName )
{
	broadcastManager.removePacket(killKey, objectCategory, queueName);
}

void ServerImpl::StopListening()
{
	listenersMapT::iterator it = listenersMap.begin();
	while (it != listenersMap.end())
	{
		Acceptor* pAcceptor = it->second;
		pAcceptor->stopListening();
		it++;
	}
}

bool ServerImpl::StartListening()
{
	listenersMapT::iterator it = listenersMap.begin();
	while (it != listenersMap.end())
	{
		Acceptor* pAcceptor = it->second;
		if (!pAcceptor->startListening())
		{
			StopListening();
			return false;
		}
		it++;
	}
	return true;
}

void ServerImpl::reshuffleMonitorsStreamer()
{
	monitorsStreamer->awakeAll();
}

unsigned int ServerImpl::calculateMaxBufferSizePerMessage()
{
	unsigned int nMaxSize = 0;

	listenersMapT::iterator it = listenersMap.begin();
	while (it != listenersMap.end())
	{
		Acceptor* pAcceptor = it->second;

		unsigned int nSize = pMsgFactory->getMaximumMessageSize();
		Protocol* pProtocol = pAcceptor->getProtocol();
		while (pProtocol)
		{
			nSize = pProtocol->getRequiredOutputSize(nSize);
			pProtocol = pProtocol->getLowerProtocol();
		}

		if (nSize > nMaxSize)
		{
			nMaxSize = nSize;
		}

		it++;
	}

	return nMaxSize;
}

unsigned int ServerImpl::calculateAdditionalBuffersForProtocols(unsigned int nMaxPoolConnections , unsigned int nMaxThreads)
{
	unsigned int nTotal = 0;

	listenersMapT::iterator it = listenersMap.begin();
	while (it != listenersMap.end())
	{
		Acceptor* pAcceptor = it->second;

		nTotal += pAcceptor->getProtocol()->getRequiredRecyclableBuffers(nMaxPoolConnections, nMaxThreads);

		it++;
	}

	return nTotal;
}

bool ServerImpl::initializeProtocolContextPools( unsigned int nMaxPoolConnections )
{
	listenersMapT::iterator it = listenersMap.begin();
	while (it != listenersMap.end())
	{
		Acceptor* pAcceptor = it->second;
		if (!pAcceptor->getProtocol()->initialize(nMaxPoolConnections))
			return false;
		it++;
	}

	return true;
}


}

