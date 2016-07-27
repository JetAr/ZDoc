/********************************************************************
	File :			PhysicalConnection.h
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
#ifndef PhysicalConnection__INCLUDED
#define PhysicalConnection__INCLUDED

#pragma once


#ifdef Plateform_Windows
#include "IOBuffer.h"
#endif

#include "Connection.h"


namespace PushFramework
{

class ServerImpl;
struct ListenerOptions;
class LogicalConnectionImpl;
class GarbageCollector;
class ConnectionContext;

class PhysicalConnection : public Connection
{
    friend class GarbageCollector;
public:
    enum
    {
        Disposable,
        WaitingForWrite,
        Connected,
        Attached,
    };

    PhysicalConnection();
    ~PhysicalConnection();

	void reset(SOCKET s, SOCKADDR_IN address, bool bIsObserver, ListenerOptions* pListenerOptions);

	void Recycle();
	
    int getStatus();
    Protocol* getProtocol();
	MessageFactory& getMessageFactory();
    bool isObserverChannel();
    LogicalConnectionImpl* getLogicalConnectionImpl();

    bool checkIfUnusedByIOWorkers();
    int getIoWorkersReferenceCounter() const;
    void incrementIoWorkersReferenceCounter();
    void decrementIoWorkersReferenceCounter();

    UINT getPeerPort();
    std::string getPeerIP();

    double getLifeDuration();
	double getTimeToLastReceive();
private:
    bool bIsObserver;
    int status;//status
    long ioWorkersReferenceCounter;
    time_t dtCreationTime;
	time_t dtLastReceiveTime;
    CRITICAL_SECTION csLock;
    LogicalConnectionImpl* pLogicalConnection;

public:
    void closeConnection(bool bWaitForSendsToComplete);
    void postReceive();
    bool readReceivedBytes(RecyclableBuffer& incomingBytes, int dwIoSize);
    SendResult::Type pushPacket(OutgoingPacket* pPacket);

    SendResult::Type pushPacketCommon( OutgoingPacket* pPacket );
	int pushBytes(Buffer& buffer, Protocol* pProtocol);

    SendResult::Type tryPushPacket(OutgoingPacket* pPacket);
    bool OnSendCompleted(int dwIoSize, bool& bIsBufferIdle);
	Buffer& GetSendBuffer();
	bool IsWriteInProgress();
private:

#ifdef Plateform_Windows
	
	IOBuffer readIoBuffer;
	IOBuffer writeIoBuffer;

#endif
    RecyclableBuffer oBuffer;
    bool bWriteInProgress;

    SOCKET socket;
    UINT rPeerPort;
    std::string rPeerIP;
	ListenerOptions* pListenerOptions;

    bool WriteBytes();
    void CloseSocket();

private:
    ConnectionContext* pConnectionContext;
	bool isInitialized;
public:
    void SetConnectionContext(ConnectionContext* pConnectionContext);
    ConnectionContext* GetConnectionContext();
    void attachToClient(LogicalConnectionImpl* pLogicalConnection);
    SOCKET getSocket();
	
protected:
	virtual void InitializeConnection();


};


}

#endif // PhysicalConnection__INCLUDED
