/********************************************************************
	File :			LogicalConnection.cpp
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
#include "../include/LogicalConnection.h"
#include "LogicalConnectionImpl.h"


namespace PushFramework
{

LogicalConnection::LogicalConnection(void)
{
	pImpl = new LogicalConnectionImpl(this);
}

LogicalConnection::~LogicalConnection(void)
{
	delete pImpl;
}

SendResult::Type LogicalConnection::PushPacket( OutgoingPacket* pPacket)
{
	return pImpl->PushPacket(pPacket);
}

SendResult::Type LogicalConnection::TryPushPacket( OutgoingPacket* pPacket )
{
	return pImpl->TryPushPacket(pPacket);
}

double LogicalConnection::getVisitDuration()
{
   return pImpl->getVisitDuration();
}

bool LogicalConnection::IsInactive()
{
	return pImpl->IsInactive();
}

void LogicalConnection::OnReadyForSend( unsigned int nAvailableSpace )
{
	//
}

bool LogicalConnection::SubscribeToQueue( BROADCASTQUEUE_NAME queueName, bool ignorePreviousPackets /*= false*/ )
{
	return pImpl->SubscribeToQueue(queueName, ignorePreviousPackets);
}

void LogicalConnection::UnSubscribeFromQueue( BROADCASTQUEUE_NAME queueName )
{
	pImpl->UnSubscribeFromQueue(queueName);
}

void LogicalConnection::UnSubscribeFromAll()
{
	pImpl->UnSubscribeFromAll();
}

LogicalConnectionImpl& LogicalConnection::getImpl()
{
	return *pImpl;
}

void LogicalConnection::handleRequest( IncomingPacket* pRequest )
{
	//
}

void LogicalConnection::OnDisconnected( DisconnectionReason::Type closeReason )
{
	//
}

void LogicalConnection::OnConnected()
{
	//
}

bool LogicalConnection::OnBeforeIdleClose()
{
	return false;
}

void LogicalConnection::Disconnect( bool waitForPendingPackets )
{
	return pImpl->Disconnect(waitForPendingPackets);
}

void LogicalConnection::IncrementUsage()
{
	pImpl->IncrementUsage();
}

void LogicalConnection::DecrementUsage()
{
	pImpl->DecrementUsage();
}



}

