/********************************************************************
	File :			GarbageCollector.cpp
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
#include "GarbageCollector.h"

#include "LogicalConnectionImpl.h"
#include "LogicalConnectionPool.h"
#include "ScopedLock.h"



namespace PushFramework
{


GarbageCollector garbageCollector;

GarbageCollector::GarbageCollector(void)
{
    InitializeCriticalSection(&cs);
}

GarbageCollector::~GarbageCollector(void)
{
    DeleteCriticalSection(&cs);
}

void GarbageCollector::activate(bool killAll/* = false*/)
{
    ScopedLock lock(cs);

    for (waitingListT::iterator it = waitingList.begin();
            it!= waitingList.end();
        )
    {
        LogicalConnectionImpl* pLogicalConnection = *it;
        if (killAll || pLogicalConnection->CanDelete())
        {
			logicalConnectionPool.returnObject(pLogicalConnection);
            it =  waitingList.erase(it);
        }
        else
            it++;
    }
}

void GarbageCollector::addDisposableClient( LogicalConnectionImpl* pLogicalConnection )
{
	//See if we can immediately remove this : 
	if (pLogicalConnection->CanDelete())
	{
		logicalConnectionPool.returnObject(pLogicalConnection);
		return;
	}
	//Else defer that to a later time when no one is referencing it.
    ScopedLock lock(cs);
    waitingList.push_back(pLogicalConnection);
}

}

