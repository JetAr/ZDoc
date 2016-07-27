
#include "StdAfx.h"
#include "ClientFactory.h"
#include "Dispatcher.h"
#include "../include/ServerOptions.h"
#include "PhysicalConnection.h"
#include "PhysicalConnectionPool.h"


#ifdef Plateform_Windows
#include "IOQueue_Win.h"
#else
#ifdef Plateform_Linux
#include "IOQueue_Linux.h"
#endif
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
        
    addPhysicalConnection(connection);

    //Now Associate with IOQueue
    if (!ioQueue.addSocketContext(hSocket, connection))
    {
        removePhysicalConnection(connection);
        thePhysicalConnectionPool.returnObject(connection);
        //leave socket close to acceptor
        return false;
    }
 
    dispatcher.handleInitialize(*connection);
    return true;
}
}
