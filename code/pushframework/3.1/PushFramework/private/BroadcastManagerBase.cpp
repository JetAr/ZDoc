/********************************************************************
	File :			BroadcastManagerImplBase.cpp
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
#include "BroadcastManagerBase.h"

#include "../include/LogicalConnection.h"
#include "BroadcastQueue.h"

namespace PushFramework
{


BroadcastManagerBase::BroadcastManagerBase(void)
{
}

BroadcastManagerBase::~BroadcastManagerBase(void)
{
	//Delete Queues : 
	for (channelMapT::iterator it = channelMap.begin(); it!=channelMap.end();it++)
	{
		BroadcastQueue* pQueue = it->second;
		delete pQueue;
	}
}

void BroadcastManagerBase::createBroadcastQueue( BROADCASTQUEUE_NAME channelKey, QueueOptions& queueOptions)
{
    BroadcastQueue* pQueue = new BroadcastQueue(this, channelKey, queueOptions);
    channelMap[channelKey] = pQueue;
	if (!queueOptions.requireRegistration)
	{
		globalQueues.push_back(channelKey);
	}
}

void BroadcastManagerBase::removeBroadcastQueue( BROADCASTQUEUE_NAME channelKey )
{
    channelMapT::iterator it = channelMap.find(channelKey);
    if(it!=channelMap.end())
    {

        BroadcastQueue* pQueue = it->second;
        delete pQueue;

        channelMap.erase(it);
    }
}


bool BroadcastManagerBase::pushPacket( OutgoingPacket* pPacket, BROADCASTQUEUE_NAME channelName, BROADCASTPACKET_KEY killKey, int objectCategory )
{
    channelMapT::iterator it = channelMap.find(channelName);
	if(it==channelMap.end()){
		deleteOutgoingPacket(pPacket);
        return false;
	}

    preEncodeOutgoingPacket(pPacket);
    hanldeOnBeforePushPacket(channelName);
    //
    BroadcastQueue* packetQueue = it->second;

    bool bRet = packetQueue->pushPacket(pPacket, killKey, objectCategory);
    if (bRet)
    {
        activateSubscribers(channelName);
    }
	else
		deleteOutgoingPacket(pPacket);
    return bRet;
}   

void BroadcastManagerBase::removePacket( BROADCASTPACKET_KEY killKey, int objectCategory, BROADCASTQUEUE_NAME channelKey )
{
    channelMapT::iterator it = channelMap.find(channelKey);
    if(it==channelMap.end())
        return;
    //
    BroadcastQueue* packetQueue = it->second;
    packetQueue->removePacket(killKey, objectCategory);
}

void BroadcastManagerBase::disposeAllPackets()
{
    for (channelMapT::iterator it = channelMap.begin();
            it!=channelMap.end();
            it++)
    {
        BroadcastQueue* pQueue = it->second;
        pQueue->disposeAllPackets();
    }
}

std::string BroadcastManagerBase::getQueuesNames()
{
    std::stringstream ss;
    ss << std::noskipws;

    for (channelMapT::iterator it = channelMap.begin();
            it!=channelMap.end();
            it++)
    {
        ss << "<queue val=\"" << it->first << "\"/>";
    }

    return ss.str();
}

BroadcastQueue* BroadcastManagerBase::getQueue( BROADCASTQUEUE_NAME channelKey )
{
	channelMapT::iterator it = channelMap.find(channelKey);
	if(it==channelMap.end())
		return NULL;
	//
	return it->second;
}

vector<string>& BroadcastManagerBase::getGlobalQueues()
{
	return globalQueues;
}

}
