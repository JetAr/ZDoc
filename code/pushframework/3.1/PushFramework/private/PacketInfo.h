/********************************************************************
	File :			PacketInfo.h
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
#pragma once

#include "../include/PushFramework.h"
namespace PushFramework
{

typedef struct PacketInfo
{
    PacketInfo(OutgoingPacket* pPacket, BROADCASTPACKET_KEY killKey, int objectCategory, int packetId);
    int packetId;
    OutgoingPacket* pPacket;
    std::string killKey;
    int objectCategory;
    unsigned int refCount;
    bool bWaitingForRemoval;
    PacketInfo* pNext;
	time_t creationTime;
	unsigned int getLife()
	{
		time_t current = time(NULL);
		return (unsigned int) difftime(current, creationTime);
	}
} PacketInfo;

typedef vector<PacketInfo*> packetInfoListT;

}
