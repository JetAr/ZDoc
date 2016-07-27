/********************************************************************
        File :			Dispatcher.cpp
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
#include "Dispatcher.h"


#include "../include/ListenerOptions.h"
#include "../include/LogicalConnection.h"
#include "../include/Service.h"
#include "../include/Server.h"
#include "../include/ServerOptions.h"

#include "ScopedLock.h"
#include "PhysicalConnection.h"
#include "ServerImpl.h"
#include "ServerStats.h"
#include "XMLPacket.h"
#include "ClientFactory.h"
#include "BroadcastManager.h"
#include "MonitorsBroadcastManager.h"
#include "BroadcastQueue.h"
#include "StopWatch.h"
#include "Utilities.h"
#include "Acceptor.h"
#include "MonitorAcceptor.h"
#include "MonitorsMsgFactory.h"
#include "ProtocolManager.h"
#include "MonitorConnection.h"
#include "LogicalConnectionImpl.h"
#include "LogicalConnectionPool.h"

namespace PushFramework
{

Dispatcher dispatcher;

Dispatcher::Dispatcher()
{
    ::InitializeCriticalSection(&csSrvMap);
}

Dispatcher::~Dispatcher(void)
{
    ::DeleteCriticalSection(&csSrvMap);
}

std::string Dispatcher::getServiceNames()
{
    std::stringstream ss;
    ss << std::noskipws;

    for (serviceMapT::iterator it = serviceMap.begin();
            it != serviceMap.end();
            it++)
    {
        ss << "<service val=\"" << it->second->getName() << "\"/>";
    }

    return ss.str();
}

void Dispatcher::setCurrentService(std::string serviceName)
{
    ScopedLock lock(csSrvMap);
    ThreadIDType dwThread = Utilities::getCurrentThreadID();

    workerServiceMap[dwThread] = serviceName;
}

void Dispatcher::unsetCurrentService()
{
    ScopedLock lock(csSrvMap);
    ThreadIDType dwThread = Utilities::getCurrentThreadID();

    workerServiceMapT::iterator it = workerServiceMap.find(dwThread);
    if (it != workerServiceMap.end())
        workerServiceMap.erase(it);
}

bool Dispatcher::getCurrentService(std::string& serviceName)
{
    ScopedLock lock(csSrvMap);
    ThreadIDType dwThread = Utilities::getCurrentThreadID();

    workerServiceMapT::iterator it = workerServiceMap.find(dwThread);
    if (it == workerServiceMap.end())
        return false;

    serviceName = it->second;
    return true;
}

void Dispatcher::notifyObserversClientIN(const char* key, std::string peerIP, unsigned int peerPort)
{
	if (!options.isProfilingEnabled)
		return;

    std::string timestamp = Utilities::getCurrentTime();

    XMLPacket *response = new XMLPacket(AnalyticsProtocol::VisitorInResponse);
    response->setArgumentAsText("time", timestamp.c_str());
    response->setArgumentAsText("name", key);
    response->setArgumentAsText("ip", peerIP.c_str());
    response->setArgumentAsInt("port", peerPort);

    monitorBroadcastManager.pushPacket(response, "clientsIn", key, 0);
}

void Dispatcher::notifyObserversClientOut(const char* key)
{
	if (!options.isProfilingEnabled)
		return;
    
	XMLPacket *response = new XMLPacket(AnalyticsProtocol::VisitorOutResponse);
    response->setArgumentAsText("name", key);

    monitorBroadcastManager.pushPacket(response, "clientsOut");

    //Remove client from the other broadcast group :
    monitorBroadcastManager.removePacket(key, 0, "clientsIn");
}

void Dispatcher::handleInitialize(PhysicalConnection& connection)
{
	RecyclableBuffer protocolBytes;
	Protocol* pProtocol = connection.AdvanceInitialization(protocolBytes);
	if (pProtocol)
	{
		connection.pushBytes(protocolBytes, pProtocol->getLowerProtocol());
	}

	connection.CheckConnectionInitialization();


    connection.postReceive();
    //
}

void Dispatcher::registerService(Service* pService)
{
    serviceMap[pService->getRoutingId()] = pService;
}


void Dispatcher::processFirstRequest( PhysicalConnection& connection, IncomingPacket& packet, int nRoutingService, unsigned int serviceBytes )
{
    OutgoingPacket* pOutPacket = NULL;

	LoginData loginData;
	loginData.connectionContext = connection.GetConnectionContext();
	loginData.pRequest = &packet;

	LogicalConnectionImpl* newClient = (LogicalConnectionImpl*) logicalConnectionPool.borrowObject();

	Login::Type type = newClient->processLogin(loginData);

	if (!Login::IsSucceeded(type))
	{
		bool bWaitForSendToComplete = (loginData.pResponse != NULL);
		if (loginData.pResponse)
		{
			connection.pushPacket(loginData.pResponse);
			pServerImpl->getMessageFactory().disposeOutgoingPacket(loginData.pResponse);
		}

		if (type == Login::RefuseAndClose)
		{
			connection.closeConnection(bWaitForSendToComplete);
		}

		logicalConnectionPool.returnObject(newClient);
		return;
	}

	//At this moment, claim the physical connection:
	if (!clientFactoryImpl.removePhysicalConnection(&connection))
	{
		logicalConnectionPool.returnObject(newClient);
		return;
	}

	newClient->initializeIntern(&connection);

	pServerImpl->addClientToStreamers(newClient);

	//type == Login::AcceptClient.
	clientFactoryImpl.addLogicalConnection(newClient);

	newClient->OnConnected();

	if (type == Login::AcceptClientAndRouteRequest)
	{
		DispatchRequest(connection, loginData.pRequest, nRoutingService, serviceBytes);
	}
	
	//Statistics :
	stats.addToCumul(ServerStats::VisitorsHitsIn, 1);
	//notifyObserversClientIN(addedClient->getKey(), connection.getPeerIP(), connection.getPeerPort());
	stats.addToCumul(ServerStats::VisitorsOnline, 1);
    

    newClient->DecrementUsage();
}

void Dispatcher::handleMonitorRequest(PhysicalConnection& connection, IncomingPacket& packet)
{
    XMLPacket& requestPacket = (XMLPacket&) packet;

    int typeId = requestPacket.getTypeId();

    if (typeId == AnalyticsProtocol::LiveSubscriptionRequest)
    {
        bool bSubscribe = requestPacket.getArgumentAsBool("resume");
        if (bSubscribe)
        {
            OutgoingPacket* pInitPacket = stats.getInitializationPacket();
            connection.pushPacket(pInitPacket);
			theMonitorsMsgFactory.disposeOutgoingPacket(pInitPacket);

            //
/*
            monitorBroadcastManager.subscribeClient(connection->getLogicalConnectionKey(), "stats");
            monitorBroadcastManager.subscribeClient(connection->getLogicalConnectionKey(), "clientsIn");
            monitorBroadcastManager.subscribeClient(connection->getLogicalConnectionKey(), "clientsOut");
*/
        }
        else
        {
            //monitorBroadcastManager.removeClient(connection->getLogicalConnectionKey());
        }
    }


    if (typeId == AnalyticsProtocol::LogoutRequest)
    {
        connection.closeConnection(false);
    }

    if (typeId == AnalyticsProtocol::ConsoleCommandRequest)
    {
        std::string command = requestPacket.getArgumentAsText("command");

        XMLPacket response(AnalyticsProtocol::ConsoleCommandResponse);

        response.setArgumentAsText("client", requestPacket.getArgumentAsText("client"));

        if (command == "about")
        {
            std::string str = pServerImpl->getServerInfos() + "\nBased on PushFramework version 1.0";
            response.setArgumentAsText("console", str.c_str());
        }

        if (command == "profiling enable")
        {
            std::string str;
            if (options.isProfilingEnabled)
            {
                str = "Profiling is already enabled.";
            }
            else
            {
                options.isProfilingEnabled = false;
                str = "Profiling was enabled.";
            }
            response.setArgumentAsText("console", str.c_str());
        }

        if (command == "profiling disable")
        {
            std::string str;
            if (!options.isProfilingEnabled)
            {
                str = "Profiling is already disabled.";
            }
            else
            {
                options.isProfilingEnabled = false;
                str = "Profiling was disabled.";
            }
            response.setArgumentAsText("console", str.c_str());
        }

        if (command == "profiling status")
        {
            response.setArgumentAsText("console", (!options.isProfilingEnabled) ? "Profiling was found to be disabled." :
                                       "Profiling was found to be enabled.");
        }

        char pOut[256];

        bool bRet = pServerImpl->getFacade()->handleMonitorRequest(command.c_str(), pOut);

        if (bRet)
        {
            response.setArgumentAsText("console", pOut);
        }

        connection.pushPacket(&response);
    }

}

void Dispatcher::processMonitorFirstRequest(PhysicalConnection& connection, IncomingPacket& packet)
{
    XMLPacket& requestPacket = (XMLPacket&) packet;

    //
    XMLPacket response(AnalyticsProtocol::LoginResponse);
    std::string password = requestPacket.getArgumentAsText("password");

	if (password != options.password)
	{
		XMLPacket response(AnalyticsProtocol::LoginResponse);
		response.setArgumentAsText("result", "pass");
		connection.pushPacket(&response);
		return;
	}

	if (!clientFactoryImpl.removePhysicalConnection(&connection))
	{
		return;
	}

	static int observerId = 1;
	observerId++;
	std::stringstream ss;
	ss << observerId;

	string key = ss.str();
	MonitorConnection* pMonitor = new MonitorConnection(key);
	pMonitor->initializeIntern(&connection);

	//Add client to a streamer :
	pServerImpl->addClientToStreamers(pMonitor);

	response.setArgumentAsText("result", "ok");
	connection.pushPacket(&response);

	pMonitor->DecrementUsage();
}

void Dispatcher::handleFailedIO( PhysicalConnection& connection )
{
	connection.decrementIoWorkersReferenceCounter();
}


void Dispatcher::handleRead(PhysicalConnection& connection, int dwIoSize)
{
	int status = connection.getStatus();
	bool isClient = !connection.isObserverChannel();

	bool isFailed = false;
	bool waitForPendingPackets = false;

	RecyclableBuffer incomingBytes(RecyclableBuffer::UnAllocated);
	isFailed = !connection.readReceivedBytes(incomingBytes, dwIoSize);

	if (!isFailed)
	{
		isFailed = !theProtocolManager.processIncomingData(connection, incomingBytes);
	}

	//The Processing Loop.
	DeserializeData deserializeData(connection.getProtocol()->getLowestProtocol());
	NetworkDeserializeResult::Type result;
	while (!isFailed)
	{
		deserializeData.clear();
		result = theProtocolManager.tryDeserializeIncomingPacket(connection, deserializeData);
		
		if (result == NetworkDeserializeResult::ProtocolBytes)
		{	
			if (deserializeData.protocolBytes.hasBytes())
			{
				connection.pushBytes(deserializeData.protocolBytes, deserializeData.pProtocol->getLowerProtocol());
			}

			continue;
		}
		else if (result == NetworkDeserializeResult::Close)
		{
			if (deserializeData.protocolBytes.hasBytes())
			{
				connection.pushBytes(deserializeData.protocolBytes, deserializeData.pProtocol->getLowerProtocol());
			}

			isFailed = true;
			waitForPendingPackets = true;
		}
		else if (result == NetworkDeserializeResult::Content)
		{
			handleRequest(connection, deserializeData.pMessage, deserializeData.nRoutingService, 0);//TODO
			connection.getMessageFactory().disposeIncomingPacket(deserializeData.pMessage);
		}
		else if(result == NetworkDeserializeResult::WantMoreData)
		{
			connection.postReceive();
			break;
		}
		else
			/*result == NetworkDeserializeResult::Failure or 
			NetworkDeserializeResult::Initializationfailure*/
		{
			isFailed = true;
		}		
	}

	if (!isFailed)
	{
		connection.CheckConnectionInitialization();
	}
	
	if (isFailed)
	{
		if (status == PhysicalConnection::Attached)
		{
			clientFactoryImpl.disconnect(connection.getLogicalConnectionImpl(), waitForPendingPackets, DisconnectionReason::PeerClosure);
		}
		else
		{
			connection.closeConnection(waitForPendingPackets);
		}
	}

	connection.decrementIoWorkersReferenceCounter();
}

void Dispatcher::handleWrite( PhysicalConnection& connection, int dwIoSize )
{
	bool bIsBufferIdle = false;
	int status = connection.getStatus();

	if (!connection.OnSendCompleted(dwIoSize, bIsBufferIdle))
	{
		if (status == PhysicalConnection::Attached)
		{
			clientFactoryImpl.disconnect(connection.getLogicalConnectionImpl(), false, DisconnectionReason::PeerClosure);
		}
		else
		{
			connection.closeConnection(false);
		}

		connection.decrementIoWorkersReferenceCounter();
		return;
	}

	if (status == PhysicalConnection::Attached)
	{
		if (bIsBufferIdle)
		{
			connection.getLogicalConnectionImpl()->CheckAndProcessPendingBroadcast(false);
		}

		if (!connection.isObserverChannel())
		{
			connection.getLogicalConnectionImpl()->OnReadyForSend(connection.GetSendBuffer().getRemainingSize());
		}
	}

	connection.decrementIoWorkersReferenceCounter();
}


void Dispatcher::handleRequest( PhysicalConnection& connection, IncomingPacket* pPacket, int nRoutingService, unsigned int serviceBytes )
{
	if (connection.getStatus() == PhysicalConnection::Connected)
	{
		if (!connection.isObserverChannel())
			processFirstRequest(connection, *pPacket, nRoutingService, serviceBytes);
		else
			processMonitorFirstRequest(connection, *pPacket);

		return;
	}

	DispatchRequest(connection, pPacket, nRoutingService, serviceBytes);
	return;
}

void Dispatcher::DispatchRequest( PhysicalConnection &connection, IncomingPacket* pPacket, int nRoutingService, unsigned int serviceBytes )
{
	if (connection.getStatus() != PhysicalConnection::Attached)
	{
		return;
	}

	if (connection.isObserverChannel())
	{
		handleMonitorRequest(connection, *pPacket);
		return;
	}


	LogicalConnectionImpl* pClient = connection.getLogicalConnectionImpl();

	//
	serviceMapT::iterator it = serviceMap.find(nRoutingService);
	if (it == serviceMap.end())
	{
		pClient->getFacade()->handleRequest(pPacket);
		return;
	}
	//

	Service* pService = it->second;

	setCurrentService(pService->getName());

	StopWatch watch;
	pService->handle(pClient->getFacade(), pPacket);


	double duration = watch.getElapsedTime();
	stats.addToDistribution(ServerStats::PerformanceProcessingTimePerService, pService->getName(), duration);

	stats.addToDuration(ServerStats::PerformanceProcessingTime, duration);

	unsetCurrentService();

	//Stats. :

	stats.addToDistribution(ServerStats::BandwidthInboundVolPerRequest, pService->getName(), serviceBytes);
	stats.addToDistribution(ServerStats::PerformanceRequestVolPerRequest, pService->getName(), 1);
}

}

