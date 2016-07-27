/********************************************************************
	File :			IOQueue.h
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
#ifndef IOQueue__INCLUDED
#define IOQueue__INCLUDED

#pragma once

#include "Overlap.h"

namespace PushFramework
{


class IOQueue
{
public:
    IOQueue(void);
    ~IOQueue(void);
    bool create();
    void free();
    void postTerminationSignal();
    BOOL getQueuedEvent( LPDWORD lpBytesLen, ULONG_PTR* lpContext,LPOVERLAPPED* lpOverlapped );
    BOOL initializeSocketContext(DWORD lpObject);
    void postCustomEvent(IOType enumID);
    bool addSocketContext(SOCKET socket,DWORD lpObject);
private:
    HANDLE					m_hCompletionPort;
    class _CUSTOM_KEY
    {
    public:
        SOCKET socket;
    }* pCustomKey;
};

extern IOQueue ioQueue;
}

#endif // IOQueue__INCLUDED
