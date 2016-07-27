/********************************************************************
	File :			LogicalConnectionImpl.h
	Creation date :	3/5/2014

	License :			Copyright 2010-2014 Ahmed Charfeddine, http://www.pushframework.com

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

namespace PushFramework
{
	class ClientFactory;
	class PhysicalConnection;
	struct BroadcastQueueContext;
	struct BroadcastQueueGroupContext;
	class LogicalConnection;
	class BroadcastStreamer;
	class ServerImpl;
	//
	class LogicalConnectionImpl
	{
		friend class ClientFactory;
		friend class ServerImpl;
	public:
		LogicalConnectionImpl(LogicalConnection* facade);
		~LogicalConnectionImpl(void);

		void Recycle();
		void IncrementUsage();
		void DecrementUsage();
		bool CanDelete();
		virtual bool IsMonitor();
		bool CheckAndProcessPendingBroadcast(bool somethingHasHappened);

		void initializeIntern(PhysicalConnection* physicalConnection);
		LogicalConnection* getFacade();
	public:
		SendResult::Type PushPacket( OutgoingPacket* pPacket);
		SendResult::Type TryPushPacket( OutgoingPacket* pPacket );
		void Disconnect(bool waitForPendingPackets);
		double getVisitDuration();
		bool IsInactive();
		bool SubscribeToQueue(BROADCASTQUEUE_NAME queueName, bool ignorePreviousPackets = false);
		void UnSubscribeFromQueue(BROADCASTQUEUE_NAME queueName);
		void UnSubscribeFromAll();
		void OnReadyForSend(unsigned int nAvailableSpace);
		void OnDisconnected(DisconnectionReason::Type closeReason);
		void OnConnected();
		bool OnBeforeIdleClose();
		Login::Type processLogin( LoginData& loginData );
	private:
		LogicalConnection* facade;
		PhysicalConnection* pPhysicalConnection;
		time_t dtConnectTime;
		long nRefrenceCounter;
		BroadcastStreamer* pStreamer;
		BroadcastQueueGroupContext* pRootQueueGroupContext;
		bool skipCheckNextPacket;
		ReadWriteLock subscriptionDataLock;
		long broadcastRunFlag;
	};
}
