/********************************************************************
	File :			ServerImpl.h
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
#ifndef ServerImpl__INCLUDED
#define ServerImpl__INCLUDED

#pragma once




namespace PushFramework
{

///
class ClientFactory;
class Service;
class BroadcastStreamer;
class MonitorProtocol;
class Demultiplexor;
class Dispatcher;
class ServerStats;
class ChannelFactory;
class IOQueue;
class BroadcastManager;
class ClientFactory;
class Acceptor;
class Server;
class GarbageCollector;
struct Version;
struct ListenerOptions;
struct QueueOptions;
class LogicalConnectionImpl;

class ServerImpl
{
public:
    ServerImpl(Server* pFacade);
    ~ServerImpl(void);

    void setServeInfos(const char* serverInfos);

	void setMessageFactory( MessageFactory* pMsgFactory );
    bool createListener( int port, ListenerOptions* pOptions );
    void registerService( Service* pService);
    
	bool start(bool startInSeparateThread);
    void stop();
    void pause();
    void resume();

public:
	MessageFactory& getMessageFactory();
    Server* getFacade();
	string getServerInfos();

private:
    Server* pFacade;
    string serverInfos;

    // User defined
	MessageFactory* pMsgFactory;

#ifdef Plateform_Windows
    HANDLE			m_hEventTimer;	// Used for sleeping during interval period.
    HANDLE			m_hEventAbort; // set by stop function.
    //In case of an asynchronous start :
    HANDLE hThread;
    static DWORD WINAPI threadProc(LPVOID WorkContext);
#else
#ifdef Plateform_Linux
    pthread_cond_t abortCond;// set by stop function.
    //In case of an asynchronous start :
    pthread_t hThread;
    static void* threadProc(void* WorkContext);
#endif
#endif

private:
    unsigned int uGCPeriod;
    int nSecsGCCount;
    int nSecsPerfObservCount;

	void eternal_loop();
    void DoPeriodicJobs();

private:
	typedef std::vector<BroadcastStreamer*> streamersListT;
	streamersListT streamersList;
	BroadcastStreamer* monitorsStreamer;

	typedef unordered_map<int, Acceptor*> listenersMapT;
	listenersMapT listenersMap;

public:
	void startStreamers();
	void stopStreamers();
	void reshuffleStreamers();
	void reshuffleMonitorsStreamer();

	void addClientToStreamers(LogicalConnectionImpl* pLogicalConnection);

	void CreateQueue(BROADCASTQUEUE_NAME queueName, QueueOptions& queueOptions);
	void RemoveQueue(BROADCASTQUEUE_NAME queueName);
	bool PushPacket(OutgoingPacket* pPacket, BROADCASTQUEUE_NAME queueName);
	bool PushPacket(OutgoingPacket* pPacket, BROADCASTQUEUE_NAME queueName, BROADCASTPACKET_KEY killKey, int objectCategory);
	void RemovePacketFromQueue(BROADCASTPACKET_KEY killKey, int objectCategory, BROADCASTQUEUE_NAME queueName);

private:
	bool StartListening();
	void StopListening();

private:
	unsigned int calculateMaxBufferSizePerMessage();
	unsigned int calculateAdditionalBuffersForProtocols(unsigned int nMaxPoolConnections , unsigned int nMaxThreads);
	bool initializeProtocolContextPools(unsigned int nMaxPoolConnections);
};

extern ServerImpl *pServerImpl;

}
#endif // ServerImpl__INCLUDED

