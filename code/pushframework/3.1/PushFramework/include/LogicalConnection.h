/********************************************************************
	File :			LogicalConnection.h
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
#ifndef LogicalConnection__INCLUDED
#define LogicalConnection__INCLUDED

#pragma once

#include "PushFramework.h"

namespace PushFramework
{
	
	class LogicalConnectionImpl;
	class Dispatcher;
	class PUSHFRAMEWORK_API LogicalConnection
	{
		friend class LogicalConnectionImpl;	
/*		friend class Dispatcher;*/
	
	public:
		LogicalConnection(void);
		virtual ~LogicalConnection(void);

		SendResult::Type PushPacket(OutgoingPacket* pPacket);
		SendResult::Type TryPushPacket(OutgoingPacket* pPacket);
		void Disconnect(bool waitForPendingPackets);
		
		double getVisitDuration();

		bool SubscribeToQueue(BROADCASTQUEUE_NAME queueName, bool ignorePreviousPackets = false);
		void UnSubscribeFromQueue(BROADCASTQUEUE_NAME queueName);
		void UnSubscribeFromAll();

		void IncrementUsage();
		void DecrementUsage();
		LogicalConnectionImpl& getImpl();
		virtual void handleRequest(IncomingPacket* pRequest);
	protected:
		virtual bool IsInactive();
		virtual void OnReadyForSend(unsigned int nAvailableSpace);
		virtual void Recycle() = 0;	
		virtual Login::Type processLogin( LoginData& loginData ) = 0;
		virtual void OnDisconnected(DisconnectionReason::Type closeReason);
		virtual void OnConnected();
		virtual bool OnBeforeIdleClose();
	private:
		LogicalConnectionImpl* pImpl;
	};

}

#endif // LogicalConnection__INCLUDED
