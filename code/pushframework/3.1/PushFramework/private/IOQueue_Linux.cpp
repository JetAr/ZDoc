/********************************************************************
	File :			IOQueue_Linux.cpp
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
#include "IOQueue_Linux.h"


namespace PushFramework
{

IOQueue ioQueue;

IOQueue::IOQueue()
{
    inPollQueue = -1;
    outPollQueue = -1;
}

IOQueue::~IOQueue(void)
{
}

bool IOQueue::create()
{
	// Using two epoll for a very good reason.

    int pollsize = 1024;
    inPollQueue = epoll_create(pollsize);

    if (inPollQueue < 0)
    {
        return false;
    }

    outPollQueue = epoll_create(pollsize);

    if (outPollQueue < 0)
    {
        close(inPollQueue);
        return false;
    }

    return true;
}

void IOQueue::free()
{
    close(inPollQueue);
    close(outPollQueue);
}

int IOQueue::getQueuedEvent(epoll_event *pEvents, int nMaxEvents, bool isInputEvents)
{
    int pollQueue = isInputEvents? inPollQueue : outPollQueue;

    while (true)
    {
        int rval = epoll_wait(pollQueue, pEvents, nMaxEvents, 1000);

        if (rval == 0)
        {
            // Timeout.
            return rval;
        }

        if (rval < 0)
        {
            if (errno == EAGAIN || errno == EINTR)
            {
                continue;
            }
            //perror("Error on epoll_wait");
            return rval;
        }

        return rval;
    }
}

bool IOQueue::addSocketContext(SOCKET clientSocket, void* lpObject)
{
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLONESHOT;
    ev.data.ptr = lpObject;

    if (epoll_ctl(inPollQueue, EPOLL_CTL_ADD, clientSocket, &ev) < 0)
    {

        return false;
    }

    ev.events = EPOLLONESHOT;
    ev.data.ptr = lpObject;

    if (epoll_ctl(outPollQueue, EPOLL_CTL_ADD, clientSocket, &ev) < 0)
    {
        epoll_ctl(inPollQueue,EPOLL_CTL_DEL,clientSocket, NULL);
        return false;
    }


    return true;
}

// close will do this job. But function is kept for the purpose of
// implementing EPOLL_CTL_DISABLE when it comes to the official Linux kernel.
void IOQueue::deleteSocketContext(SOCKET socket)
{
    epoll_ctl(inPollQueue,EPOLL_CTL_DEL,socket, NULL);
    epoll_ctl(outPollQueue,EPOLL_CTL_DEL,socket, NULL);
}

bool IOQueue::rearmSocketForRead(SOCKET clientSocket, void* lpObject)
{
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLONESHOT;
    ev.data.ptr = lpObject;

    if (epoll_ctl(inPollQueue, EPOLL_CTL_MOD, clientSocket, &ev) < 0)
    {
        return false;
    }
    return true;
}

bool IOQueue::rearmSocketforWrite(SOCKET clientSocket, void* lpObject)
{
    epoll_event ev;
    ev.events = EPOLLOUT| EPOLLONESHOT;
    ev.data.ptr = lpObject;

    if (epoll_ctl(outPollQueue, EPOLL_CTL_MOD, clientSocket, &ev) < 0)
    {
        return false;
    }
    return true;
}


}

