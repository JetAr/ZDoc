#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef JETBYTE_TOOLS_WIN32_SOCKET_SERVER_INCLUDED__
#define JETBYTE_TOOLS_WIN32_SOCKET_SERVER_INCLUDED__
///////////////////////////////////////////////////////////////////////////////
//
// File           : $Workfile: SocketServer.h $
// Version        : $Revision: 37 $
// Function       :
//
// Author         : $Author: Len $
// Date           : $Date: 12/08/02 12:04 $
//
// Notes          :
//
// Modifications  :
//
// $Log: /Web Articles/SocketServers/ThreadPoolLargePacketEchoServer/JetByteTools/Win32Tools/SocketServer.h $
//
// 37    12/08/02 12:04 Len
// Lint issues.
//
// 36    12/08/02 11:41 Len
// The derived class is now informed when a client resets the connection.
// The user of a socket can check to see if it's connected (in either
// direction) Read and Write calls on a disconnected socket now report
// their failure.
//
// 35    12/08/02 9:26 Len
// Started to change how we handle read/write errors.
//
// 34    9/08/02 13:04 Len
// Lint issues.
//
// 33    6/08/02 17:40 Len
// Added a helper method, IsValid(), which does the check for
// (INVALID_SOCKET != m_socket) and locks appropriately so as to remove
// the race conditions during socket closure.
// We now don't call the OnClientClose() notification unless the client
// has closed the socket - we used to call it even if the socket closure
// was due to the server closing the socket.Thanks to David McConnell for
// reporting both issues and suggesting the fixes.
//
// 32    6/08/02 11:51 Len
// Switched to using a critical section per socket to do some quick
// performance comparison tests.
//
// 31    29/07/02 10:02 Len
// GetServerDataPtr() is now static.
//
// 30    22/07/02 18:27 Len
// Added helper class that makes it easier to write servers that perform
// their filtering on a thread other than an IO thread.
//
// 29    17/07/02 10:56 Len
// Allow derived classes more scope to override some operations.
//
// 28    15/07/02 9:53 Len
// Added the concept of posting 0 byte reads (for servers with lots of
// connected clients, so we dont use up the non-paged pool). When a 0 byte
// read completes it means that data can be read immediately, so we issue
// a normal read.
// Fixed a race condition between the assignment of the read sequence
// number and the issuing of the read.
//
// 27    9/07/02 18:33 Len
// Use of sequence numbers is now optional.
//
// 26    6/07/02 14:08 Len
// All buffers sent and received now contain sequence numbers. These are
// maintained on a per socket basis. To allow filters to add/remove
// individual reads/writes yet still be able to ensure that they process
// the application's read/write requests in the correct sequence there are
// pre and post filtering sequence numbers. All socket writes now check
// the sequence numbers to ensure that buffers are transmitted in the
// correct sequence, this is required because we post our write requests
// to the io thread pool and though they come out of the IOCP in the
// correct order, if there are multiple threads in the io pool then they
// could be transmitted out of sequence.
//
// 25    4/07/02 15:43 Len
// We now use protected inheritance from CIOBuffer::Allocator so that
// derived classes can use us as an allocator.
//
// 24    1/07/02 22:33 Len
// Added the concept of 'filtering'. A derived class can now opt to filter
// certain operations, such as a read completion. This allows a derived
// class to process the data before it is passed to the ReadComplete()
// event. This is useful for such things as ensuring that multiple
// outstanding reads complete in the order that they were issued, or
// adding encryption to the data stream below the application level.
//
// 23    29/06/02 18:25 Len
// Moved all callbacks to the SocketServer class. We now provide a default
// implementation for the WorkerThread. This raises its events via the
// socket server. The user now only has to provide one derived class
// rather than two. We now provide a default implementation for listening
// socket creation.
//
// 22    28/06/02 13:44 Len
// Allow derived class to intercept socket closure and inform derived
// class of client closure.
//
// 21    27/06/02 16:05 Len
// Fixed how we handle socket closure. We now handle the client sending a
// recv shutdown correctly and allow the server code to issue a send
// shutdown and for it to be actioned after all pending writes have been
// sent.
//
// 20    7/06/02 14:13 Len
// Lint issues.
//
// 19    5/06/02 19:17 Len
// Abortive socket closure is now done by an IO pool worker thread. This
// is a workaround for a problem with the COM wrapper.
//
// 18    29/05/02 12:05 Len
// Lint issues.
//
// 17    26/05/02 15:10 Len
// Factored out common 'user data' code into a mixin base class.
//
// 16    24/05/02 12:13 Len
// Refactored all the linked list stuff for the sockets into a NodeList
// class.
//
// 15    21/05/02 11:36 Len
// User data can now be stored/retrieved as either an unsigned long or a
// void *.
// A CIOBuffer containing the client's address is now passed with
// OnConnectionEstablished().
//
// 14    21/05/02 8:33 Len
// Allow derived class to flush buffer allocator in destructor so that it
// can receive notifications about buffer release.
//
// 13    21/05/02 8:05 Len
// SocketServer now derives from the buffer allocator.
//
// 12    20/05/02 23:17 Len
// Updated copyright and disclaimers.
//
// 11    20/05/02 17:26 Len
// Merged OnNewConnection() into OnConnectionEstablished().
// We now pass the socket to OnConnectionClosed() so that the derived
// class can dealocate any per connection user data when the connection is
// closed.
//
// 10    20/05/02 14:45 Len
// SocketServer doesn't need to pass allocator to WorkerThread.
//
// 9     20/05/02 14:38 Len
// WorkerThread never needs to use the allocator.
//
// 8     20/05/02 8:09 Len
// Moved the concept of the io operation used for the io buffer into the
// socket server. The io buffer now simply presents 'user data' access
// functions. Added a similar concept of user data to the socket class so
// that users can associate their own data with a connection . Derived
// class is now notified when a connection occurs so that they can send a
// greeting or request a read, etc.
// General code cleanup and refactoring.
//
// 7     16/05/02 21:35 Len
// Users now signal that we're finished with a socket by calling
// Shutdown() rather than Close().
//
// 6     14/05/02 14:37 Len
// Expose CThread::Start() using a using declaration rather than a
// forwarding function.
// Lint cleanup.
//
// 5     14/05/02 13:53 Len
// We now explicitly start the thread pool rather than allowing it to
// start itself in the constructor. There was a race condition over the
// completion of construction of derived classes and the first access to
// the pure virtual functions.
// Refactored some of the socket code to improve encapsulation.
//
// 4     13/05/02 13:44 Len
// Added OnError() methods so that derived class can do something about
// obscure error situations.
// Added a 'max free sockets' concept so that the socket pool can shrink
// as well as grow. This exposed a problem in how we were handling sockets
// - knowing when we can actually delete them was complicated so they're
// now reference counted.
//
// 3     11/05/02 11:04 Len
// Made CreateListeningSocket() pure virtual as there are an infinte
// number of ways that you can create the listening socket so we'll allow
// the derived class to specify exactly how it's done.
//
// 2     10/05/02 19:25 Len
// Lint options and code cleaning.
//
// 1     9/05/02 18:47 Len
//
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1997 - 2002 JetByte Limited.
//
// JetByte Limited grants you ("Licensee") a non-exclusive, royalty free,
// licence to use, modify and redistribute this software in source and binary
// code form, provided that i) this copyright notice and licence appear on all
// copies of the software; and ii) Licensee does not utilize the software in a
// manner which is disparaging to JetByte Limited.
//
// This software is provided "as is" without a warranty of any kind. All
// express or implied conditions, representations and warranties, including
// any implied warranty of merchantability, fitness for a particular purpose
// or non-infringement, are hereby excluded. JetByte Limited and its licensors
// shall not be liable for any damages suffered by licensee as a result of
// using, modifying or distributing the software or its derivatives. In no
// event will JetByte Limited be liable for any lost revenue, profit or data,
// or for direct, indirect, special, consequential, incidental or punitive
// damages, however caused and regardless of the theory of liability, arising
// out of the use of or inability to use software, even if JetByte Limited
// has been advised of the possibility of such damages.
//
// This software is not designed or intended for use in on-line control of
// aircraft, air traffic, aircraft navigation or aircraft communications; or in
// the design, construction, operation or maintenance of any nuclear
// facility. Licensee represents and warrants that it will not use or
// redistribute the Software for such purposes.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//
// Class member is a reference
//lint -esym(1725, CSocketServer::m_allocator)
//lint -esym(1725, Socket::m_server)
//lint -esym(1725, WorkerThread::m_allocator)
//lint -esym(1725, WorkerThread::m_iocp)
//lint -esym(1725, WorkerThread::m_server)
//
// Private copy constructor
//lint -esym(1704, CSocketServer::CSocketServer)
//lint -esym(1704, Socket::Socket)
//lint -esym(1704, WorkerThread::WorkerThread)
//
// No default constructor
//lint -esym(1712, CSocketServer)
//lint -esym(1712, Socket)
//lint -esym(1712, WorkerThread)
//lint -esym(1712, SequenceData)
//
// Base class destructor isnt virtual
//lint -esym(1509, CUsesWinsock)
//lint -esym(1509, UserData)
//lint -esym(1509, ServerData)
//
// Data member hides inherited member
//lint -esym(1516, Allocator::m_activeList)
//lint -esym(1516, Allocator::m_freeList)
//
//lint -esym(1748, COpaqueUserData)
//
// Ignoring return value of function
//lint -esym(534, Socket::Read)
//lint -esym(534, Socket::Write)
//
///////////////////////////////////////////////////////////////////////////////

#include "UsesWinsock.h"
#include "Thread.h"
#include "CriticalSection.h"
#include "IOCompletionPort.h"
#include "IOBuffer.h"
#include "ManualResetEvent.h"
#include "NodeList.h"
#include "OpaqueUserData.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools
{
namespace Win32
{

///////////////////////////////////////////////////////////////////////////////
// CSocketServer
///////////////////////////////////////////////////////////////////////////////

class CSocketServer :
    protected CThread,
    private CUsesWinsock,
    protected CIOBuffer::Allocator
{
public:

    class Socket;

    friend class Socket;

    class FilteringHelper;

    friend class FilteringHelper;

    virtual ~CSocketServer();

    using CThread::Start;

    void StartAcceptingConnections();
    void StopAcceptingConnections();

    void InitiateShutdown();

    void WaitForShutdownToComplete();

protected :

    class WorkerThread;

    friend class WorkerThread;

    CSocketServer(
        unsigned long addressToListenOn,
        unsigned short portToListenOn,
        size_t maxFreeSockets,
        size_t maxFreeBuffers,
        size_t bufferSize = 1024,
        size_t numThreads = 0,
        bool useSequenceNumbers = true,
        bool postZeroByteReads = false);

    void PostFilteredReadRequest(
        Socket *pSocket,
        CIOBuffer *pBuffer);

    void PostFilteredReadCompleted(
        Socket *pSocket,
        CIOBuffer *pBuffer);

    void PostFilteredWriteRequest(
        Socket *pSocket,
        CIOBuffer *pBuffer);

    void PostFilteredWriteCompleted(
        Socket *pSocket,
        CIOBuffer *pBuffer);

    static void SetServerDataPtr(
        Socket *pSocket,
        void *pData);

    static void *GetServerDataPtr(
        const Socket *pSocket);

    static void WriteCompleted(
        Socket *pSocket);

    static CSocketServer &GetServer(
        Socket *pSocket);

    //lint -e{1768} Virtual function has different access specifier to base class
    virtual int Run();

    enum ConnectionErrorSource
    {
        ZeroByteReadError,
        ReadError,
        WriteError
    };

private :

    class UserData;
    class ServerData;

    // Override this to create your worker thread

    virtual WorkerThread *CreateWorkerThread(
        CIOCompletionPort &iocp);

    // Override this to create the listening socket of your choice

    virtual SOCKET CreateListeningSocket(
        unsigned long address,
        unsigned short port);

    // Interface for derived classes to receive state change notifications...

    virtual void OnStartAcceptingConnections() {}
    virtual void OnStopAcceptingConnections() {}
    virtual void OnShutdownInitiated() {}
    virtual void OnShutdownComplete() {}

    virtual void OnConnectionCreated() {}

    virtual void OnConnectionEstablished(
        Socket *pSocket,
        CIOBuffer *pAddress) = 0;

    virtual void OnConnectionClientClose(
        Socket * /*pSocket*/) {}

    virtual void OnConnectionReset(
        Socket * /*pSocket*/) {}

    virtual bool OnConnectionClosing(
        Socket * /*pSocket*/)
    {
        return false;
    }

    virtual void OnConnectionClosed(
        Socket * /*pSocket*/) {}

    virtual void OnConnectionDestroyed() {}

    virtual void OnConnectionError(
        ConnectionErrorSource source,
        Socket *pSocket,
        CIOBuffer *pBuffer,
        DWORD lastError);

    virtual void OnError(
        const _tstring &message);

    virtual void OnBufferCreated() {}
    virtual void OnBufferAllocated() {}
    virtual void OnBufferReleased() {}
    virtual void OnBufferDestroyed() {}

    virtual void OnThreadCreated() {}
    virtual void OnThreadBeginProcessing() {}
    virtual void OnThreadEndProcessing() {}
    virtual void OnThreadDestroyed() {}

    virtual bool FilterReadRequest(
        Socket *pSocket,
        CIOBuffer **ppBuffer);

    virtual bool FilterReadCompleted(
        Socket *pSocket,
        CIOBuffer **ppBuffer);

    virtual bool FilterWriteRequest(
        Socket *pSocket,
        CIOBuffer **ppBuffer);

    virtual bool FilterWriteCompleted(
        Socket *pSocket,
        CIOBuffer **ppBuffer);

    virtual bool FilterSocketShutdown(
        Socket *pSocket,
        int how);

    virtual bool FilterSocketClose(
        Socket *pSocket);

    virtual void ReadCompleted(
        Socket *pSocket,
        CIOBuffer *pBuffer) = 0;

    virtual void WriteCompleted(
        Socket *pSocket,
        CIOBuffer *pBuffer);

    void ReleaseSockets();

    Socket *AllocateSocket(
        SOCKET theSocket);

    void ReleaseSocket(
        Socket *pSocket);

    void DestroySocket(
        Socket *pSocket);

    enum IO_Operation
    {
        IO_Zero_Byte_Read_Request,
        IO_Zero_Byte_Read_Completed,
        IO_Read_Request,
        IO_Read_Completed,
        IO_Write_Request,
        IO_Write_Completed,
        IO_Filtered_Read_Request,
        IO_Filtered_Read_Completed,
        IO_Filtered_Write_Request,
        IO_Filtered_Write_Completed
    };

    void PostIoOperation(
        Socket *pSocket,
        CIOBuffer *pBuffer,
        IO_Operation operation);

    const size_t m_numThreads;

    CCriticalSection m_listManipulationSection;

    typedef JetByteTools::TNodeList<Socket> SocketList;

    SocketList m_activeList;
    SocketList m_freeList;

    SOCKET m_listeningSocket;

    CIOCompletionPort m_iocp;

    CManualResetEvent m_shutdownEvent;

    CManualResetEvent m_acceptConnectionsEvent;

    const unsigned long m_address;
    const unsigned short m_port;

    const size_t m_maxFreeSockets;

    const bool m_useSequenceNumbers;

    const bool m_postZeroByteReads;

    // No copies do not implement
    CSocketServer(const CSocketServer &rhs);
    CSocketServer &operator=(const CSocketServer &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::UserData
///////////////////////////////////////////////////////////////////////////////

class CSocketServer::UserData : public COpaqueUserData
{
    // UserData is a shim class that allows Socket to inherit from two
    // COpaqueUserData bases without ambiguity
};

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::ServerData
///////////////////////////////////////////////////////////////////////////////

class CSocketServer::ServerData : public COpaqueUserData
{
    // ServerData is a shim class that allows Socket to inherit from two
    // COpaqueUserData bases without ambiguity
};

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::Socket
///////////////////////////////////////////////////////////////////////////////

class CSocketServer::Socket :
    public CNodeList::Node,
    public CSocketServer::UserData,
    protected CSocketServer::ServerData
{
public :

    friend class CSocketServer;
    friend class CSocketServer::WorkerThread;

    using UserData::SetUserData;
    using UserData::GetUserData;
    using UserData::SetUserPtr;
    using UserData::GetUserPtr;

    bool Read(
        CIOBuffer *pBuffer = 0,
        bool throwOnFailure = false);

    bool Write(
        const char *pData,
        size_t dataLength,
        bool throwOnFailure = false);

    bool Write(
        const BYTE *pData,
        size_t dataLength,
        bool throwOnFailure = false);

    bool Write(
        CIOBuffer *pBuffer,
        bool throwOnFailure = false);

    void AddRef();
    void Release();

    void Shutdown(
        int how = SD_BOTH);

    void Close();

    void AbortiveClose();

    bool IsConnected(
        int how = SD_BOTH) const;

private :

    Socket(
        CSocketServer &server,
        SOCKET socket,
        bool useSequenceNumbers);

    ~Socket();

    void Attach(
        SOCKET socket);

    void *GetServerDataPtr() const
    {
        return ServerData::GetUserPtr();
    }

    void SetServerDataPtr(void *pData)
    {
        ServerData::SetUserPtr(pData);
    }

    void WriteCompleted();
    bool WritePending();

    void OnClientClose();
    void OnConnectionReset();

    bool IsValid();

    void InternalClose();

    void OnConnectionError(
        CSocketServer::ConnectionErrorSource source,
        CIOBuffer *pBuffer,
        DWORD lastError);

    CCriticalSection m_crit;

    CSocketServer &m_server;
    SOCKET m_socket;

    long m_ref;

    long m_outstandingWrites;

    // TODO we could store all of these 1 bit flags in with the outstanding write count..

    bool m_readShutdown;
    bool m_writeShutdown;
    bool m_closing;
    long m_clientClosed;

    enum SequenceType
    {
        ReadSequenceNo,
        WriteSequenceNo,
        FilteredReadSequenceNo,
        FilteredWriteSequenceNo,
        MaxSequenceNo
    };

    long GetSequenceNumber(
        SequenceType type);

    CIOBuffer *GetNextBuffer(
        CIOBuffer *pBuffer = 0);

    struct SequenceData
    {
        explicit SequenceData(
            CCriticalSection &section);

        void Reset();

        long m_numbers[4];

        CIOBuffer::InOrderBufferList m_outOfSequenceWrites;
    };

    SequenceData *m_pSequenceData;

    // No copies do not implement
    Socket(const Socket &rhs);
    Socket &operator=(const Socket &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::WorkerThread
///////////////////////////////////////////////////////////////////////////////

class CSocketServer::WorkerThread : public CThread
{
public :

    WorkerThread(
        CSocketServer &server,
        CIOCompletionPort &iocp);

    virtual ~WorkerThread();

    void InitiateShutdown();

    void WaitForShutdownToComplete();

protected :

    //lint -e{1768} Virtual function has different access specifier to base class
    virtual int Run();

    virtual void HandleOperation(
        Socket *pSocket,
        CIOBuffer *pBuffer,
        DWORD dwIoSize,
        bool weClosedSocket);

private :

    void ZeroByteRead(
        Socket *pSocket,
        CIOBuffer *pBuffer) const;

    void Read(
        Socket *pSocket,
        CIOBuffer *pBuffer) const;

    void Write(
        Socket *pSocket,
        CIOBuffer *pBuffer) const;

    CSocketServer &m_server;
    CIOCompletionPort &m_iocp;

    // No copies do not implement
    WorkerThread(const WorkerThread &rhs);
    WorkerThread &operator=(const WorkerThread &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::FilteringHelper
///////////////////////////////////////////////////////////////////////////////

class CSocketServer::FilteringHelper
{
public :

    static void PostFilteredReadRequest(
        Socket *pSocket,
        CIOBuffer *pBuffer);

    static void PostFilteredReadCompleted(
        Socket *pSocket,
        CIOBuffer *pBuffer);

    static void PostFilteredWriteRequest(
        Socket *pSocket,
        CIOBuffer *pBuffer);

    static void PostFilteredWriteCompleted(
        Socket *pSocket,
        CIOBuffer *pBuffer);

    static void WriteCompleted(
        Socket *pSocket);
};

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Win32
} // End of namespace JetByteTools

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -restore
//
///////////////////////////////////////////////////////////////////////////////

#endif // JETBYTE_TOOLS_WIN32_SOCKET_SERVER_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////

