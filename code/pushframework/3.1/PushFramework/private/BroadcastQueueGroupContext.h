#pragma once

namespace PushFramework
{
	struct BroadcastQueueContext;
	struct PacketInfo;
	struct BroadcastQueueGroupContext 
	{
		BroadcastQueueGroupContext(BroadcastQueueContext* pQueueContext);
		~BroadcastQueueGroupContext();
		BroadcastQueueContext* pRootItem;
		BroadcastQueueContext* pCurrent;
		BroadcastQueueGroupContext* pNextGroup;

		unsigned int getPriority();
		bool insert(BroadcastQueueContext* pInfo);
		void remove(BROADCASTQUEUE_NAME queueName);

		OutgoingPacket* getNextPacket(PacketInfo*& pPacketInfo);
		void returnPacket(PacketInfo* pPacketInfo, bool isSent = true);
	};

}
