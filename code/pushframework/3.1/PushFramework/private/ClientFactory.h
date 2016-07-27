/********************************************************************
	File :			ClientFactoryImpl.h
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
#ifndef ClientFactoryImpl__INCLUDED
#define ClientFactoryImpl__INCLUDED

#pragma once

namespace PushFramework
{

	typedef struct IPRange
	{
		IN_ADDR startIP;
		IN_ADDR endIP;
	} IPRange;

struct ListenerOptions;
class ClientFactory;
class LogicalConnectionImpl;
class ServerImpl;
class Server;
class PhysicalConnection;
typedef std::vector<PhysicalConnection*> vectConnectionsT;
typedef std::unordered_set<LogicalConnectionImpl*> logicalConnectionSetT;

class ClientFactory
{
public:
    ClientFactory();
    ~ClientFactory(void);
    void addLogicalConnection(LogicalConnectionImpl* pClient);
    
	bool disconnect(LogicalConnectionImpl* pLogicalConnection, bool waitForPendingPackets, int closeReason);
	void disconnectIntern( LogicalConnectionImpl* pLogicalConnection, bool waitForPendingPackets, int closeReason );

    void returnClient(LogicalConnectionImpl* pClient);
    unsigned int getClientCount();
	void scrutinize();

	void scrutinizeChannels();

public:
	void addIPRangeAccess(const char* ipStart, const char* ipStop, bool bPermit);
	bool isAddressAllowed(IN_ADDR	SockAddr);
	bool createPhysicalConnection(SOCKET hSocket, SOCKADDR_IN address, bool isObserver, ListenerOptions* pListenerOptions);
	void addPhysicalConnection( PhysicalConnection* connection );
	bool removePhysicalConnection(PhysicalConnection* connection);
	void stop();
	void disposePhysicalConnection(PhysicalConnection* connection);

private:
    bool removeIfExisting(LogicalConnectionImpl* pLogicalConnection);
    Server* getServer();

private:
    logicalConnectionSetT clientSet;
    CRITICAL_SECTION cs;
    unsigned int nClientsCount;
    //

private:
	logicalConnectionSetT monitorSet;

private:
	CRITICAL_SECTION csChannelMap;
	vectConnectionsT vectPendingPhysicalConnections;
	//
	typedef std::vector<IPRange> ipRangeListT;
	ipRangeListT blockedIPs;
	ipRangeListT permittedIPs;
	//
	bool isPermitOnly;
};
extern ClientFactory clientFactoryImpl;
}

#endif // ClientFactoryImpl__INCLUDED
