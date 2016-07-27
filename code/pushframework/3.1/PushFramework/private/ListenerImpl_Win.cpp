/********************************************************************
	File :			ListenerImpl_Win.cpp
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
#include "ListenerImpl_Win.h"

#include "Listener.h"


namespace PushFramework
{

ListenerImpl::ListenerImpl(Listener* pFacade)
{
    this->pFacade = pFacade;
    hSocket = INVALID_SOCKET;
    hThread = INVALID_HANDLE_VALUE;
    hKillEvent = INVALID_HANDLE_VALUE;
}

ListenerImpl::~ListenerImpl()
{
}

bool ListenerImpl::startListening()
{
    //Create the socket :
    hSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (hSocket == INVALID_SOCKET)
    {
        return false;
    }

    //Prepare address to bind to :
    SOCKADDR_IN		saServer;
    saServer.sin_port = htons(pFacade->uPort);
    saServer.sin_family = AF_INET;
    if(pFacade->listenerOptions.interfaceAddress != NULL)
    {
        u_long nInterfaceAddr = inet_addr(pFacade->listenerOptions.interfaceAddress);
        if (nInterfaceAddr != INADDR_NONE)
        {
            closesocket(hSocket);
            return false;
        }
        saServer.sin_addr.s_addr = nInterfaceAddr;
    }
    else
        saServer.sin_addr.s_addr = INADDR_ANY;


    //Bind the socket :
    int nRet = bind(hSocket, (LPSOCKADDR)&saServer, sizeof(struct sockaddr));
    if (nRet == SOCKET_ERROR)
    {
        closesocket(hSocket);
        return false;
    }


    //Create socket IO event
    hSocketEvent = WSACreateEvent();
    if (hSocketEvent == WSA_INVALID_EVENT)
    {
        closesocket(hSocket);
        return false;
    }


    //I am interested in accept event only :
    nRet = WSAEventSelect(hSocket, hSocketEvent, FD_ACCEPT);
    if (nRet == SOCKET_ERROR)
    {
        WSACloseEvent(hSocketEvent);
        closesocket(hSocket);
        return false;
    }


    //Set socket to listening mode :
    nRet = ::listen(hSocket, pFacade->listenerOptions.listeningBackLog);
    if (nRet == SOCKET_ERROR)
    {
        WSACloseEvent(hSocketEvent);
        closesocket(hSocket);
        return false;
    }


    //Create kill event used by ::stop to terminate the listening thread
    hKillEvent	= CreateEvent(NULL, TRUE, FALSE, NULL);


    DWORD	dwThreadId = 0;
    hThread = CreateThread(NULL, // Security
                                     0,					// default stack size
                                     threadProc,  // thread proc
                                     (void*) this,
                                     0,					// init flag
                                     &dwThreadId);	//


    if (hThread == INVALID_HANDLE_VALUE)
    {
        WSACloseEvent(hSocketEvent);
        closesocket(hSocket);
        CloseHandle(hKillEvent);
        return false;
    }
    return true;
}

void ListenerImpl::stopListening()
{
    ::SetEvent(hKillEvent);

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    CloseHandle(hKillEvent);
    closesocket(hSocket);
    WSACloseEvent(hSocketEvent);
}

DWORD WINAPI ListenerImpl::threadProc( LPVOID lpVoid )
{
    ListenerImpl* pThis = reinterpret_cast<ListenerImpl*>(lpVoid);
    pThis->doListen();
    return 0;
}

bool ListenerImpl::acceptConnectionRequest()
{
    // accept socket :
    SOCKADDR_IN	SockAddr;
    int nLen = sizeof(SOCKADDR_IN);
    SOCKET	clientSocket = accept(hSocket, (LPSOCKADDR)&SockAddr, &nLen);
    if (clientSocket == INVALID_SOCKET)
        return false;

    //TCP no delay option :
    const char optValue = pFacade->listenerOptions.isTcpNoDelay ? 1 : 0;
    int nRet = setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, &optValue, sizeof(char));
    if (nRet == -1)
    {
        shutdown(clientSocket, 2);
        closesocket(clientSocket);
        return true;
    }

    //Start servicing new connection
    if (!pFacade->handleAcceptedSocket(clientSocket, SockAddr))
    {
        shutdown(clientSocket, 2);
        closesocket(clientSocket);
        return true;
    }
    return true;
}

void ListenerImpl::doListen()
{
    HANDLE			hWaits[2];
    hWaits[0]		= hKillEvent;
    hWaits[1]		= hSocketEvent;


    WSANETWORKEVENTS events;
    while(true)
    {
        // Wait for either the kill event or  an accept event :
        DWORD dwRet = WSAWaitForMultipleEvents(2, hWaits, FALSE, INFINITE, FALSE);

        //If kill event then quit loop :
        if (dwRet == WAIT_OBJECT_0)
        {
            break;
        }

        int nRet = WSAEnumNetworkEvents(hSocket, hSocketEvent, &events);
        if (nRet == SOCKET_ERROR)
        {
            //Error
            break;
        }

        //Deal with the new incoming connection
        if ((events.lNetworkEvents & FD_ACCEPT) && (events.iErrorCode[FD_ACCEPT_BIT] == 0))
        {
            if(!acceptConnectionRequest())
                break;
        }
        else
            break;
    }
}


}