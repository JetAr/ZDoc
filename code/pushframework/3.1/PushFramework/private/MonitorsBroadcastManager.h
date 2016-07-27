/********************************************************************
	File :			MonitorsBroadcastManager.h
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
#ifndef MonitorsBroadcastManager__INCLUDED
#define MonitorsBroadcastManager__INCLUDED

#pragma once

#include "../include/PushFramework.h"

#include "BroadcastManagerBase.h"
#include "PhysicalConnection.h"
namespace PushFramework
{


class MonitorsBroadcastManager : public BroadcastManagerBase
{
public:
    MonitorsBroadcastManager();
    virtual ~MonitorsBroadcastManager();

protected:
    virtual void preEncodeOutgoingPacket(OutgoingPacket* pPacket);
    virtual void deleteOutgoingPacket(OutgoingPacket* pPacket);
	virtual void activateSubscribers(std::string channelName);
};
extern MonitorsBroadcastManager monitorBroadcastManager;
}
#endif // MonitorsBroadcastManager__INCLUDED
