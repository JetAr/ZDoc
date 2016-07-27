/********************************************************************
        File :			Server.h
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
#ifndef Server__INCLUDED
#define Server__INCLUDED

#pragma once

#include "PushFramework.h"
#include "Common.h"

namespace PushFramework
{

	struct ListenerOptions;
	struct QueueOptions;
	class ServerImpl;
	class Service;
	class LogicalConnection;

	class PUSHFRAMEWORK_API Server
	{
		friend class ServerImpl;
	public:
		Server();
		virtual ~Server(void);

		bool createListener(int port, ListenerOptions* pOptions);

		void addBlockedIPRange(const char* ipStart, const char* ipStop);
		void addPermitIPRange(const char* ipStart, const char* ipStop);

		void registerService(Service* pService);
		void setServerInfos(const char* serverName);
		virtual bool handleMonitorRequest(const char* command, char* lpOut);

		bool start(bool startInSeparateThread = false);
		void stop();
		void pause();
		void resume();

		void setMessageFactory(MessageFactory* pMsgFactory);

		void CreateQueue(BROADCASTQUEUE_NAME queueName, QueueOptions& queueOptions);
		void RemoveQueue(BROADCASTQUEUE_NAME queueName);
		bool PushPacket(OutgoingPacket* pPacket, BROADCASTQUEUE_NAME queueName);
		bool PushPacket(OutgoingPacket* pPacket, BROADCASTQUEUE_NAME queueName, BROADCASTPACKET_KEY killKey, int objectCategory);
		void RemovePacketFromQueue(BROADCASTPACKET_KEY killKey, int objectCategory, BROADCASTQUEUE_NAME queueName);

	private:
		ServerImpl* pImpl;
		
	public:
		virtual OutgoingPacket* getChallenge(ConnectionContext* pConnectionContext);
		virtual LogicalConnection* createLogicalConnection() = 0;
		virtual void OnStarted();
		virtual ConnectionContext* createConnectionContext();
	};
}

#endif // Server__INCLUDED
