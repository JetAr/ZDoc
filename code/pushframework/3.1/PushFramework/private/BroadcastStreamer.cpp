/********************************************************************
	File :			BroadcastStreamer.cpp
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
#include "StdAfx.h"
#include "BroadcastStreamer.h"

#include "LogicalConnectionImpl.h"
#include "PacketInfo.h"
#include "BroadcastManager.h"
#include "PhysicalConnection.h"
#include "GarbageCollector.h"
#include "ScopedLock.h"

namespace PushFramework
{

BroadcastStreamer::BroadcastStreamer(void)
{
	shouldStop = false;
	itemsCount = 0;
	eventBits = 0;
	InitializeCriticalSection(&cs_SharedLists);
}

BroadcastStreamer::~BroadcastStreamer(void)
{
	DeleteCriticalSection(&cs_SharedLists);
}

void BroadcastStreamer::addItem( ItemPtr pItem )
{
	pItem->IncrementUsage();
	ScopedLock lock(cs_SharedLists);

	newItems.insert(pItem);
	eventBits |= EventsBits::ADD_ITEM;
}


void BroadcastStreamer::removeItem( ItemPtr pItem )
{
	ScopedLock lock(cs_SharedLists);

	removedItems.insert(pItem);
	eventBits |= EventsBits::REMOVE_ITEM;
}


bool BroadcastStreamer::processItem( ItemPtr pLogicalConnection )
{
	return pLogicalConnection->CheckAndProcessPendingBroadcast(true);
}

void BroadcastStreamer::awakeAll()
{
	eventBits |= EventsBits::AWAKE_ALL;
}

void BroadcastStreamer::doWork()
{
	while (!shouldStop)
	{
		if (!eventBits)
		{
			Sleep(10);
			continue;
		}

		bool requireLock = (eventBits & (EventsBits::ADD_ITEM|EventsBits::REMOVE_ITEM)) != 0;
		if (requireLock)
		{
			EnterCriticalSection(&cs_SharedLists);
		}

		if (eventBits & EventsBits::REMOVE_ITEM)
		{
			eventBits &= (~EventsBits::REMOVE_ITEM);
			clearRemovedItems();
		}

		if (eventBits & EventsBits::ADD_ITEM)
		{
			eventBits &= (~EventsBits::ADD_ITEM);
			checkList.insert(newItems.begin(), newItems.end());
			newItems.clear();
		}


		if (requireLock)
		{
			LeaveCriticalSection(&cs_SharedLists);
		}

		if (eventBits & EventsBits::AWAKE_ALL)
		{
			eventBits &= (~EventsBits::AWAKE_ALL);
			processAwakedItems(checkList);
			/*checkList.clear();*/
		}
	}
}

int BroadcastStreamer::getItemsCount()
{
	return itemsCount;
}

void BroadcastStreamer::start()
{
#ifdef Plateform_Windows
	DWORD  nThreadID;

	hThread = CreateThread(NULL,// Security
		0,						// Stack size - use default
		threadProc,     		// Thread fn entry point
		(void*) this,			// Param for thread
		0,						// Init flag
		&nThreadID);			// Thread address
#else
#ifdef Plateform_Linux
	pthread_create(&hThread, NULL, threadProc, (void*) this);
#endif
#endif
}

void BroadcastStreamer::stop()
{
	shouldStop = true;
#ifdef Plateform_Windows
	WaitForSingleObject(hThread, INFINITE);
#else
#ifdef Plateform_Linux
	pthread_join(hThread, NULL);
#endif
#endif
}

#ifdef Plateform_Windows
DWORD WINAPI BroadcastStreamer::threadProc(LPVOID lpVoid)
#else
#ifdef Plateform_Linux
void* BroadcastStreamer::threadProc(void* lpVoid)
#endif
#endif
{
	BroadcastStreamer* pMe = reinterpret_cast<BroadcastStreamer*> (lpVoid);
	pMe->doWork();
#ifdef Plateform_Windows
	return 0;
#endif
}

void BroadcastStreamer::clearRemovedItems()
{
	itemSetT::iterator it, it2;
	it = removedItems.begin();
	while (it != removedItems.end())
	{
		ItemPtr removedItem = *it;
		bool found = false;

		it2 = newItems.find(removedItem);
		if(it2 != newItems.end()){
			newItems.erase(it2);
			found = true;
		}
		it2 = checkList.find(removedItem);
		if(it2 != checkList.end()){
			checkList.erase(it2);
			found = true;
		}

		if (found)
		{
			removedItem->DecrementUsage();
		}
		it++;
	}

	removedItems.clear();
}

void BroadcastStreamer::processAwakedItems( itemSetT& activeSet )
{
	itemSetT::iterator it = activeSet.begin();
	while (it != activeSet.end())
	{
		processItem(*it);
		it++;
	}
}

}
