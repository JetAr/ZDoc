/********************************************************************
	File :			DemuxImpl_Linux.cpp
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
#include "DemuxImpl_Linux.h"

#include "Demultiplexor.h"
#include "ServerImpl.h"

#ifdef Plateform_Windows
#include "IOQueue_Win.h"
#else
#ifdef Plateform_Linux
#include "IOQueue_Linux.h"
#endif
#endif

#include "Dispatcher.h"
#include "PhysicalConnection.h"
#include "../include/Server.h"
#include "../include/ServerOptions.h"


namespace PushFramework
{
DemuxImpl::DemuxImpl(Demultiplexor* pFacade)
{
    this->pFacade = pFacade;
    stopWorking = false;
}

DemuxImpl::~DemuxImpl()
{
}

bool DemuxImpl::start()
{
    stopWorking = false;

    int nThreads = options.nWorkersCount;
    for (int i = 0; i < nThreads; i++)
    {
        pthread_t thread;
        pthread_create(&thread, NULL, threadProcForReadEvents, (void*) this);
        workersThreadsVect.push_back(thread);
    }
    for (int i = 0; i < nThreads; i++)
    {
        pthread_t thread;
        pthread_create(&thread, NULL, threadProcForWriteEvents, (void*) this);
        workersThreadsVect.push_back(thread);
    }
    return true;

}

void DemuxImpl::stop()
{
    stopWorking = true;
    for (int i = 0; i < workersThreadsVect.size(); i++)
    {
        pthread_t hThread = workersThreadsVect.at(i);
        pthread_join(hThread, NULL);
    }
}

void* DemuxImpl::threadProcForReadEvents(void* WorkContext)
{
    DemuxImpl* pThis = reinterpret_cast<DemuxImpl*> (WorkContext);

    pThis->proc();

    return 0;
}
void* DemuxImpl::threadProcForWriteEvents(void* WorkContext)
{
    DemuxImpl* pThis = reinterpret_cast<DemuxImpl*> (WorkContext);

    pThis->proc(false);

    return 0;
}

void DemuxImpl::proc(bool processReadEvents)
{
    int epollArraySize = 64;
    struct epoll_event epoll_events[64];

    while (!stopWorking)
    {
        int rval = ioQueue.getQueuedEvent(epoll_events, epollArraySize, processReadEvents);

        if (rval == 0)
        {
            continue;// Timeout. A chance to stop demux threads.
        }

        if (rval < 0)
        {
			// TODO. Investigate on this possible scenario.
            return;
        }

        //Start handling events :
        for (int i = 0; i < rval; i++)
        {
            //
            uint32_t events = epoll_events[i].events;
            int fd = epoll_events[i].data.fd;
            PhysicalConnection* pPerSocketContext = reinterpret_cast<PhysicalConnection*> (epoll_events[i].data.ptr);

            if (events & EPOLLERR || events & EPOLLHUP || events & EPOLLRDHUP)
            {
				// TODO. Investigate on these scenario. Not sure if handling code is correct.
                shutdown(fd, SHUT_RDWR);
                close(fd);
                continue;
            }

            //
            if (events & EPOLLOUT)
            {
                dispatcher.handleWrite(*pPerSocketContext, -1);
            }
            if (events & EPOLLIN)
            {
                dispatcher.handleRead(*pPerSocketContext, -1);
            }
        }
    }
}

}

