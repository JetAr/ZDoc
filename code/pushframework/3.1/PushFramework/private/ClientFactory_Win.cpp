/********************************************************************
	File :			ChannelFactory_Win.cpp
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
#include "ClientFactory.h"
#include "ServerImpl.h"
#include "Dispatcher.h"
#include "../include/ServerOptions.h"
#include "PhysicalConnectionPool.h"
#include "PhysicalConnection.h"

#ifdef Plateform_Windows
#include "IOQueue_Win.h"
#elif Plateform_Linux
#include "IOQueue_Linux.h"
#endif

namespace PushFramework
{

bool ClientFactory::createPhysicalConnection( SOCKET hSocket, SOCKADDR_IN address, bool isObserver, ListenerOptions* pListenerOptions )
{
	PhysicalConnection* connection = (PhysicalConnection*) thePhysicalConnectionPool.borrowObject();

	// In both cases reset the object.
	connection->reset(hSocket, address, isObserver, pListenerOptions);

	//
	if (!connection->SetUpProtocolContexts())
	{
		thePhysicalConnectionPool.returnObject(connection);
		return false;
	}

    //Now Associate with IOCP main Handle
    if (!ioQueue.addSocketContext(hSocket, (DWORD) connection))
    {
		thePhysicalConnectionPool.returnObject(connection);
        //leave socket close to acceptor
        return false;
    }

	addPhysicalConnection(connection);

    //Unlocking iocp event:
    if(!ioQueue.initializeSocketContext( (DWORD) connection))
    {
        removePhysicalConnection(connection);
        thePhysicalConnectionPool.returnObject(connection);
        //leave socket close to acceptor
        return false;
    }

    return true;
}


}