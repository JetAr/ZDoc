/********************************************************************
	File :			DemuxImpl_Win.cpp
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
#include "DemuxImpl_Win.h"

#include "Demultiplexor.h"
#include "ServerImpl.h"

#ifdef Plateform_Windows
#include "IOQueue_Win.h"
#elif Plateform_Linux
#include "IOQueue_Linux.h"
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
	g_hShutdownEvent	= NULL;
}

DemuxImpl::~DemuxImpl()
{
}

bool DemuxImpl::start()
{
    g_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    DWORD  nThreadID;

    for ( unsigned int i = 0; i < options.nWorkersCount; i++ )
    {

        HANDLE hThread = CreateThread(NULL,				// Security
                               0,						// Stack size - use default
                               KerIOCPWorkerProc,     	// Thread fn entry point
                               (void*) this,			// Param for thread
                               0,						// Init flag
                               &nThreadID);				// Thread address
		if (hThread)
		{
			workersThreadsVect.push_back(hThread);
		}
		else
		{
			stop();
			return false;
		}
    }
    return true;
}

void DemuxImpl::stop()
{
	if(workersThreadsVect.empty())
		return;
	
	SetEvent(g_hShutdownEvent);

	HANDLE *pThreads = new HANDLE[workersThreadsVect.size()];
	for (size_t i = 0; i < workersThreadsVect.size(); i++)
	{
		pThreads[i] = workersThreadsVect.at(i);
		// Ensure all workers will awake from GetQueuedCompletionStatus
		ioQueue.postTerminationSignal();
	}

    WaitForMultipleObjects(workersThreadsVect.size(), pThreads, TRUE, INFINITE);
	delete [] pThreads;
	
	CloseHandle(g_hShutdownEvent);
}
DWORD WINAPI DemuxImpl::KerIOCPWorkerProc( LPVOID WorkContext )
{
    DemuxImpl* pThis = reinterpret_cast<DemuxImpl*>(WorkContext);

    pThis->proc();

    return 0;
}

void DemuxImpl::proc()
{
    DWORD dwIoSize;
    PhysicalConnection *pConnection = NULL;
    LPOVERLAPPED lpOverlapped;//Passed to GetCompletionStatus to point to the completed operation's buffer
    OVERLAPPEDPLUS  *pPerIOContext;//OverlapPlus associated to the lpOverlapped returned by GetCompletionStatus(..


    while (WAIT_OBJECT_0 != WaitForSingleObject(g_hShutdownEvent, 0))
    {
        BOOL bSuccess = ioQueue.getQueuedEvent(&dwIoSize,(ULONG_PTR*) &pConnection,&lpOverlapped);

        if(!bSuccess)
        {
			if (pConnection)
			{
				// when the socket is closed, all pending IOs on the completion port are
				// cancelled. We should decrement the physical connection reference counter
				// to allow for the safe disposal of both the logical connection and its physical
				// connection structure.
				dispatcher.handleFailedIO(*pConnection);
			}
            continue;
        }
        if (pConnection==NULL)
        {
            // Signal to stop.
            break;
        }

        pPerIOContext = CONTAINING_RECORD(lpOverlapped, OVERLAPPEDPLUS, m_ol);
        switch (pPerIOContext->m_ioType)
        {
        case IORead:
            dispatcher.handleRead(*pConnection,dwIoSize);
            break;
        case IOWrite:
            dispatcher.handleWrite(*pConnection,dwIoSize);
            break;
        case IOInitialize:
            dispatcher.handleInitialize(*pConnection);
            delete pPerIOContext;
            break;
        default:
            break;
        }//end of switch

    }//end of while
}

}