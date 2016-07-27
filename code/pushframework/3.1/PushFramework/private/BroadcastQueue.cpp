/********************************************************************
	File :			BroadcastQueue.cpp
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
#include "BroadcastQueue.h"

#include "ScopedLock.h"
#include "BroadcastManagerBase.h"

namespace PushFramework
{


BroadcastQueue::BroadcastQueue(BroadcastManagerBase* pManager, std::string channelName, QueueOptions& queueOptions)
{
    this->pManager = pManager;
    this->channelName = channelName;
    this->queueOptions = queueOptions;

    ::InitializeCriticalSection(&cs);
    cumulatedCount = 0;
    lastGenId = 0;
}

BroadcastQueue::~BroadcastQueue(void)
{
    ::DeleteCriticalSection(&cs);
}


bool BroadcastQueue::pushPacket( OutgoingPacket* pPacket, BROADCASTPACKET_KEY killKey/*=""*/, int objectCategory /*=0*/ )
{
    ScopedLock csLock(cs);

    if (!checkAgainstMaxFillRate())
    {
        return false;
    }

    PacketInfo* pPacketInfo = new PacketInfo(pPacket, killKey, objectCategory, ++lastGenId);

	activeList.push_back(pPacketInfo);
	if (activeList.size() > queueOptions.maxPackets)
	{
		PacketInfo* head = activeList.front();
		activeList.erase(activeList.begin());

		internalRemove(head);
	}
	return true;
}

void BroadcastQueue::removePacket( BROADCASTPACKET_KEY killKey/*=""*/, int objectCategory /*=0*/ )
{
    ScopedLock csLock(cs);

	packetInfoListT::iterator it = activeList.begin();
	while (it!=activeList.end())
	{
		PacketInfo* pInfo = *it;
		if (pInfo->killKey == killKey && pInfo->objectCategory == objectCategory)
		{
			internalRemove(pInfo);
			activeList.erase(it);
			return;
		}
		it++;
	}
}

void BroadcastQueue::subscribeClient( std::string clientKey, bool ignorePreviousPackets/* = false */)
{
    ScopedLock csLock(cs);

	clientMap[clientKey] = ignorePreviousPackets ? lastGenId : 0;
}

void BroadcastQueue::unsubscribeClient( std::string clientKey )
{
    ScopedLock csLock(cs);

    clientMapT::iterator it = clientMap.find(clientKey);
    if (it!=clientMap.end())
        clientMap.erase(it);
}

void BroadcastQueue::disposePacket( PacketInfo* pPacketInfo)
{
    ScopedLock csLock(cs);

    //In all cases dec refCount :
    pPacketInfo->refCount--;

    //Check if item is in internal garbage list :
    if (pPacketInfo->bWaitingForRemoval && pPacketInfo->refCount == 0)
    {
        std::vector<PacketInfo*>::iterator it = removedList.begin();
        while (it!=removedList.end())
        {
            if (pPacketInfo == (*it))
            {
				pManager->deleteOutgoingPacket(pPacketInfo->pPacket);
                delete pPacketInfo;
                it = removedList.erase(it);
            }
            else
                it++;
        }
    }
}

OutgoingPacket* BroadcastQueue::getNextPacket( int previouslyInsertedId, PacketInfo*& pPacketInfo )
{
	pPacketInfo = NULL;

    ScopedLock csLock(cs);

	packetInfoListT::iterator it = activeList.begin();

	PacketInfo* pSearchItem = NULL;
	while (it != activeList.end())
	{
		pSearchItem = *it;

		bool isNotSent = pSearchItem->packetId > previouslyInsertedId;
		if (isNotSent)
		{
			bool isNotExpired = queueOptions.maxExpireDurationSecs == 0 || pSearchItem->getLife() <= queueOptions.maxExpireDurationSecs;
			if (isNotExpired)
			{
				pPacketInfo = pSearchItem;
				break;
			}
		}

		it++;
	}

    if (pPacketInfo)
    {
        pPacketInfo->refCount++;
        return pPacketInfo->pPacket;
    }
    return NULL;
}


void BroadcastQueue::disposeAllPackets()
{
	//Delete all packets in the queue.
	for (unsigned int i=0; i<activeList.size(); i++)
	{
		PacketInfo* pTemp = activeList.at(i);
		pManager->deleteOutgoingPacket(pTemp->pPacket);
		delete pTemp;
	}
}

std::string BroadcastQueue::getChannelName()
{
    return channelName;
}

void BroadcastQueue::internalRemove( PacketInfo* pPacketInfo )
{
    if(pPacketInfo->refCount==0)
    {
        pManager->deleteOutgoingPacket(pPacketInfo->pPacket);
        delete pPacketInfo;
    }
    else
    {
        removedList.push_back(pPacketInfo);
        pPacketInfo->bWaitingForRemoval = true;
    }
}

bool BroadcastQueue::checkAgainstMaxFillRate()
{
    if (queueOptions.fillRateThrottlingPeriod != 0)
    {
        time_t timeNow = time(NULL);

        if (cumulatedCount == 0)
        {
            lastCountTime = timeNow;
            cumulatedCount = 1;
        }
        else
        {
            if (difftime(timeNow, lastCountTime) < queueOptions.fillRateThrottlingPeriod )
            {
                if (cumulatedCount == queueOptions.fillRateThrottlingMaxPackets)
                {
                    return false;
                }
                else
                {
                    cumulatedCount++;
                }
            }
            else
            {
                cumulatedCount = 1;
                lastCountTime = timeNow;
            }
        }
    }
    return true;
}

unsigned int BroadcastQueue::getLastGeneratedId()
{
	return lastGenId;
}

QueueOptions& BroadcastQueue::getOptions()
{
	return queueOptions;
}

}

