/********************************************************************
	File :			BroadcastManagerImplBase.h
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
#ifndef BroadcastManagerImplBase__INCLUDED
#define BroadcastManagerImplBase__INCLUDED

#pragma once

#include "../include/PushFramework.h"

namespace PushFramework
{

class BroadcastQueue;
struct PacketInfo;
class ServerImpl;
class LogicalConnection;
struct QueueOptions;

class BroadcastManagerBase
{
    friend class BroadcastQueue;
public:
    BroadcastManagerBase(void);
    virtual ~BroadcastManagerBase(void);

    void createBroadcastQueue(BROADCASTQUEUE_NAME channelKey, QueueOptions& queueOptions);
    void removeBroadcastQueue(BROADCASTQUEUE_NAME channelKey);
	BroadcastQueue* getQueue(BROADCASTQUEUE_NAME channelKey);
    bool pushPacket(OutgoingPacket* pPacket, BROADCASTQUEUE_NAME channelName, BROADCASTPACKET_KEY killKey = "", int objectCategory = 0);
    void removePacket(BROADCASTPACKET_KEY killKey, int objectCategory, BROADCASTQUEUE_NAME channelKey);
    void disposeAllPackets();
    std::string getQueuesNames();
	vector<string>& getGlobalQueues();

protected:

    virtual void preEncodeOutgoingPacket(OutgoingPacket* pPacket) = 0;
    virtual void deleteOutgoingPacket(OutgoingPacket* pPacket) = 0;
    virtual void activateSubscribers(std::string channelName) = 0;
    virtual void hanldeOnBeforePushPacket(std::string channelName) {};
    virtual void handleOnAfterPacketIsSent(std::string channelName, std::string subscriberKey) {};

private:
    typedef unordered_map<std::string, BroadcastQueue*> channelMapT;
    channelMapT channelMap;
	vector<string> globalQueues;
};

}







#endif // BroadcastManagerImplBase__INCLUDED
