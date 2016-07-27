/********************************************************************
File :			PhysicalConnection_Win.cpp
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
#include "PhysicalConnection.h"

#include "ScopedLock.h"
#include "..\include\ListenerOptions.h"
#include "ServerStats.h"
#include "../include/ServerOptions.h"

namespace PushFramework
{

void PhysicalConnection::postReceive()
{
    if (status < Connected)
    {
        return;
    }

    ULONG	ulFlags = MSG_PARTIAL;
    DWORD dwIoSize;

    UINT nRetVal = WSARecv(socket, &readIoBuffer.m_wsaBuffer, 1, &dwIoSize, &ulFlags, &readIoBuffer.pOverlap->m_ol, NULL);

    if ( nRetVal == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
    {
        CloseSocket();
        return;
    }
    incrementIoWorkersReferenceCounter();
}


bool PhysicalConnection::readReceivedBytes( RecyclableBuffer& incomingBytes, int dwIoSize )
{
    if(dwIoSize==0)
    {
        return false;
    }

	// Report these received bytes to stats.
	if (!isObserverChannel())
	{
		stats.addToCumul(ServerStats::BandwidthInbound, dwIoSize);
		stats.addToKeyedDuration(ServerStats::BandwidthInboundPerConnection, getSocket(), dwIoSize);
	}

	dtLastReceiveTime = time(NULL);

	incomingBytes.assign(readIoBuffer, dwIoSize);

	return true;
}

bool PhysicalConnection::WriteBytes()
{
    DWORD dwSent = 0;
    DWORD dwToPick=min(options.nSocketBufferSize, oBuffer.getDataSize());

	CopyMemory(writeIoBuffer.getBuffer(), oBuffer.getBuffer(), dwToPick);
	writeIoBuffer.m_wsaBuffer.len = dwToPick;

    ULONG	ulFlags = MSG_PARTIAL;

    UINT nRetVal = WSASend(socket, &writeIoBuffer.m_wsaBuffer, 1, &dwSent, ulFlags, &writeIoBuffer.pOverlap->m_ol, NULL);
    if ( nRetVal == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
    {
		int lastError = WSAGetLastError();
        return false;
    }

    bWriteInProgress = TRUE;
    incrementIoWorkersReferenceCounter();
    return true;
}


bool PhysicalConnection::OnSendCompleted( int dwIoSize, bool& bIsBufferIdle )
{
    ScopedLock lock(csLock);

	bIsBufferIdle = false;

	if(status < WaitingForWrite)
	{
		return false;
	}

	if (!isObserverChannel())
	{
		stats.addToCumul(ServerStats::BandwidthOutbound, dwIoSize);
		stats.addToKeyedDuration(ServerStats::BandwidthOutboundPerConnection, getSocket(), dwIoSize);
	}

    oBuffer.pop(dwIoSize);

    bIsBufferIdle = oBuffer.getDataSize()==0;
    if (bIsBufferIdle)
    {
        if (status==WaitingForWrite )
        {
			CloseSocket(); 
            return false; // Connection will be closed.
        }
        else
        {
            bWriteInProgress = FALSE;
			return true;
        }
    }

	return WriteBytes();
}

void PhysicalConnection::closeConnection( bool bWaitForSendsToComplete )
{
    ScopedLock lock(csLock);


    //Either connected or attached :
    if (bWaitForSendsToComplete)
    {
        if (oBuffer.getDataSize() == 0)
        {
            CloseSocket();
        }
        else
            status = WaitingForWrite;
    }
    else
    {
        CloseSocket();
    }
}

void PhysicalConnection::CloseSocket()
{
    closesocket( socket );
    status = Disposable;
}

bool PhysicalConnection::checkIfUnusedByIOWorkers()
{
	return status == Disposable && ioWorkersReferenceCounter == 0;
}


}
