/********************************************************************
	File :			IOQueue.cpp
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
#include "StdAfx.h"
#include "IOQueue_Win.h"

namespace PushFramework
{

IOQueue ioQueue;

IOQueue::IOQueue(void)
{
    m_hCompletionPort = NULL;
    pCustomKey = NULL;
}

IOQueue::~IOQueue(void)
{
}
bool IOQueue::create()
{
    SOCKET s; //Used to create the iocp:
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if ( s == INVALID_SOCKET )
    {
        return false;
    }
    //
    m_hCompletionPort = CreateIoCompletionPort( (HANDLE)s, NULL, 0, 0 );
    if ( m_hCompletionPort == NULL )
    {
        closesocket( s );
        return false;
    }
    closesocket( s );//In either cases.

    SOCKET lSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if ( lSocket == INVALID_SOCKET )
    {
        return false;
    }
    pCustomKey=new _CUSTOM_KEY();
    pCustomKey->socket=lSocket;
    
	// Now Associate with IOCP main Handle
    if (!addSocketContext(lSocket,(DWORD) pCustomKey))
        return false;
    return true;
}

void IOQueue::free()
{
    closesocket(pCustomKey->socket);

    // Close the CompletionPort and stop any more requests
    CloseHandle(m_hCompletionPort);

    delete pCustomKey;
}

void IOQueue::postTerminationSignal()
{
    PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD) NULL, NULL);
}

BOOL IOQueue::getQueuedEvent( LPDWORD lpBytesLen, ULONG_PTR* lpContext, LPOVERLAPPED* lpOverlapped )
{
    return GetQueuedCompletionStatus(m_hCompletionPort, lpBytesLen, lpContext, lpOverlapped, INFINITE);
}

BOOL IOQueue::initializeSocketContext( DWORD lpObject )
{
    OVERLAPPEDPLUS	*pOverlap = new OVERLAPPEDPLUS(IOInitialize);
    return PostQueuedCompletionStatus(m_hCompletionPort, 0, lpObject, &pOverlap->m_ol);
}

void IOQueue::postCustomEvent( IOType enumID )
{
    OVERLAPPEDPLUS	*pOverlap = new OVERLAPPEDPLUS(enumID);
    PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD) pCustomKey, &pOverlap->m_ol);
}

bool IOQueue::addSocketContext( SOCKET socket,DWORD lpObject )
{
    HANDLE h = CreateIoCompletionPort((HANDLE)socket, m_hCompletionPort, lpObject, 0);
    return h==m_hCompletionPort;
}
}

