/********************************************************************
	File :			ListenerImpl_Linux.cpp
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
#include "ListenerImpl_Linux.h"

#include "Listener.h"

namespace PushFramework
{
ListenerImpl::ListenerImpl(Listener* pFacade)
{
    this->pFacade = pFacade;
    stopListen = false;
    hSocket = -1;
}

ListenerImpl::~ListenerImpl()
{
}

bool ListenerImpl::startListening()
{
    //Create the socket :
    hSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (hSocket < 0)
    {
        return false;
    }

    int on = 1;
    if (setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on)))
    {
        close(hSocket);
        return false;
    }

    //Prepare address to bind to :
    SOCKADDR_IN saServer;
    saServer.sin_port = htons(pFacade->uPort);
    saServer.sin_family = AF_INET;
    if (pFacade->listenerOptions.interfaceAddress != NULL)
    {
        u_long nInterfaceAddr = inet_addr(pFacade->listenerOptions.interfaceAddress);
        if (nInterfaceAddr != INADDR_NONE)
        {
            close(hSocket);
            return false;
        }
        saServer.sin_addr.s_addr = nInterfaceAddr;
    }
    else
        saServer.sin_addr.s_addr = INADDR_ANY;


    //Bind the socket :
    int nRet = bind(hSocket, (struct sockaddr *) &saServer, sizeof (struct sockaddr));
    if (nRet < 0)
    {
        close(hSocket);
        return false;
    }


    //Set socket to listening mode :
    if (listen(hSocket, pFacade->listenerOptions.listeningBackLog))
    {
        close(hSocket);
        return false;
    }

    //Create kill event used by ::stop to terminate the listening thread
    stopListen = false;

    pthread_create(&hThread, NULL, threadProc, (void*) this);

    return true;
}

void ListenerImpl::stopListening()
{
    stopListen = true;
    shutdown(hSocket, SHUT_RDWR);
    pthread_join(hThread, NULL);
}

void* ListenerImpl::threadProc(void* lpVoid)
{
    ListenerImpl* pThis = reinterpret_cast<ListenerImpl*> (lpVoid);
    pThis->doListen();
    return 0;
}

bool ListenerImpl::acceptConnectionRequest(SOCKET clientSocket, SOCKADDR_IN SockAddr)
{
    // Set as non-blocking :
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK))
    {
        shutdown(clientSocket, 2);
        close(clientSocket);
        return true;
    }

    //Start servicing new connection
    if (!pFacade->handleAcceptedSocket(clientSocket, SockAddr))
    {
        shutdown(clientSocket, 2);
        close(clientSocket);
        return true;
    }
    return true;
}

void ListenerImpl::doListen()
{
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof (cli_addr);

    while (!stopListen)
    {
        int clientSocket = accept(hSocket,
                                  (struct sockaddr *) &cli_addr, &clilen);
        if (clientSocket < 0)
        {
            if (errno == EAGAIN)
            {
                continue;
            }
			// Should not come here. 
			// TODO. Investigate on this scenario. Consider restarting listening thread.
            break;
        }
        acceptConnectionRequest(clientSocket, cli_addr);
    }
    close(hSocket);
}

}


