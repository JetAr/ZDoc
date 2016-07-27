/********************************************************************
	File :			BroadcastManagerImpl.cpp
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
#include "BroadcastManager.h"

#include "../include/Server.h"
#include "../include/LogicalConnection.h"

#include "ServerStats.h"
#include "ServerImpl.h"
#include "ClientFactory.h"
#include "ServerImpl.h"
#include "BroadcastStreamer.h"
#include "Acceptor.h"

namespace PushFramework
{

BroadcastManager broadcastManager;

BroadcastManager::BroadcastManager(void)
{
}

BroadcastManager::~BroadcastManager(void)
{
}

void BroadcastManager::preEncodeOutgoingPacket( OutgoingPacket* pPacket )
{
	pServerImpl->getMessageFactory().preSerializeMessage(*pPacket);
}

void BroadcastManager::deleteOutgoingPacket( OutgoingPacket* pPacket )
{
	pServerImpl->getMessageFactory().disposeOutgoingPacket(pPacket);
}

void BroadcastManager::activateSubscribers( std::string channelName )
{
    pServerImpl->reshuffleStreamers();
}

void BroadcastManager::hanldeOnBeforePushPacket( std::string channelName )
{
    stats.addToDistribution(ServerStats::QoSFillRatePerChannel,channelName, 1);
}

void BroadcastManager::handleOnAfterPacketIsSent( std::string channelName, std::string subscriberKey )
{
    stats.addToDistribution(ServerStats::QoSSendRatePerChannel,channelName, 1);
    stats.addToKeyedDistributionDuration(ServerStats::QoSAvgSendRatePerChannel, channelName, subscriberKey, 1);
}

}

