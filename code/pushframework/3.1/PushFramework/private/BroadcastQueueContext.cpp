#include "StdAfx.h"
#include "BroadcastQueueContext.h"
#include "BroadcastQueue.h"

namespace PushFramework
{

	BroadcastQueueContext::BroadcastQueueContext( BroadcastQueue* pQueue, unsigned int nLastSentPacketId )
	{
		this->nLastSentPacketId = nLastSentPacketId;
		this->nPacketsSent = 0;
		this->pNext = NULL;
		this->pQueue = pQueue;
	}


	BroadcastQueueContext::~BroadcastQueueContext(void)
	{
		if (pNext)
			delete pNext;
	}

	bool BroadcastQueueContext::hasHigherPriority( BroadcastQueueContext* pInfo )
	{
		if (pQueue->getOptions().priority > pInfo->pQueue->getOptions().priority)
		{
			return true;
		}

		if (pQueue->getOptions().priority == pInfo->pQueue->getOptions().priority)
		{
			return pQueue->getOptions().packetsQuota > pInfo->pQueue->getOptions().packetsQuota;
		}

		return false;
	}

	OutgoingPacket* BroadcastQueueContext::getNextPacket( PacketInfo*& pPacketInfo )
	{
		return pQueue->getNextPacket(nLastSentPacketId, pPacketInfo);
	}

	void BroadcastQueueContext::returnPacket(PacketInfo* pPacketInfo)
	{
		nLastSentPacketId = pPacketInfo->packetId;
		nPacketsSent++;
		pQueue->disposePacket(pPacketInfo);
	}

	QueueOptions& BroadcastQueueContext::getQueueOptions()
	{
		return pQueue->getOptions();
	}

}
