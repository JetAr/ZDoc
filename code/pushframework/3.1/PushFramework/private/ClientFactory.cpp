/********************************************************************
	File :			ClientFactoryImpl.cpp
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
#include "ClientFactory.h"


#include "../include/Server.h"
#include "../include/ServerOptions.h"
#include "LogicalConnectionImpl.h"
#include "PhysicalConnection.h"
#include "ServerImpl.h"
#include "ServerStats.h"
#include "BroadcastManager.h"
#include "Dispatcher.h"
#include "GarbageCollector.h"
#include "ScopedLock.h"
#include "BroadcastStreamer.h"
#include "PhysicalConnectionPool.h"

namespace PushFramework
{

ClientFactory clientFactoryImpl;

ClientFactory::ClientFactory()
{
    nClientsCount = 0;
    ::InitializeCriticalSection(&cs);
	::InitializeCriticalSection(&csChannelMap);
	isPermitOnly = false;
}

ClientFactory::~ClientFactory(void)
{
    ::DeleteCriticalSection(&cs);
	::DeleteCriticalSection(&csChannelMap);
}

void ClientFactory::addLogicalConnection( LogicalConnectionImpl* pClient )
{
    ScopedLock csLock(cs);
    //    
	if (pClient->IsMonitor())
	{
		monitorSet.insert(pClient);
	}
	else
	{
		nClientsCount++;
		clientSet.insert(pClient);
	}

	pClient->IncrementUsage();
}

Server* ClientFactory::getServer()
{
    return pServerImpl->getFacade();
}

bool ClientFactory::disconnect( LogicalConnectionImpl* pLogicalConnection, bool waitForPendingPackets , int closeReason)
{
    if (!removeIfExisting(pLogicalConnection))
    {
        return false;
    }

	disconnectIntern(pLogicalConnection, waitForPendingPackets, closeReason);
	return true;
}

bool ClientFactory::removeIfExisting( LogicalConnectionImpl* pLogicalConnection )
{
    ScopedLock csLock(cs);

	logicalConnectionSetT& set = pLogicalConnection->IsMonitor() ? monitorSet :  clientSet;
	logicalConnectionSetT::iterator it = set.find(pLogicalConnection);
	if (it != set.end())
	{
		set.erase(it);

		if (!pLogicalConnection->IsMonitor())
			nClientsCount--;

		return true;
	}

	return false;
}

unsigned int ClientFactory::getClientCount()
{
    return nClientsCount;
}

void ClientFactory::returnClient( LogicalConnectionImpl* pClient )
{
    pClient->DecrementUsage();
}

void ClientFactory::disconnectIntern( LogicalConnectionImpl* pLogicalConnection, bool waitForPendingPackets, int closeReason )
{
	pLogicalConnection->pPhysicalConnection->closeConnection(waitForPendingPackets);

	if (!pLogicalConnection->IsMonitor())
	{
		//dispatcher.notifyObserversClientOut(pLogicalConnection->getKey());

		//Statistics :
		stats.addToCumul(ServerStats::VisitorsHitsOut, 1);
		stats.addToCumul(ServerStats::VisitorsOnline, -1);
		stats.addToDuration(ServerStats::VisitorsDuration, pLogicalConnection->getVisitDuration());

		pLogicalConnection->OnDisconnected((DisconnectionReason::Type) closeReason);
	}
	
	pLogicalConnection->pStreamer->removeItem(pLogicalConnection);
	garbageCollector.addDisposableClient(pLogicalConnection);
}


bool ClientFactory::isAddressAllowed( IN_ADDR SockAddr )
{
	long ip = SockAddr.s_addr;

	ipRangeListT& list = isPermitOnly ? permittedIPs : blockedIPs;
	bool bIsInList = false;


	for (ipRangeListT::iterator it = list.begin();
		it!= list.end();
		it++)
	{
		IPRange& ipRange = *it;

		long start = ipRange.startIP.s_addr;
		long stop = ipRange.endIP.s_addr;

		if (ip <= stop && ip >= start)
		{
			bIsInList = true;
			break;
		}
	}
	return isPermitOnly ? bIsInList : !bIsInList;
}

void ClientFactory::addIPRangeAccess( const char* ipStart, const char* ipStop, bool bPermit )
{
	isPermitOnly = bPermit;

	IPRange ipRange;

	ipRange.startIP.s_addr = inet_addr(ipStart);
	ipRange.endIP.s_addr = inet_addr(ipStop);

	ipRangeListT& list = bPermit ? permittedIPs : blockedIPs;

	list.push_back(ipRange);
}

bool ClientFactory::removePhysicalConnection( PhysicalConnection* connection )
{
	ScopedLock lock(csChannelMap);

	vectConnectionsT::iterator it = vectPendingPhysicalConnections.begin();

	while (it != vectPendingPhysicalConnections.end())
	{
		if ((*it) == connection)
		{
			vectPendingPhysicalConnections.erase(it);
			return true;
		}
		it ++;
	}

	return false;
}

void ClientFactory::scrutinize()
{
	ScopedLock csLock(cs);

	logicalConnectionSetT::iterator it = clientSet.begin();
	while (it!=clientSet.end())
	{
		LogicalConnectionImpl* pClient = *it;
		if (pClient->IsInactive())
		{
			nClientsCount--;
			bool waitForPendingPackets = pClient->OnBeforeIdleClose();
			disconnectIntern(pClient, waitForPendingPackets, DisconnectionReason::InactiveClient);
			clientSet.erase(it++);
		}
		else
			++it;
	}

	///////////
	scrutinizeChannels();

}

void ClientFactory::stop()
{
	//Remove all connections (Demux is already stopped):
	vectConnectionsT::iterator it = vectPendingPhysicalConnections.begin();
	while (it != vectPendingPhysicalConnections.end())
	{
		PhysicalConnection* connection = *it;
		disposePhysicalConnection(connection);
		it = vectPendingPhysicalConnections.erase(it);
	}
}

void ClientFactory::addPhysicalConnection( PhysicalConnection* connection )
{
	ScopedLock lock(csChannelMap);

	vectPendingPhysicalConnections.push_back(connection);
}

void ClientFactory::disposePhysicalConnection( PhysicalConnection* connection )
{
	thePhysicalConnectionPool.returnObject(connection);
}

void ClientFactory::scrutinizeChannels()
{
	ScopedLock lock(csChannelMap);

	vectConnectionsT::iterator it = vectPendingPhysicalConnections.begin();

	while (it != vectPendingPhysicalConnections.end())
	{
		PhysicalConnection* connection = *it;

		if (connection->getStatus() == PhysicalConnection::Connected)
		{
			// Verify expiry : 
			int nMaxDuration = connection->isObserverChannel() ? 40 : options.uLoginExpireDuration;
			if (connection->getLifeDuration() > nMaxDuration)
			{
				connection->closeConnection(false);
			}
		}

		// Delete disposable pending connections :
		if (connection->checkIfUnusedByIOWorkers())
		{
			disposePhysicalConnection(connection);

			it = vectPendingPhysicalConnections.erase(it);
		}
		else{			
			it++;
		}        
	}
}



}

