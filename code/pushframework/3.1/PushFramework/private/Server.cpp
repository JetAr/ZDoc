/********************************************************************
	File :			Server.cpp
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
#include "../include/Server.h"

#include "../include/ServerOptions.h"
#include "../include/Service.h"

#include "ServerImpl.h"
#include "ClientFactory.h"
#include "Utilities.h"
#include "MonitorAcceptor.h"


namespace PushFramework
{
Server::Server(void)
{
    pImpl = new ServerImpl(this);
}

Server::~Server(void)
{
    delete pImpl;
}

void Server::registerService( Service* pService )
{
    pService->pServer = this;
    //
    pImpl->registerService(pService);
}

bool Server::start(bool startInSeparateThread)
{
    return pImpl->start(startInSeparateThread);
}

void Server::stop()
{
    pImpl->stop();
}

void Server::pause()
{
    pImpl->pause();
}

void Server::resume()
{
    pImpl->resume();
}

bool Server::handleMonitorRequest( const char* command, char* lpOut )
{
    return false;
}

void Server::setServerInfos( const char* serverName )
{
    pImpl->setServeInfos(serverName);
}

bool Server::createListener( int port, ListenerOptions* pOptions )
{
    return pImpl->createListener(port, pOptions);
}


void Server::addBlockedIPRange( const char* ipStart, const char* ipStop )
{
    clientFactoryImpl.addIPRangeAccess(ipStart, ipStop, false);
}

void Server::addPermitIPRange( const char* ipStart, const char* ipStop )
{
    clientFactoryImpl.addIPRangeAccess(ipStart, ipStop, true);
}

void Server::OnStarted()
{

}

void Server::CreateQueue( BROADCASTQUEUE_NAME queueName, QueueOptions& queueOptions )
{
	pImpl->CreateQueue(queueName, queueOptions);
}

void Server::RemoveQueue( BROADCASTQUEUE_NAME queueName )
{
	pImpl->RemoveQueue(queueName);
}

bool Server::PushPacket( OutgoingPacket* pPacket, BROADCASTQUEUE_NAME queueName )
{
	return pImpl->PushPacket(pPacket, queueName);
}

bool Server::PushPacket( OutgoingPacket* pPacket, BROADCASTQUEUE_NAME queueName, BROADCASTPACKET_KEY killKey, int objectCategory )
{
	return pImpl->PushPacket(pPacket, queueName, killKey, objectCategory);
}

void Server::RemovePacketFromQueue( BROADCASTPACKET_KEY killKey, int objectCategory, BROADCASTQUEUE_NAME queueName )
{
	pImpl->RemovePacketFromQueue(killKey, objectCategory, queueName);
}

void Server::setMessageFactory( MessageFactory* pMsgFactory )
{
	pImpl->setMessageFactory(pMsgFactory);
}


ConnectionContext* Server::createConnectionContext()
{
	return NULL;
}

OutgoingPacket* Server::getChallenge(ConnectionContext* pConnectionContext)
{
	return NULL;
}


}
