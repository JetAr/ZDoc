///////////////////////////////////////////////////////////////////////////////
//
// File           : $Workfile: SocketServer.cpp $
// Version        : $Revision: 42 $
// Function       :
//
// Author         : $Author: Len $
// Date           : $Date: 13/08/02 9:46 $
//
// Notes          :
//
// Modifications  :
//
// $Log: /Web Articles/SocketServers/EchoServer/JetByteTools/Win32Tools/SocketServer.cpp $
//
// 42    13/08/02 9:46 Len
// Recognise WSAECONNABORTED as a connection reset error.
//
// 41    12/08/02 12:04 Len
// Lint issues.
//
// 40    12/08/02 11:41 Len
// The derived class is now informed when a client resets the connection.
// The user of a socket can check to see if it's connected (in either
// direction) Read and Write calls on a disconnected socket now report
// their failure.
//
// 39    12/08/02 9:26 Len
// Started to change how we handle read/write errors.
//
// 38    10/08/02 13:19 Len
// Clean up socket's use of critical section. For now we're keeping the
// per socket critical section...
//
// 37    9/08/02 13:04 Len
// Lint issues.
//
// 36    6/08/02 17:40 Len
// Added a helper method, IsValid(), which does the check for
// (INVALID_SOCKET != m_socket) and locks appropriately so as to remove
// the race conditions during socket closure.
// We now don't call the OnClientClose() notification unless the client
// has closed the socket - we used to call it even if the socket closure
// was due to the server closing the socket.Thanks to David McConnell for
// reporting both issues and suggesting the fixes.
//
// 35    6/08/02 11:50 Len
// All calls to Output() are now wrapped in a DEBUG_ONLY() macro so they
// vanish completely from release builds.
// Switched to using a critical section per socket to do some quick
// performance comparison tests.
//
// 34    22/07/02 18:27 Len
// Added helper class that makes it easier to write servers that perform
// their filtering on a thread other than an IO thread.
//
// 33    17/07/02 10:56 Len
// Allow derived classes more scope to override some operations.
//
// 32    15/07/02 9:53 Len
// Added the concept of posting 0 byte reads (for servers with lots of
// connected clients, so we dont use up the non-paged pool). When a 0 byte
// read completes it means that data can be read immediately, so we issue
// a normal read.
// Fixed a race condition between the assignment of the read sequence
// number and the issuing of the read.
//
// 31    9/07/02 19:22 Len
// Bug fix. We were using the filtered read sequence number when we should
// have been using the unfiltered read sequence number.
//
// 30    9/07/02 18:33 Len
// Use of sequence numbers is now optional.
//
// 29    6/07/02 14:08 Len
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
// 28    4/07/02 15:43 Len
// Fixes to how we use WritePending.
//
// 27    1/07/02 22:33 Len
// Added the concept of 'filtering'. A derived class can now opt to filter
// certain operations, such as a read completion. This allows a derived
// class to process the data before it is passed to the ReadComplete()
// event. This is useful for such things as ensuring that multiple
// outstanding reads complete in the order that they were issued, or
// adding encryption to the data stream below the application level.
//
// 26    29/06/02 18:25 Len
// Moved all callbacks to the SocketServer class. We now provide a default
// implementation for the WorkerThread. This raises its events via the
// socket server. The user now only has to provide one derived class
// rather than two. We now provide a default implementation for listening
// socket creation.
//
// 25    28/06/02 13:44 Len
// Allow derived class to intercept socket closure and inform derived
// class of client closure.
//
// 24    27/06/02 16:05 Len
// Fixed how we handle socket closure. We now handle the client sending a
// recv shutdown correctly and allow the server code to issue a send
// shutdown and for it to be actioned after all pending writes have been
// sent.
//
// 23    7/06/02 14:15 Len
// Changes due to change in CIOBuffer. The buffer now derives from
// OVERLAPPED so the explicit conversion functions are no longer required.
//
// 22    5/06/02 19:17 Len
// Abortive socket closure is now done by an IO pool worker thread. This
// is a workaround for a problem with the COM wrapper.
//
// 21    29/05/02 12:05 Len
// Lint issues.
//
// 20    26/05/02 15:10 Len
// Factored out common 'user data' code into a mixin base class.
//
// 19    24/05/02 12:13 Len
// Refactored all the linked list stuff for the sockets into a NodeList
// class.
//
// 18    21/05/02 11:36 Len
// User data can now be stored/retrieved as either an unsigned long or a
// void *.
// A CIOBuffer containing the client's address is now passed with
// OnConnectionEstablished().
//
// 17    21/05/02 8:33 Len
// Allow derived class to flush buffer allocator in destructor so that it
// can receive notifications about buffer release.
//
// 16    21/05/02 8:05 Len
// SocketServer now derives from the buffer allocator.
//
// 15    20/05/02 23:17 Len
// Updated copyright and disclaimers.
//
// 14    20/05/02 17:26 Len
// Merged OnNewConnection() into OnConnectionEstablished().
// We now pass the socket to OnConnectionClosed() so that the derived
// class can dealocate any per connection user data when the connection is
// closed.
//
// 13    20/05/02 14:45 Len
// SocketServer doesn't need to pass allocator to WorkerThread.
//
// 12    20/05/02 14:38 Len
// WorkerThread never needs to use the allocator.
//
// 11    20/05/02 8:09 Len
// Moved the concept of the io operation used for the io buffer into the
// socket server. The io buffer now simply presents 'user data' access
// functions. Added a similar concept of user data to the socket class so
// that users can associate their own data with a connection . Derived
// class is now notified when a connection occurs so that they can send a
// greeting or request a read, etc.
// General code cleanup and refactoring.
//
// 10    16/05/02 21:35 Len
// Users now signal that we're finished with a socket by calling
// Shutdown() rather than Close().
//
// 9     15/05/02 11:07 Len
// TX and RX data logging are now wrapped in a DEBUG_ONLY() macro as the
// call to DumpData() was occurring even though the output wasnt being
// logged. This change almost doubled the throughput of the server...
//
// 8     15/05/02 10:45 Len
// Enabled TX and RX data logging in debug build
//
// 7     14/05/02 14:37 Len
// Expose CThread::Start() using a using declaration rather than a
// forwarding function.
// Lint cleanup.
//
// 6     14/05/02 13:53 Len
// We now explicitly start the thread pool rather than allowing it to
// start itself in the constructor. There was a race condition over the
// completion of construction of derived classes and the first access to
// the pure virtual functions.
// Refactored some of the socket code to improve encapsulation.
//
// 5     13/05/02 13:44 Len
// Added OnError() methods so that derived class can do something about
// obscure error situations.
// Added a 'max free sockets' concept so that the socket pool can shrink
// as well as grow. This exposed a problem in how we were handling sockets
// - knowing when we can actually delete them was complicated so they're
// now reference counted.
//
// 4     11/05/02 11:05 Len
// Removed CreateListeningSocket() as it's now the responsibility of the
// derived class. General code cleaning.
//
// 3     10/05/02 19:52 Len
// Bug fix. During the code cleanup we'd renamed most, but not all
// instances of 'socket'...
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

#include "SocketServer.h"
#include "IOCompletionPort.h"
#include "Win32Exception.h"
#include "Utils.h"
#include "SystemInfo.h"
#include "Socket.h"

#include <vector>

#pragma comment(lib, "ws2_32.lib")

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//
// Symbol did not appear in the constructor initialiser list
//lint -esym(1928, CThread)
//lint -esym(1928, CUsesWinsock)
//lint -esym(1928, Node)
//lint -esym(1928, UserData)
//lint -esym(1928, ServerData)
//
// Symbol's default constructor implicitly called
//lint -esym(1926, CSocketServer::m_listManipulationSection)
//lint -esym(1926, CSocketServer::m_shutdownEvent)
//lint -esym(1926, CSocketServer::m_acceptConnectionsEvent)
//lint -esym(1926, CSocketServer::m_activeList)
//lint -esym(1926, CSocketServer::m_freeList)
//lint -esym(1926, Socket::m_crit)
//
// Member not defined
//lint -esym(1526, CSocketServer::CSocketServer)
//lint -esym(1526, CSocketServer::operator=)
//lint -esym(1526, Socket::Socket)
//lint -esym(1526, Socket::operator=)
//lint -esym(1526, WorkerThread::WorkerThread)
//lint -esym(1526, WorkerThread::operator=)
//
//lint -esym(534, InterlockedIncrement)   ignoring return value
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Using directives
///////////////////////////////////////////////////////////////////////////////

using std::vector;

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools
{
namespace Win32
{

///////////////////////////////////////////////////////////////////////////////
// Static helper methods
///////////////////////////////////////////////////////////////////////////////

static size_t CalculateNumberOfThreads(
    size_t numThreads);

///////////////////////////////////////////////////////////////////////////////
// CSocketServer
///////////////////////////////////////////////////////////////////////////////

CSocketServer::CSocketServer(
    unsigned long addressToListenOn,
    unsigned short portToListenOn,
    size_t maxFreeSockets,
    size_t maxFreeBuffers,
    size_t bufferSize /* = 1024 */,
    size_t numThreads /* = 0 */,
    bool useSequenceNumbers /* = true */,
    bool postZeroByteReads /* = false */)
    :  CIOBuffer::Allocator(bufferSize, maxFreeBuffers),
       m_numThreads(CalculateNumberOfThreads(numThreads)),
       m_listeningSocket(INVALID_SOCKET),
       m_iocp(0),
       m_address(addressToListenOn),
       m_port(portToListenOn),
       m_maxFreeSockets(maxFreeSockets),
       m_useSequenceNumbers(useSequenceNumbers),
       m_postZeroByteReads(postZeroByteReads)
{
}

CSocketServer::~CSocketServer()
{
}

void CSocketServer::ReleaseSockets()
{
    CCriticalSection::Owner lock(m_listManipulationSection);

    while (m_activeList.Head())
    {
        ReleaseSocket(m_activeList.Head());
    }

    while (m_freeList.Head())
    {
        DestroySocket(m_freeList.PopNode());
    }

    if (m_freeList.Count() + m_freeList.Count() != 0)
    {
        //lint -e{1933} call to unqualified virtual function
        OnError(_T("CSocketServer::ReleaseSockets() - Leaked sockets"));
    }
}

void CSocketServer::StartAcceptingConnections()
{
    if (m_listeningSocket == INVALID_SOCKET)
    {
        //lint -e{1933} call to unqualified virtual function
        OnStartAcceptingConnections();

        //lint -e{1933} call to unqualified virtual function
        m_listeningSocket = CreateListeningSocket(m_address, m_port);

        m_acceptConnectionsEvent.Set();
    }
}

void CSocketServer::StopAcceptingConnections()
{
    if (m_listeningSocket != INVALID_SOCKET)
    {
        m_acceptConnectionsEvent.Reset();

        if (0 != ::closesocket(m_listeningSocket))
        {
            //lint -e{1933} call to unqualified virtual function
            OnError(_T("CSocketServer::StopAcceptingConnections() - closesocket - ") + GetLastErrorMessage(::WSAGetLastError()));
        }

        m_listeningSocket = INVALID_SOCKET;

        //lint -e{1933} call to unqualified virtual function
        OnStopAcceptingConnections();
    }
}

void CSocketServer::InitiateShutdown()
{
    // signal that the dispatch thread should shut down all worker threads and then exit

    StopAcceptingConnections();

    {
        CCriticalSection::Owner lock(m_listManipulationSection);

        Socket *pSocket = m_activeList.Head();

        while (pSocket)
        {
            Socket *pNext = SocketList::Next(pSocket);

            pSocket->AbortiveClose();

            pSocket = pNext;
        }
    }

    m_shutdownEvent.Set();

    //lint -e{1933} call to unqualified virtual function
    OnShutdownInitiated();
}

void CSocketServer::WaitForShutdownToComplete()
{
    // if we havent already started a shut down, do so...

    InitiateShutdown();

    Wait();

    ReleaseSockets();
    Flush();
}

SOCKET CSocketServer::CreateListeningSocket(
    unsigned long address,
    unsigned short port)
{
    SOCKET s = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (s == INVALID_SOCKET)
    {
        throw CWin32Exception(_T("CSocket::CreateListeningSocket()"), ::WSAGetLastError());
    }

    CSocket listeningSocket(s);

    CSocket::InternetAddress localAddress(address, port);

    listeningSocket.Bind(localAddress);

    listeningSocket.Listen(5);

    return listeningSocket.Detatch();
}

CSocketServer::WorkerThread *CSocketServer::CreateWorkerThread(
    CIOCompletionPort &iocp)
{
    return new WorkerThread(*this, iocp);
}

int CSocketServer::Run()
{
    try
    {
        vector<WorkerThread *> workers;

        workers.reserve(m_numThreads);

        for (size_t i = 0; i < m_numThreads; ++i)
        {
            //lint -e{1933} call to unqualified virtual function
            WorkerThread *pThread = CreateWorkerThread(m_iocp);

            workers.push_back(pThread);

            pThread->Start();

            //lint -e{429} custodial pointer neither freed nor returned
        }

        HANDLE handlesToWaitFor[2];

        handlesToWaitFor[0] = m_shutdownEvent.GetEvent();
        handlesToWaitFor[1] = m_acceptConnectionsEvent.GetEvent();

        while (!m_shutdownEvent.Wait(0))
        {
            DWORD waitResult = ::WaitForMultipleObjects(2, handlesToWaitFor, false, INFINITE);

            if (waitResult == WAIT_OBJECT_0)
            {
                // Time to shutdown
                break;
            }
            else if (waitResult == WAIT_OBJECT_0 + 1)
            {
                // accept connections

                while (!m_shutdownEvent.Wait(0) && m_acceptConnectionsEvent.Wait(0))
                {
                    CIOBuffer *pAddress = Allocate();

                    int addressSize = (int)pAddress->GetSize();

                    //lint -e{826} suspicious pointer conversion
                    SOCKET acceptedSocket = ::WSAAccept(
                                                m_listeningSocket,
                                                reinterpret_cast<sockaddr*>(const_cast<BYTE*>(pAddress->GetBuffer())),
                                                &addressSize,
                                                0,
                                                0);

                    pAddress->Use(addressSize);

                    if (acceptedSocket != INVALID_SOCKET)
                    {
                        Socket *pSocket = AllocateSocket(acceptedSocket);

                        //lint -e{1933} call to unqualified virtual function
                        OnConnectionEstablished(pSocket, pAddress);

                        pSocket->Release();
                    }
                    else if (m_acceptConnectionsEvent.Wait(0))
                    {
                        //lint -e{1933} call to unqualified virtual function
                        OnError(_T("CSocketServer::Run() - WSAAccept:") + GetLastErrorMessage(::WSAGetLastError()));
                    }

                    pAddress->Release();
                }
            }
            else
            {
                //lint -e{1933} call to unqualified virtual function
                OnError(_T("CSocketServer::Run() - WaitForMultipleObjects: ") + GetLastErrorMessage(::GetLastError()));
            }
        }

        for (size_t i = 0; i < m_numThreads; ++i)
        {
            workers[i]->InitiateShutdown();
        }

        for (size_t i = 0; i < m_numThreads; ++i)
        {
            workers[i]->WaitForShutdownToComplete();

            delete workers[i];

            workers[i] = 0;
        }
    }
    catch(const CException &e)
    {
        //lint -e{1933} call to unqualified virtual function
        OnError(_T("CSocketServer::Run() - Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage());
    }
    catch(...)
    {
        //lint -e{1933} call to unqualified virtual function
        OnError(_T("CSocketServer::Run() - Unexpected exception"));
    }

    //lint -e{1933} call to unqualified virtual function
    OnShutdownComplete();

    return 0;
}

CSocketServer::Socket *CSocketServer::AllocateSocket(
    SOCKET theSocket)
{
    CCriticalSection::Owner lock(m_listManipulationSection);

    Socket *pSocket = 0;

    if (!m_freeList.Empty())
    {
        pSocket = m_freeList.PopNode();

        pSocket->Attach(theSocket);

        pSocket->AddRef();
    }
    else
    {
        pSocket = new Socket(*this, theSocket, m_useSequenceNumbers);

        //lint -e{1933} call to unqualified virtual function
        OnConnectionCreated();
    }

    m_activeList.PushNode(pSocket);

    //lint -e{611} suspicious cast
    m_iocp.AssociateDevice(reinterpret_cast<HANDLE>(theSocket), (ULONG_PTR)pSocket);

    return pSocket;
}

void CSocketServer::ReleaseSocket(Socket *pSocket)
{
    if (!pSocket)
    {
        throw CException(_T("CSocketServer::ReleaseSocket()"), _T("pSocket is null"));
    }

    CCriticalSection::Owner lock(m_listManipulationSection);

    pSocket->RemoveFromList();

    if (m_maxFreeSockets == 0 ||
            m_freeList.Count() < m_maxFreeSockets)
    {
        m_freeList.PushNode(pSocket);
    }
    else
    {
        DestroySocket(pSocket);
    }
}

void CSocketServer::DestroySocket(
    Socket *pSocket)
{
    delete pSocket;

    //lint -e{1933} call to unqualified virtual function
    OnConnectionDestroyed();
}

void CSocketServer::OnError(
    const _tstring &message)
{
    DEBUG_ONLY(Output(message));
}

void CSocketServer::WriteCompleted(
    Socket * /*pSocket*/,
    CIOBuffer *pBuffer)
{
    if (pBuffer->GetUsed() != pBuffer->GetWSABUF()->len)
    {
        //lint -e{1933} call to unqualified virtual function
        OnError(_T("CSocketServer::WorkerThread::WriteCompleted - Socket write where not all data was written"));
    }

    //lint -e{818} pointer pBuffer could be declared const (but not in derived classes...)
}

void CSocketServer::SetServerDataPtr(
    Socket *pSocket,
    void *pData)
{
    pSocket->SetServerDataPtr(pData);
}

void *CSocketServer::GetServerDataPtr(
    const Socket *pSocket)
{
    return pSocket->GetServerDataPtr();
}

void CSocketServer::WriteCompleted(
    Socket *pSocket)
{
    pSocket->WriteCompleted();
}

bool CSocketServer::FilterReadRequest(
    Socket * /*pSocket*/,
    CIOBuffer ** /*ppBuffer*/)
{
    // Normal processing here is to return a filtered buffer if we can filter in place or false if
    // the filtered data will be returned via a call to PostFilteredReadRequest

    return true;
}

bool CSocketServer::FilterReadCompleted(
    Socket * /*pSocket*/,
    CIOBuffer ** /*ppBuffer*/)
{
    // Normal processing here is to return a filtered buffer if we can filter in place or false if
    // the filtered data will be returned via a call to PostFilteredReadCompleted

    return true;
}

bool CSocketServer::FilterWriteRequest(
    Socket * /*pSocket*/,
    CIOBuffer ** /*ppBuffer*/)
{
    // Normal processing here is to return a filtered buffer if we can filter in place or false if
    // the filtered data will be returned via a call to PostFilteredWriteRequest

    return true;
}

bool CSocketServer::FilterWriteCompleted(
    Socket * /*pSocket*/,
    CIOBuffer ** /*ppBuffer*/)
{
    // Normal processing here is to return a filtered buffer if we can filter in place or false if
    // the filtered data will be returned via a call to PostFilteredWriteCompleted

    return true;
}

bool CSocketServer::FilterSocketShutdown(
    Socket * /*pSocket*/,
    int /*how*/)
{
    return true;
}

bool CSocketServer::FilterSocketClose(
    Socket * /*pSocket*/)
{
    return true;
}

void CSocketServer::PostFilteredReadRequest(
    Socket *pSocket,
    CIOBuffer *pBuffer)
{
    PostIoOperation(pSocket, pBuffer, IO_Filtered_Read_Request);
}

void CSocketServer::PostFilteredReadCompleted(
    Socket *pSocket,
    CIOBuffer *pBuffer)
{
    pBuffer->SetSequenceNumber(pSocket->GetSequenceNumber(Socket::FilteredReadSequenceNo));

    PostIoOperation(pSocket, pBuffer, IO_Filtered_Read_Completed);
}

void CSocketServer::PostFilteredWriteRequest(
    Socket *pSocket,
    CIOBuffer *pBuffer)
{
    if (pSocket->WritePending())
    {
        pBuffer->SetSequenceNumber(pSocket->GetSequenceNumber(Socket::FilteredWriteSequenceNo));

        PostIoOperation(pSocket, pBuffer, IO_Filtered_Write_Request);
    }
}

void CSocketServer::PostFilteredWriteCompleted(
    Socket *pSocket,
    CIOBuffer *pBuffer)
{
    PostIoOperation(pSocket, pBuffer, IO_Filtered_Write_Completed);
}

void CSocketServer::PostIoOperation(
    Socket *pSocket,
    CIOBuffer *pBuffer,
    IO_Operation operation)
{
    pBuffer->SetOperation(operation);
    pBuffer->AddRef();

    pSocket->AddRef();

    m_iocp.PostStatus((ULONG_PTR)pSocket, 0, pBuffer);
}

CSocketServer &CSocketServer::GetServer(
    Socket *pSocket)
{
    return pSocket->m_server;
}

void CSocketServer::OnConnectionError(
    ConnectionErrorSource /*source*/,
    Socket * /*pSocket*/,
    CIOBuffer * /*pBuffer*/,
    DWORD lastError)
{
    DEBUG_ONLY(Output(_T("CSocketServer::OnConnectionError() - : ") + GetLastErrorMessage(lastError)));
}

//////////////////////////////////////////////////////////////////////////////
// CSocketServer::Socket
///////////////////////////////////////////////////////////////////////////////

CSocketServer::Socket::Socket(
    CSocketServer &server,
    SOCKET theSocket,
    bool useSequenceNumbers)
    :  m_server(server),
       m_socket(theSocket),
       m_ref(1),
       m_outstandingWrites(0),
       m_readShutdown(false),
       m_writeShutdown(false),
       m_closing(false),
       m_clientClosed(false),
       m_pSequenceData(0)
{
    if (!IsValid())
    {
        throw CException(_T("CSocketServer::Socket::Socket()"), _T("Invalid socket"));
    }

    if (useSequenceNumbers)
    {
        m_pSequenceData = new SequenceData(m_crit);
    }
}

CSocketServer::Socket::~Socket()
{
    try
    {
        delete m_pSequenceData;
    }
    catch(...)
    {
    }
}

void CSocketServer::Socket::Attach(
    SOCKET theSocket)
{
    if (IsValid())
    {
        throw CException(_T("CSocketServer::Socket::Attach()"), _T("Socket already attached"));
    }

    m_socket = theSocket;

    SetUserData(0);

    m_readShutdown = false;
    m_writeShutdown = false;
    m_outstandingWrites = 0;
    m_closing = false;
    m_clientClosed = false;

    if (m_pSequenceData)
    {
        m_pSequenceData->Reset();
    }

}

void CSocketServer::Socket::AddRef()
{
    ::InterlockedIncrement(&m_ref);
}

void CSocketServer::Socket::Release()
{
    if (0 == ::InterlockedDecrement(&m_ref))
    {
        if (IsValid())
        {
            AddRef();

            if (!m_closing)
            {
                m_closing = true;

                if (!m_server.OnConnectionClosing(this))
                {
                    AbortiveClose();
                }
            }
            else
            {
                AbortiveClose();
            }

            Release();
            return;
        }

        m_server.ReleaseSocket(this);
    }
}

void CSocketServer::Socket::Shutdown(
    int how /* = SD_BOTH */)
{
    DEBUG_ONLY(Output(_T("CSocketServer::Socket::Shutdown() ") + ToString(how)));

    if (m_server.FilterSocketShutdown(this, how))
    {
        if (how == SD_RECEIVE || how == SD_BOTH)
        {
            m_readShutdown = true;
        }

        if (how == SD_SEND || how == SD_BOTH)
        {
            m_writeShutdown = true;
        }

        if (::InterlockedExchange(&m_outstandingWrites, m_outstandingWrites) > 0)
        {
            // Send side will be shut down when last pending write completes...

            if (how == SD_BOTH)
            {
                how = SD_RECEIVE;
            }
            else if (how == SD_SEND)
            {
                return;
            }
        }

        if (IsValid())
        {
            if (0 != ::shutdown(m_socket, how))
            {
                m_server.OnError(_T("CSocketServer::Server::Shutdown() - ") + GetLastErrorMessage(::WSAGetLastError()));
            }

            DEBUG_ONLY(Output(_T("shutdown initiated")));
        }
    }
}

bool CSocketServer::Socket::IsConnected(
    int how /*= SD_BOTH*/) const
{
    if (how == SD_RECEIVE)
    {
        return !m_readShutdown;
    }

    if (how == SD_SEND)
    {
        return !m_writeShutdown;
    }

    if (how == SD_BOTH)
    {
        return (!m_writeShutdown && !m_readShutdown);
    }

    return false;
}

void CSocketServer::Socket::Close()
{
    CCriticalSection::Owner lock(m_crit);

    if (IsValid())
    {
        if (m_server.FilterSocketClose(this))
        {
            InternalClose();
        }
    }
}

bool CSocketServer::Socket::WritePending()
{
    if (m_writeShutdown)
    {
        DEBUG_ONLY(Output(_T("CSocketServer::Socket::WritePending() - Attempt to write after write shutdown")));

        return false;
    }

    ::InterlockedIncrement(&m_outstandingWrites);

    return true;
}

void CSocketServer::Socket::WriteCompleted()
{
    if (::InterlockedDecrement(&m_outstandingWrites) == 0)
    {
        if (m_writeShutdown)
        {
            // The final pending write has been completed so we can now shutdown the send side of the
            // connection.

            Shutdown(SD_SEND);
        }
    }
}

void CSocketServer::Socket::AbortiveClose()
{
    // Force an abortive close.

    LINGER lingerStruct;

    lingerStruct.l_onoff = 1;
    lingerStruct.l_linger = 0;

    if (SOCKET_ERROR == ::setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct)))
    {
        m_server.OnError(_T("CSocketServer::Socket::AbortiveClose() - setsockopt(SO_LINGER) - ")  + GetLastErrorMessage(::WSAGetLastError()));
    }

    InternalClose();
}

void CSocketServer::Socket::OnClientClose()
{
    if (0 == ::InterlockedExchange(&m_clientClosed, 1))
    {
        Shutdown(SD_RECEIVE);

        m_server.OnConnectionClientClose(this);
    }
}

void CSocketServer::Socket::OnConnectionReset()
{
    CCriticalSection::Owner lock(m_crit);

    if (IsValid())
    {
        m_server.OnConnectionReset(this);

        InternalClose();
    }
}

void CSocketServer::Socket::InternalClose()
{
    CCriticalSection::Owner lock(m_crit);

    if (IsValid())
    {
        if (0 != ::closesocket(m_socket))
        {
            m_server.OnError(_T("CSocketServer::Socket::InternalClose() - closesocket - ") + GetLastErrorMessage(::WSAGetLastError()));
        }

        m_socket = INVALID_SOCKET;

        m_readShutdown = true;
        m_writeShutdown = true;

        m_server.OnConnectionClosed(this);
    }
}

bool CSocketServer::Socket::Read(
    CIOBuffer *pBuffer /* = 0 */,
    bool throwOnFailure /* = false*/)
{
    if (!IsValid())
    {
        if (throwOnFailure)
        {
            // Todo throw SocketClosedException();
            throw CException(_T("CSocketServer::Socket::Read()"), _T("Socket is closed"));
        }
        else
        {
            return false;
        }
    }
    // Post a read request to the iocp so that the actual socket read gets performed by
    // one of the server's IO threads...

    if (!pBuffer)
    {
        pBuffer = m_server.Allocate();
    }
    else
    {
        pBuffer->AddRef();
    }

    m_server.PostIoOperation(this, pBuffer, m_server.m_postZeroByteReads ? IO_Zero_Byte_Read_Request : IO_Read_Request);

    pBuffer->Release();

    return true;
}

bool CSocketServer::Socket::Write(
    const char *pData,
    size_t dataLength,
    bool throwOnFailure /* = false*/)
{
    return Write(reinterpret_cast<const BYTE*>(pData), dataLength, throwOnFailure);
}

bool CSocketServer::Socket::Write(
    const BYTE *pData,
    size_t dataLength,
    bool throwOnFailure /* = false*/)
{
    if (!IsValid())
    {
        if (throwOnFailure)
        {
            // Todo throw SocketClosedException();
            throw CException(_T("CSocketServer::Socket::Write()"), _T("Socket is closed"));
        }
        else
        {
            return false;
        }
    }

    if (!WritePending())
    {
        if (throwOnFailure)
        {
            // Todo throw SocketClosedException();
            throw CException(_T("CSocketServer::Socket::Write()"), _T("Socket is shutdown"));
        }
        else
        {
            return false;
        }
    }

    CIOBuffer *pBuffer = m_server.Allocate();

    pBuffer->AddData(pData, dataLength);

    pBuffer->SetSequenceNumber(GetSequenceNumber(WriteSequenceNo));

    m_server.PostIoOperation(this, pBuffer, IO_Write_Request);

    pBuffer->Release();

    return true;
}

bool CSocketServer::Socket::Write(
    CIOBuffer *pBuffer,
    bool throwOnFailure /* = false*/)
{
    if (!IsValid())
    {
        if (throwOnFailure)
        {
            // Todo throw SocketClosedException();
            throw CException(_T("CSocketServer::Socket::Write()"), _T("Socket is closed"));
        }
        else
        {
            return false;
        }
    }

    if (!WritePending())
    {
        if (throwOnFailure)
        {
            // Todo throw SocketClosedException();
            throw CException(_T("CSocketServer::Socket::Write()"), _T("Socket is shutdown"));
        }
        else
        {
            return false;
        }
    }

    pBuffer->SetSequenceNumber(GetSequenceNumber(WriteSequenceNo));

    m_server.PostIoOperation(this, pBuffer, IO_Write_Request);

    return true;
}

long CSocketServer::Socket::GetSequenceNumber(
    SequenceType type)
{
    if (m_pSequenceData)
    {
        return m_pSequenceData->m_numbers[type]++;
    }

    return 0;
}

CIOBuffer *CSocketServer::Socket::GetNextBuffer(
    CIOBuffer *pBuffer /* = 0 */)
{
    if (m_pSequenceData)
    {
        if (pBuffer)
        {
            return m_pSequenceData->m_outOfSequenceWrites.GetNext(pBuffer);
        }
        else
        {
            return m_pSequenceData->m_outOfSequenceWrites.ProcessAndGetNext();
        }
    }

    return pBuffer;
}

bool CSocketServer::Socket::IsValid()
{
    CCriticalSection::Owner lock(m_crit);

    return (INVALID_SOCKET != m_socket);
}

void CSocketServer::Socket::OnConnectionError(
    CSocketServer::ConnectionErrorSource source,
    CIOBuffer *pBuffer,
    DWORD lastError)
{
    if (WSAESHUTDOWN == lastError)
    {
        OnClientClose();
    }
    else if (WSAECONNRESET == lastError || WSAECONNABORTED == lastError)
    {
        OnConnectionReset();
    }
    else if (!IsValid() && WSAENOTSOCK == lastError)
    {
        // Swallow this error as we expect it...
    }
    else
    {
        m_server.OnConnectionError(source, this, pBuffer, lastError);
    }
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::Socket::SequenceData
///////////////////////////////////////////////////////////////////////////////

CSocketServer::Socket::SequenceData::SequenceData(
    CCriticalSection &section)
    :  m_outOfSequenceWrites(section)
{
    memset(m_numbers, 0, sizeof(m_numbers));
}

void CSocketServer::Socket::SequenceData::Reset()
{
    memset(m_numbers, 0, sizeof(m_numbers));

    m_outOfSequenceWrites.Reset();
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::WorkerThread
///////////////////////////////////////////////////////////////////////////////

CSocketServer::WorkerThread::WorkerThread(
    CSocketServer &server,
    CIOCompletionPort &iocp)
    :  m_server(server),
       m_iocp(iocp)
{
    m_server.OnThreadCreated();
}

CSocketServer::WorkerThread::~WorkerThread()
{
    try
    {
        m_server.OnThreadDestroyed();
    }
    catch(...)
    {
    }
}

int CSocketServer::WorkerThread::Run()
{
    try
    {
        //lint -e{716} while(1)
        while (true)
        {
            // continually loop to service io completion packets

            DWORD dwIoSize = 0;
            Socket *pSocket = 0;
            CIOBuffer *pBuffer = 0;

            bool weClosedSocket = false;

            try
            {
                m_iocp.GetStatus((PDWORD_PTR)&pSocket, &dwIoSize, (OVERLAPPED**)&pBuffer);
            }
            catch (const CWin32Exception &e)
            {
                if (e.GetError() == ERROR_NETNAME_DELETED)
                {
                    weClosedSocket = true;
                }
                else if (e.GetError() != WSA_OPERATION_ABORTED)
                {
                    throw;
                }

                DEBUG_ONLY(Output(_T("IOCP error - client connection dropped")));
            }

            if (!pSocket)
            {
                // A completion key of 0 is posted to the iocp to request us to shut down...

                break;
            }

            m_server.OnThreadBeginProcessing();

            //lint -e{1933} call to unqualified virtual function
            HandleOperation(pSocket, pBuffer, dwIoSize, weClosedSocket);

            m_server.OnThreadEndProcessing();
        }
    }
    catch(const CException &e)
    {
        m_server.OnError(_T("CSocketServer::WorkerThread::Run() - Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage());
    }
    catch(...)
    {
        m_server.OnError(_T("CSocketServer::WorkerThread::Run() - Unexpected exception"));
    }

    return 0;
}

void CSocketServer::WorkerThread::InitiateShutdown()
{
    m_iocp.PostStatus(0);
}

void CSocketServer::WorkerThread::WaitForShutdownToComplete()
{
    // if we havent already started a shut down, do so...

    InitiateShutdown();

    Wait();
}

void CSocketServer::WorkerThread::HandleOperation(
    Socket *pSocket,
    CIOBuffer *pBuffer,
    DWORD dwIoSize,
    bool weClosedSocket)
{
    if (pBuffer)
    {
        const IO_Operation operation = static_cast<IO_Operation>(pBuffer->GetOperation());

        switch (operation)
        {
        case IO_Zero_Byte_Read_Request :

            DEBUG_ONLY(Output(_T("ZeroByteRead:") + ToString(pBuffer)));

            ZeroByteRead(pSocket, pBuffer);

            pSocket->Release();
            pBuffer->Release();

            break ;

        case IO_Zero_Byte_Read_Completed :
        case IO_Read_Request :

            DEBUG_ONLY(Output(_T("IO_Zero_Byte_Read_Completed|IO_Read_Request:") + ToString(pBuffer)));

            if (m_server.FilterReadRequest(pSocket, &pBuffer))
            {
                Read(pSocket, pBuffer);
            }

            pSocket->Release();
            pBuffer->Release();

            break;

        case IO_Filtered_Read_Request :

            DEBUG_ONLY(Output(_T("IO_Filtered_Read_Request:") + ToString(pBuffer)));

            Read(pSocket, pBuffer);

            pSocket->Release();
            pBuffer->Release();

            break;

        case IO_Read_Completed :

            DEBUG_ONLY(Output(_T("IO_Read_Completed:") + ToString(pBuffer)));

            pBuffer->Use(dwIoSize);

            DEBUG_ONLY(Output(_T("  RX: ") + ToString(pBuffer->GetUsed()) + _T(" bytes") + _T(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" : ") + ToString(pBuffer) + _T("\n")));
            //DEBUG_ONLY(Output(_T("  RX: ") + ToString(pBuffer->GetUsed()) + _T(" bytes") + _T(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" : ") + ToString(pBuffer) + _T("\n") + DumpData(reinterpret_cast<const BYTE*>(pBuffer->GetWSABUF()->buf), dwIoSize, 40)));

            if (m_server.FilterReadCompleted(pSocket, &pBuffer))
            {
                if (0 != dwIoSize)
                {
                    m_server.ReadCompleted(pSocket, pBuffer);
                }
            }

            if (0 == dwIoSize && !weClosedSocket)
            {
                // client connection dropped?

                DEBUG_ONLY(Output(_T("ReadCompleted - 0 bytes - client connection dropped")));

                pSocket->OnClientClose();
            }

            pSocket->Release();
            pBuffer->Release();

            break;

        case IO_Filtered_Read_Completed :

            DEBUG_ONLY(Output(_T("IO_Filtered_Read_Completed:") + ToString(pBuffer)));

            //DEBUG_ONLY(Output(_T("F RX: ") + ToString(pBuffer->GetUsed()) + _T(" bytes") + _T(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" : ") + ToString(pBuffer) + _T("\n") + DumpData(reinterpret_cast<const BYTE*>(pBuffer->GetBuffer()), pBuffer->GetUsed(), 40)));

            if (0 != pBuffer->GetUsed())     // TODO post the correct io size
            {
                m_server.ReadCompleted(pSocket, pBuffer);
            }

            pSocket->Release();
            pBuffer->Release();

            break;

        case IO_Write_Request :

            DEBUG_ONLY(Output(_T("IO_Write_Request:") + ToString(pBuffer)));

            DEBUG_ONLY(Output(_T("  TX: ") + ToString(pBuffer->GetUsed()) + _T(" bytes") + _T(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" : ") + ToString(pBuffer) + _T("\n")));
            //DEBUG_ONLY(Output(_T("  TX: ") + ToString(pBuffer->GetUsed()) + _T(" bytes") + _T(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" : ") + ToString(pBuffer) + _T("\n") + DumpData(reinterpret_cast<const BYTE*>(pBuffer->GetBuffer()), pBuffer->GetUsed(), 40)));

            if (m_server.FilterWriteRequest(pSocket, &pBuffer))
            {
                Write(pSocket, pBuffer);
            }

            pSocket->Release();
            pBuffer->Release();

            break;

        case IO_Filtered_Write_Request :

            DEBUG_ONLY(Output(_T("IO_Filtered_Write_Request:") + ToString(pBuffer)));

            //DEBUG_ONLY(Output(_T("F TX: ") + ToString(pBuffer->GetUsed()) + _T(" bytes") + _T(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" : ") + ToString(pBuffer) + _T("\n") + DumpData(reinterpret_cast<const BYTE*>(pBuffer->GetBuffer()), pBuffer->GetUsed(), 40)));

            Write(pSocket, pBuffer);

            pSocket->Release();
            pBuffer->Release();

            break;

        case IO_Write_Completed :

            DEBUG_ONLY(Output(_T("IO_Write_Completed:") + ToString(pBuffer)));

            pBuffer->Use(dwIoSize);

            if (m_server.FilterWriteCompleted(pSocket, &pBuffer))
            {
                m_server.WriteCompleted(pSocket, pBuffer);

                pSocket->WriteCompleted();
            }

            pSocket->Release();
            pBuffer->Release();

            break;

        case IO_Filtered_Write_Completed :

            m_server.WriteCompleted(pSocket, pBuffer);

            pSocket->WriteCompleted();

            pSocket->Release();
            pBuffer->Release();

            break;

        default :
            m_server.OnError(_T("CSocketServer::WorkerThread::Run() - Unexpected operation"));
            break;
        }
    }
    else
    {
        m_server.OnError(_T("CSocketServer::WorkerThread::Run() - Unexpected - pBuffer is 0"));
    }
}

void CSocketServer::WorkerThread::ZeroByteRead(
    Socket *pSocket,
    CIOBuffer *pBuffer) const
{
    pSocket->AddRef();

    pBuffer->SetOperation(IO_Zero_Byte_Read_Completed);
    pBuffer->SetupZeroByteRead();
    pBuffer->AddRef();

    DWORD dwNumBytes = 0;
    DWORD dwFlags = 0;

    if (SOCKET_ERROR == ::WSARecv(
                pSocket->m_socket,
                pBuffer->GetWSABUF(),
                1,
                &dwNumBytes,
                &dwFlags,
                pBuffer,
                NULL))
    {
        DWORD lastError = ::WSAGetLastError();

        if (ERROR_IO_PENDING != lastError)
        {
            pSocket->OnConnectionError(ZeroByteReadError, pBuffer, lastError);

            pSocket->Release();
            pBuffer->Release();
        }
    }
}

void CSocketServer::WorkerThread::Read(
    Socket *pSocket,
    CIOBuffer *pBuffer) const
{
    pSocket->AddRef();

    pBuffer->SetOperation(IO_Read_Completed);
    pBuffer->SetupRead();
    pBuffer->AddRef();

    CCriticalSection::Owner lock(pSocket->m_crit);

    pBuffer->SetSequenceNumber(pSocket->GetSequenceNumber(Socket::ReadSequenceNo));

    DWORD dwNumBytes = 0;
    DWORD dwFlags = 0;

    if (SOCKET_ERROR == ::WSARecv(
                pSocket->m_socket,
                pBuffer->GetWSABUF(),
                1,
                &dwNumBytes,
                &dwFlags,
                pBuffer,
                NULL))
    {
        DWORD lastError = ::WSAGetLastError();

        if (ERROR_IO_PENDING != lastError)
        {
            pSocket->OnConnectionError(ReadError, pBuffer, lastError);

            pSocket->Release();
            pBuffer->Release();
        }
    }
}

void CSocketServer::WorkerThread::Write(
    Socket *pSocket,
    CIOBuffer *pBuffer) const
{
    pSocket->AddRef();

    pBuffer->SetOperation(IO_Write_Completed);
    pBuffer->SetupWrite();
    pBuffer->AddRef();

    pBuffer = pSocket->GetNextBuffer(pBuffer);

    while(pBuffer)
    {
        DEBUG_ONLY(Output(ToString(pSocket) + _T(" X TX: Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" bytes = ") + ToString(pBuffer->GetWSABUF()->len)));

        DWORD dwFlags = 0;
        DWORD dwSendNumBytes = 0;

        if (SOCKET_ERROR == ::WSASend(
                    pSocket->m_socket,
                    pBuffer->GetWSABUF(),
                    1,
                    &dwSendNumBytes,
                    dwFlags,
                    pBuffer,
                    NULL))
        {
            DWORD lastError = ::WSAGetLastError();

            if (ERROR_IO_PENDING != lastError)
            {
                pSocket->OnConnectionError(WriteError, pBuffer, lastError);

                pSocket->WriteCompleted();  // this pending write will never complete...

                pSocket->Release();
                pBuffer->Release();
            }
        }

        pBuffer = pSocket->GetNextBuffer();
    }
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::FilteringHelper
///////////////////////////////////////////////////////////////////////////////

void CSocketServer::FilteringHelper::PostFilteredReadRequest(
    Socket *pSocket,
    CIOBuffer *pBuffer)
{
    CSocketServer::GetServer(pSocket).PostFilteredReadRequest(pSocket, pBuffer);
}

void CSocketServer::FilteringHelper::PostFilteredReadCompleted(
    Socket *pSocket,
    CIOBuffer *pBuffer)
{
    CSocketServer::GetServer(pSocket).PostFilteredReadCompleted(pSocket, pBuffer);
}

void CSocketServer::FilteringHelper::PostFilteredWriteRequest(
    Socket *pSocket,
    CIOBuffer *pBuffer)
{
    CSocketServer::GetServer(pSocket).PostFilteredWriteRequest(pSocket, pBuffer);
}

void CSocketServer::FilteringHelper::PostFilteredWriteCompleted(
    Socket *pSocket,
    CIOBuffer *pBuffer)
{
    CSocketServer::GetServer(pSocket).PostFilteredWriteCompleted(pSocket, pBuffer);
}

void CSocketServer::FilteringHelper::WriteCompleted(
    Socket *pSocket)
{
    CSocketServer::WriteCompleted(pSocket);
}

///////////////////////////////////////////////////////////////////////////////
// Static helper methods
///////////////////////////////////////////////////////////////////////////////

static size_t CalculateNumberOfThreads(size_t numThreads)
{
    if (numThreads == 0)
    {
        CSystemInfo systemInfo;

        numThreads = systemInfo.dwNumberOfProcessors * 2;
    }

    return numThreads;
}

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

///////////////////////////////////////////////////////////////////////////////
// End of file...
///////////////////////////////////////////////////////////////////////////////
