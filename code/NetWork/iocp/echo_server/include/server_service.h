#ifndef ___SERVER_SERVICE_H_INCLUDED___
#define ___SERVER_SERVICE_H_INCLUDED___

#include "INCLUDE/log.h"
#include "INCLUDE/threading.h"
#include "INCLUDE/socket_server.h"
#include "INCLUDE/iocp.h"
#include <vector>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// A template class designed for verifying client sockets
// against time-out cases. Time-out case = if no I/O actions
// happen with a socket during a configured number of
// seconds. This implies that the attachment of the
// client socket must implement "GetTimeElapsed()" and "ResetTime(...)"
// methods.
template<class T>
class CTimeOutChecker: public IRunnable
{
private:
    unsigned int				m_nTimeOutValue;
    vector<ClientSocket<T> *>	*m_arrSocketPool;
    IOCPSimple<T>				*m_hIocp;

protected:
    // method checks sockets to detect time-out cases
    virtual void run()
    {
        ClientSocket<T>* pSocket = NULL;
        unsigned int i = 0;
        long lngTimeElapsed;

        Log::LogMessage( L"Time out checker service started.\n" );

        vector<ClientSocket<T> *>::iterator itPos = m_arrSocketPool->begin();

        while ( !CThread::currentThread().isInterrupted() )
        {

            if ( itPos >= m_arrSocketPool->end() )
            {
                itPos = m_arrSocketPool->begin();
                i = 0;
            }

            pSocket = *itPos;
            pSocket->Lock();

            // check if client socket object is
            // assigned a socket handler.
            if ( pSocket->IsBusy() )
            {
                // call "GetTimeElapsed()" of the socket's
                // attachment.
                lngTimeElapsed = pSocket->GetAttachment()->GetTimeElapsed();

                // check elapsed time, since last registered action,
                // with the configured time-out.
                if ( lngTimeElapsed > (int)this->m_nTimeOutValue )
                {
                    // clear the time and push the socket to
                    // the IOCP with status _CLOSE. Socket
                    // will be closed shortly.
                    pSocket->GetAttachment()->ResetTime( true );
                    m_hIocp->SetCloseMode( pSocket );
                }
            }
            pSocket->UnLock();
            // sleep 2 milliseconds after every 2 iterations
            if ( ( i % 2 ) == 0 ) ::Sleep( 2 );

            itPos++;
            i++;
        }

        Log::LogMessage( L"Time out checker service stopped.\n" );
    };

public:
    // Constructor of the template class. Requires:
    // - a pointer to a collection of pointers to client sockets.
    //   This is the collection of sockets to be checked against
    //   time-out cases.
    // - a pointer to the IO completion port object responsible
    //   for checking I/O events against passed collection of
    //   client sockets.
    // - value of the time-out, in seconds.
    CTimeOutChecker( vector<ClientSocket<T> *> *arrSocketPool,
                     IOCPSimple<T> *hIocp, unsigned int nTimeOutValue )
    {
        m_arrSocketPool = arrSocketPool;
        m_hIocp = hIocp;

        // If time-out is less than 5 seconds, drop it to 5 seconds.
        // It is expected that configured time-out value to be >= 5!
        if ( nTimeOutValue > 5 ) this->m_nTimeOutValue = nTimeOutValue;
        else this->m_nTimeOutValue = 5;
    };

    // Nothing to destruct as inside the template class
    // we keep/use just pointers obtained from external
    // sources.
    ~CTimeOutChecker() {};
};

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// A template interface showing how the client socket event
// handler should look like.
template<class T>
class ISockEvent
{
public:
    // Client socket ("pSocket") is about to be closed.
    virtual void OnClose( ClientSocket<T> *pSocket,
                          MYOVERLAPPED *pOverlap,
                          ServerSocket<T> *pServerSocket,
                          IOCPSimple<T> *pHIocp
                        ) = 0;

    // Client socket ("pSocket") was just accepted by
    // the server socket (new connection with a client
    // is created).
    virtual void OnAccept( ClientSocket<T> *pSocket,
                           MYOVERLAPPED *pOverlap,
                           ServerSocket<T> *pServerSocket,
                           IOCPSimple<T> *pHIocp
                         ) = 0;

    // Client socket ("pSocket") was returned from the IOCP
    // queue with status _PENDING. For more details see
    // "IOCPSimple<T>::SetPendingMode(...)". This method
    // is invoked only if there was a call to
    // "IOCPSimple<T>::SetPendingMode(...)", performed by a
    // user code, internally "SetPendingMode(...)"
    // is never called.
    virtual void OnPending( ClientSocket<T> *pSocket,
                            MYOVERLAPPED *pOverlap,
                            ServerSocket<T> *pServerSocket,
                            IOCPSimple<T> *pHIocp
                          ) = 0;

    // Client socket ("pSocket") was returned from IOCP
    // queue with status _READ. This means that overlapped
    // reading operation, started previously with
    // "ClientSocket<T>::ReadFromSocket(...)", was
    // successfully finished.
    // - "pOverlap->DataBuf" structure points to the data
    //   buffer and buffer's size that were passed to the
    //   "ClientSocket<T>::ReadFromSocket(...)".
    // - "dwBytesTransferred" will indicate how many
    //   bytes were read.
    virtual void OnReadFinalized( ClientSocket<T> *pSocket,
                                  MYOVERLAPPED *pOverlap,
                                  DWORD dwBytesTransferred,
                                  ServerSocket<T> *pServerSocket,
                                  IOCPSimple<T> *pHIocp
                                ) = 0;

    // Client socket ("pSocket") was returned from IOCP
    // queue with status _WRITE. This means that overlapped
    // writting operation, started previously with
    // "ClientSocket<T>::WriteToSocket(...)", was
    // successfully finished.
    // - "pOverlap->DataBuf" structure points to the data
    //   buffer and buffer's size that were passed to the
    //   "ClientSocket<T>::WriteToSocket(...)".
    // - "dwBytesTransferred" will indicate how many
    //   bytes were written.
    virtual void OnWriteFinalized( ClientSocket<T> *pSocket,
                                   MYOVERLAPPED *pOverlap,
                                   DWORD dwBytesTransferred,
                                   ServerSocket<T> *pServerSocket,
                                   IOCPSimple<T> *pHIocp
                                 ) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// Class is used internally by "ServerService<T>" as a
// Task entity to be submitted to "ServerService<T>::m_ThPool".
template<class T>
class CSockEventTask: public IRunnable
{
private:
    MYOVERLAPPED	*m_pOverlap;
    ClientSocket<T>	*m_pSocket;
    ISockEvent<T>	*m_pSEvent;
    ServerSocket<T>	*m_pServerSocket;
    IOCPSimple<T>	*m_pHIocp;
    DWORD			m_dwBytesTransferred;
    QueuedBlocks<CSockEventTask<T> > *m_pSockEventTaskPool;

protected:
    virtual void run()
    {
        if ( m_pSEvent == NULL ) return;

        m_pSocket->Lock();

        // check if client socket object is assigned a socket
        // handler and session assigned to operation is equal
        // to the session of the socket.
        if ( m_pSocket->IsBusy() && ( m_pOverlap->nSession == m_pSocket->GetSession() ) )
        {
            switch ( m_pOverlap->OperationType )
            {
            // invoke the relevant event handler
            case _CLOSE:
                Log::LogMessage( L"CloseSocket request %d.\n", m_pSocket->GetSocket() );
                m_pSEvent->OnClose( m_pSocket, m_pOverlap, m_pServerSocket, m_pHIocp );
                // Make sure (double check) the socket
                // is closed.
                m_pServerSocket->Release( m_pSocket );
                break;

            case _ACCEPT:
                m_pSEvent->OnAccept( m_pSocket, m_pOverlap, m_pServerSocket, m_pHIocp );
                break;

            case _READ:
                m_pSEvent->OnReadFinalized( m_pSocket, m_pOverlap, m_dwBytesTransferred,
                                            m_pServerSocket, m_pHIocp );
                break;

            case _PENDING:
                m_pSEvent->OnPending( m_pSocket, m_pOverlap, m_pServerSocket, m_pHIocp );
                break;

            case _WRITE:
                m_pSEvent->OnWriteFinalized( m_pSocket, m_pOverlap, m_dwBytesTransferred,
                                             m_pServerSocket, m_pHIocp );
                break;

            default:
                Log::LogMessage( L"Oops...\n" );

            }
        }

        m_pSocket->UnLock();

        // Place the structure back to the pool.
        Overlapped::Release( m_pOverlap );
        m_pSockEventTaskPool->Release( this );
    };

public:
    CSockEventTask()
    {
        Clear();
    };

    // Set everything required to the Task to be
    // able to invoke the relevant event handler.
    void Set( QueuedBlocks<CSockEventTask<T> > *pSockEventTaskPool,
              ISockEvent<T> *pSEvent,
              ClientSocket<T> *pSocket,
              MYOVERLAPPED *pOverlap,
              DWORD dwBytesTransferred,
              ServerSocket<T> *pServerSocket,
              IOCPSimple<T> *pHIocp
            )
    {
        m_pSockEventTaskPool = pSockEventTaskPool;
        m_pSEvent = pSEvent;
        m_pOverlap = pOverlap;
        m_dwBytesTransferred = dwBytesTransferred;
        m_pSocket = pSocket;
        m_pServerSocket = pServerSocket;
        m_pHIocp = pHIocp;
    };

    // Method is used by 'QueuedBlocks' from
    // "mem_manager.h".
    void Clear()
    {
        m_dwBytesTransferred = 0;
        m_pSockEventTaskPool = NULL;
        m_pSEvent = NULL;
        m_pOverlap = NULL;
        m_pSocket = NULL;
        m_pServerSocket = NULL;
        m_pHIocp = NULL;
    };
};

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// Well, finally, here is the template class joining all
// the stuff together. Considering the Aspect Oriented paradigm,
// this template class may be seen as an Aspect. The "individualizations"
// of this aspect are "ISockEvent<T>" and "T" itself. "T" is nothing
// else but attachment of the client socket (see ClientSocket<T> template
// class for more details). Implementing "ISockEvent<T>" and "T" will
// define the individual behaviour of this aspect.
// It is a composition of the IOCP, server socket, time-out checker
// and thread pool. Class implements the business logic that makes all
// these entities working together.
template<class T>
class ServerService: public IRunnable
{
private:
    ServerSocket<T>	m_ServerSocket;
    IOCPSimple<T>	m_hIocp;
    ISockEvent<T>	*m_pSEvent;

    CTimeOutChecker<T>	*m_TChecker;

    // thread pool which will execute the tasks
    CSimpleThreadPool	*m_ThPool;

    // a pool of the CSockEventTask<T> objects
    QueuedBlocks<CSockEventTask<T> > m_SockEventTaskPool;

protected:
    // This method will be executed by a thread
    // of the task pool.
    virtual void run()
    {
        int nRet;
        DWORD dwBytesTransferred;

        MYOVERLAPPED *pOverlap = NULL;
        ClientSocket<T> *pSocket = NULL;

        Log::LogMessage( L"Socket service thread started.\n" );

        while ( !CThread::currentThread().isInterrupted() )
        {
            ::Sleep( 10 );

            // check for an incoming connection
            pSocket = m_ServerSocket.Accept();
            if ( pSocket != NULL )
            {
                Log::LogMessage( L"Socket created at %d.\n", pSocket->GetSocket() );

                // associate socket with IOCP
                m_hIocp.AssociateSocket( pSocket );
                pSocket->GetAttachment()->ResetTime( false );
                // push the socket to the IOCP queue with
                // status _ACCEPT
                if ( !( m_hIocp.SetAcceptMode( pSocket ) ) )
                {
                    m_ServerSocket.Release( pSocket );
                }
            }

            dwBytesTransferred = 0;
            pSocket = NULL;
            // check the IOCP queue for:
            // - a completed I/O operation against a socket or
            // - a queued socket with a fake status like _ACCEPT,
            //   _CLOSE or _PENDING
            //
            // When an overlapped I/O completes, an I/O completion packet
            // arrives at the IOCP and GetQueuedCompletionStatus returns.
            nRet = m_hIocp.GetQueuedCompletionStatus( &dwBytesTransferred,
                    &pSocket, &pOverlap );

            // something is wrong
            if ( nRet == 0 )
            {
                if ( ::GetLastError() == WAIT_TIMEOUT ) continue;

                if ( pOverlap != NULL ) dwBytesTransferred = 0;
                else continue;
            }

            if ( pSocket == NULL )
            {
                Overlapped::Release( pOverlap );
                continue;
            }

            // not good if zero bytes were transferred
            if ( dwBytesTransferred == 0 )
            {
                // clear the time and push the socket to
                // the IOCP with status _CLOSE. Socket
                // will be closed shortly.
                if ( pSocket->IsBusy() && ( pOverlap->nSession == pSocket->GetSession() ) )
                {
                    pSocket->GetAttachment()->ResetTime( true );
                    if ( !( m_hIocp.SetCloseMode( pSocket, pOverlap ) ) )
                    {
                        m_ServerSocket.Release( pSocket );
                    }
                }

                Overlapped::Release( pOverlap );
            }
            else
            {
                // obtain a free instance of the event task
                // which will be submitted to the threat pool
                CSockEventTask<T> *pCSockEv = m_SockEventTaskPool.Get();
                if ( pCSockEv != NULL )
                {
                    pCSockEv->Set( &m_SockEventTaskPool, m_pSEvent,
                                   pSocket, pOverlap, dwBytesTransferred,
                                   &m_ServerSocket, &m_hIocp
                                 );

                    // _CLOSE operation type will have a lower
                    // priority that other operation types.
                    int priority = ( pOverlap->OperationType == _CLOSE )?1:2;

                    // submit the task to the thread pool
                    m_ThPool->submit( pCSockEv, priority );
                }
                else
                {
                    // no free resources, so close the socket
                    m_ServerSocket.Release( pSocket );
                    Overlapped::Release( pOverlap );
                }
            }
        }

        Log::LogMessage( L"Socket service thread stopped.\n" );
    };

public:
    // Constructor or the class.
    // pSEvent      - pointer to an instance implementing
    //                ISockEvent<T>. This instance will be used
    //                as a client socket event handler.
    // nPort        - port number to bind server socket to.
    // nMaxClients  - the maximum number of accepted (concurrent)
    //                client connections. To be passed to
    //                the server socket and also will be used
    //                as the initial size for the pool of the
    //                CSockEventTask<T> objects.
    // nNoThreads   - indicative (the real one is computed,
    //                see below) number of the threads
    //                to be created by the thread pool.
    // timeout      - the value of the time-out, in seconds.
    //                Will be passed to the time-out checker.
    //                If time-out is zero, time-out checker
    //                will not be created.
    // blnBindLocal - see ServerSocket<T> for more details.
    //                If "true" then server socket is bind
    //                to localhost ("127.0.0.1").
    //                If "false" then server socket is bind
    //                to INADDR_ANY ("0.0.0.0").
    ServerService( ISockEvent<T> *pSEvent, unsigned int nPort, unsigned int nMaxClients,
                   unsigned int nNoThreads, unsigned int timeout,
                   bool blnBindLocal = true ):
        m_ServerSocket( nPort, nMaxClients, true, blnBindLocal ),
        m_hIocp( 200 ), m_SockEventTaskPool( nMaxClients )
    {

        unsigned int i = 0;
        SYSTEM_INFO si;

        Log::LogMessage( L"Creating \"socket_listener\" ... " );

        if ( nMaxClients < 1 )
        {
            throw "illegal value for \"max_connections\" supplied (should be > 0).";
        }

        if ( nNoThreads < 1 )
        {
            throw "illegal value for \"working_threads\" supplied (should be > 0).";
        }

        if ( pSEvent == NULL )
        {
            throw "NULL pointer set for socket event handler.";
        }

        m_pSEvent = pSEvent;

        // Query system info in order to check
        // how many CPU's are there installed in
        // the system.
        ::GetSystemInfo( &si );

        // Compute the real number of the threads
        // to be created by the thread pool.
        i = nNoThreads + si.dwNumberOfProcessors;

        // If configured time-out is zero, then don't
        // create time-out checker object. Client sockets
        // will not be checked against time-out cases.
        if ( timeout > 0 )
        {
            m_TChecker = new CTimeOutChecker<T>( m_ServerSocket.GetPool(), &m_hIocp, timeout );

            // Increase the number of the threads,
            // +1 for the time-out checker.
            i++;
        }
        else m_TChecker = NULL;

        // Create the thread pool which will handle
        // the tasks. "nMaxClients" is used as the
        // initial capacity of the priority queue
        // of (associated with) the thread pool.
        m_ThPool = new CSimpleThreadPool( i, 2 * nMaxClients );

        // Submit this instance (as a task) to the
        // thread pool, as many times as CPU's are installed
        // in the system. The "run()" method will be executed
        // by pool's threads.
        for (i = 0; i < si.dwNumberOfProcessors; i++) m_ThPool->submit( this, 1 );

        // If time-out checker was created, then
        // submit it to the thread pool (as a task).
        if ( m_TChecker != NULL ) m_ThPool->submit( m_TChecker );

        Log::LogMessage( L"DONE\n" );
    };

    virtual ~ServerService()
    {
        // Stop all the pool's threads.
        m_ThPool->shutdown();

        // If time-out checker was created, then
        // delete it.
        if (m_TChecker != NULL) delete m_TChecker;

        // Delete the thread pool.
        delete m_ThPool;
        Log::LogMessage( L"Killing \"socket_listener\" ... DONE\n" );
    };

    // Start the threat pool (== all the
    // threads in the pool).
    void start()
    {
        m_ThPool->startAll();
    };
};

#endif
