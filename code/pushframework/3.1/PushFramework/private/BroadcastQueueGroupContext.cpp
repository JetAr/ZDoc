#include "StdAfx.h"
#include "BroadcastQueueGroupContext.h"
#include "BroadcastQueueContext.h"
#include "BroadcastQueue.h"

namespace PushFramework
{

	BroadcastQueueGroupContext::BroadcastQueueGroupContext( BroadcastQueueContext* pQueueContext )
	{
		pRootItem = pQueueContext;
		pCurrent = pQueueContext;
		pNextGroup = NULL;
	}

	BroadcastQueueGroupContext::~BroadcastQueueGroupContext()
	{
		if (pNextGroup)
			delete pNextGroup;

		if (pRootItem)
			delete pRootItem;
	}

	unsigned int BroadcastQueueGroupContext::getPriority()
	{
		return pRootItem->getQueueOptions().priority;
	}

	bool BroadcastQueueGroupContext::insert( BroadcastQueueContext* pInfo )
	{
		if(getPriority() < pInfo->getQueueOptions().priority)
			return false;

		if (pInfo->getQueueOptions().priority < getPriority())
		{
			if(!pNextGroup)
			{
				pNextGroup = new BroadcastQueueGroupContext(pInfo);
			}
			else
			{
				if (!pNextGroup->insert(pInfo))
				{
					BroadcastQueueGroupContext* newGroupContext = new BroadcastQueueGroupContext(pInfo);
					newGroupContext->pNextGroup = pNextGroup;
					pNextGroup = newGroupContext;
				}
			}
			return true;
		}

		//Same priority.
		if (pInfo->getQueueOptions().packetsQuota > pRootItem->getQueueOptions().packetsQuota)
		{
			pInfo->pNext = pRootItem;
			pRootItem = pInfo;
		}
		else
		{
			BroadcastQueueContext* pImmediateParent = pRootItem;
			while (pImmediateParent->pNext && pImmediateParent->pNext->getQueueOptions().packetsQuota > pInfo->getQueueOptions().packetsQuota)
			{
				pImmediateParent = pImmediateParent->pNext;
			}
			pInfo->pNext = pImmediateParent->pNext;
			pImmediateParent->pNext = pInfo;
		}
		return true;
	}

	OutgoingPacket* BroadcastQueueGroupContext::getNextPacket( PacketInfo*& pPacketInfo )
	{
		BroadcastQueueContext* pContext = pCurrent;

		while (true)
		{
			OutgoingPacket* pPacket = pContext->getNextPacket(pPacketInfo);
			if (pPacket)
			{
				pCurrent = pContext;
				return pPacket;
			}
			//
			pContext = pContext->pNext ? pContext->pNext : pRootItem;

			if (pContext == pCurrent)
			{
				return NULL;
			}
		}
	}

	void BroadcastQueueGroupContext::returnPacket( PacketInfo* pPacketInfo, bool isSent /*= true*/ )
	{
		if (isSent)
		{
			pCurrent->nLastSentPacketId = pPacketInfo->packetId;
			pCurrent->nPacketsSent ++;
		}
		
		pCurrent->pQueue->disposePacket(pPacketInfo);
		
		if (pCurrent->nPacketsSent >= pCurrent->getQueueOptions().packetsQuota)
		{
			pCurrent->nPacketsSent = 0;
			pCurrent = pCurrent->pNext ? pCurrent->pNext : pRootItem;
		}
	}

	void BroadcastQueueGroupContext::remove( BROADCASTQUEUE_NAME queueName )
	{
		BroadcastQueueContext* pSearch = pRootItem;
		BroadcastQueueContext* pPrevious = NULL;
		while (pSearch != NULL)
		{
			if (pSearch->pQueue->getChannelName() == queueName)
			{
				break;
			}

			pPrevious = pSearch;
			pSearch = pSearch->pNext;
		}


		if (pSearch)
		{
			if (pPrevious)
			{
				pPrevious->pNext = pSearch->pNext;
			}
			else
			{
				pRootItem = pSearch->pNext;
			}

			pSearch->pNext = NULL;

			if (pCurrent == pSearch)
			{
				pCurrent = NULL;
			}

			delete pSearch;
		}
		else
		{
			if (pNextGroup)
			{
				pNextGroup->remove(queueName);

				if (pNextGroup->pRootItem == NULL)
				{
					//Next group has become empty. remove it.
					BroadcastQueueGroupContext* pTemp = pNextGroup;

					this->pNextGroup = pTemp->pNextGroup;
					pTemp->pNextGroup = NULL;

					delete pTemp;
				}
			}			
		}
	}

}
