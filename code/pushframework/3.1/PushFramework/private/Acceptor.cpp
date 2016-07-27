/********************************************************************
	File :			Acceptor.cpp
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
#include "Acceptor.h"

#include "../include/ServerOptions.h"
#include "ServerImpl.h"
#include "ServerStats.h"
#include "ClientFactory.h"
#include "StopWatch.h"


namespace PushFramework
{

Acceptor::Acceptor()
{
}

Acceptor::~Acceptor()
{
}

bool Acceptor::handleAcceptedSocket( SOCKET clientSocket, SOCKADDR_IN address )
{
    //
    stats.addToCumul(ServerStats::VisitorsSYNs, 1);

	if (clientFactoryImpl.getClientCount() >= options.nMaxConnections)
	{
		return false;
	}

	//Check if address is not blocked :
	if (!clientFactoryImpl.isAddressAllowed(address.sin_addr))
	{
		return false;
	}

	return clientFactoryImpl.createPhysicalConnection(clientSocket, address, false, &listenerOptions);
}

}
