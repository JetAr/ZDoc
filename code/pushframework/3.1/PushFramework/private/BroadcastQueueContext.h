#pragma once

namespace PushFramework
{

	class BroadcastQueue;
	struct QueueOptions;
	struct PacketInfo;

	struct BroadcastQueueContext
	{
		BroadcastQueueContext(BroadcastQueue* pQueue, unsigned int nLastSentPacketId);
		~BroadcastQueueContext(void);

		BroadcastQueue* pQueue;
		unsigned int nPacketsSent;
		unsigned int nLastSentPacketId;

		BroadcastQueueContext* pNext;

		bool hasHigherPriority(BroadcastQueueContext* pInfo);

		OutgoingPacket* getNextPacket(PacketInfo*& pPacketInfo);

		void returnPacket(PacketInfo* pPacketInfo);

		QueueOptions& getQueueOptions();
	};

}