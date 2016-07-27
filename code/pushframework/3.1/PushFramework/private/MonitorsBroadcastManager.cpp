/********************************************************************
	File :			MonitorsBroadcastManager.cpp
	Creation date :	2012/01/29

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
#include "MonitorsBroadcastManager.h"

#include "../include/QueueOptions.h"

#include "ClientFactory.h"
#include "XMLPacket.h"
#include "PhysicalConnection.h"
#include "ScopedLock.h"
#include "ServerImpl.h"

namespace PushFramework
{

MonitorsBroadcastManager monitorBroadcastManager;

MonitorsBroadcastManager::MonitorsBroadcastManager()
{
    QueueOptions options;
    //
    options.maxPackets = 100;
    options.requireRegistration = true;
    options.priority = 10;
    options.packetsQuota = 10;
    options.fillRateThrottlingPeriod = 60;
    options.fillRateThrottlingMaxPackets = 100;

    createBroadcastQueue("stats", options);

    options.maxPackets = 1000;
    options.priority = 5;
    options.packetsQuota = 5;

    createBroadcastQueue("clientsIn", options);

    options.maxPackets = 50;

    createBroadcastQueue("clientsOut", options);
}

MonitorsBroadcastManager::~MonitorsBroadcastManager( void )
{
}

void MonitorsBroadcastManager::preEncodeOutgoingPacket( OutgoingPacket* pPacket )
{
    //Nothing to do, packet is already encoded.
}

void MonitorsBroadcastManager::deleteOutgoingPacket( OutgoingPacket* pPacket )
{
	XMLPacket* pXml = (XMLPacket*) pPacket;
    delete pXml;
}

void MonitorsBroadcastManager::activateSubscribers( std::string channelName )
{
	pServerImpl->reshuffleMonitorsStreamer();
}

}

