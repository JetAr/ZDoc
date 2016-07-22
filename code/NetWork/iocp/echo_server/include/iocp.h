#ifndef ___IOCP_H_INCLUDED___
#define ___IOCP_H_INCLUDED___

#include <windows.h>
#include "INCLUDE/socket_client.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// A template class (wrapper) to handle basic operations 
// related to IO Completion Ports (IOCP).

template<class T>
class IOCPSimple {
private:
	HANDLE	m_hIocp;
	DWORD	m_dwTimeout;

protected:
	// Useful method to queue a socket (handler in fact) in the IOCP's
	// queue, without requesting any 'physical' I/O operations. 
	BOOL PostQueuedCompletionStatus( ClientSocket<T> *sock, MYOVERLAPPED *pMov ) {
		return ::PostQueuedCompletionStatus( this->m_hIocp, 1, 
			(DWORD) sock, (OVERLAPPED*) pMov );
	};

public:
	~IOCPSimple() {
		// Destroy the IOCP handler 
		::CloseHandle( this->m_hIocp );
	};

	IOCPSimple( DWORD dwTimeout = 0 ) {
		// This will be used as a timeout value to wait
		// for completion events from IOCP (in milliseconds).
		// See 'GetQueuedCompletionStatus(...)' for more 
		// details. If is set to 0 == means infinite.
		m_dwTimeout = dwTimeout;

		// Create the IOCP handler 
		this->m_hIocp = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
		if ( this->m_hIocp == NULL ) {
			throw "CreateIoCompletionPort() failed to create IO Completion port.";
		}
	};

	// Method associates Socket handler with the handler
	// of the IOCP. It is important to do this association
	// first of all, once Socket is accepted and is valid.
	// In fact, this method registers socket with the IOCP, 
	// so IOCP will monitor every I/O operation happening 
	// with the socket.
	void AssociateSocket( ClientSocket<T> *sock ) {
		if ( sock != NULL ) {
			// As a key of this association, the pointer to the 
			// "sock" is set. The pointer to the "sock" will be 
			// returned with "GetQueuedCompletionStatus" call
			// whenever a I/O operation completes, so we always know
			// which is the relevant socket.
			::CreateIoCompletionPort( (HANDLE) sock->GetSocket(), 
				this->m_hIocp, (DWORD) sock, 0 );
		}
	};

	// Queue in the IOCP's queue with the status 'pending'.
	// This is a fictive status (not real). Method is useful in
	// cases when packets/data buffers to send via socket are yet
	// not ready (e.g. due some internal, time consuming, calculations) 
	// and rather than keeping socket somewhere, in separate structures, 
	// place it in the queue with status pending. It will be returned
	// shortly with the "GetQueuedCompletionStatus" call anyway.
	BOOL SetPendingMode( ClientSocket<T> *sock ) {
		BOOL ret = false;

		if ( sock != NULL ) {
			// Get a free instance of the structure
			// from the pool.
			MYOVERLAPPED *mov = Overlapped::Get();

			if ( mov != NULL ) {
				mov->OperationType = _PENDING;
				mov->nSession = sock->GetSession();
				ret = PostQueuedCompletionStatus( sock, mov );
				if ( !ret ) Overlapped::Release( mov );
			}
		}

		return ret;
	};

	// Queue in the IOCP's queue with the status 'close'.
	// This is a fictive status (not real). Useful only 
	// when close socket cases should be treated/handled 
	// within the routine, handling I/O completion statuses, 
	// and just to not spread the code across different 
	// modules/classes.
	BOOL SetCloseMode( ClientSocket<T> *sock, MYOVERLAPPED *pMov = NULL ) {
		BOOL ret = false;

		if ( sock != NULL ) {
			// Get a free instance of the structure
			// from the pool.
			MYOVERLAPPED *mov = Overlapped::Get();

			if ( mov != NULL ) {
				mov->OperationType = _CLOSE;
				mov->nSession = sock->GetSession();

				if (pMov != NULL) {
					mov->DataBuf.buf = pMov->DataBuf.buf;
					mov->DataBuf.len = pMov->DataBuf.len;
				}

				ret = PostQueuedCompletionStatus( sock, mov );
				if ( !ret ) Overlapped::Release( mov );
			}
		}

		return ret;
	};

	// Queue in the IOCP's queue with the status 'accept'.
	// This is a fictive status (not real). When a new
	// client connection is accepted by the server socket,
	// method is used to acknowledge the routine, handling 
	// I/O completion statuses, of this. Same reason as per 
	// 'close' status, just to have a unique piece of code 
	// handling all the possible statuses.
	BOOL SetAcceptMode( ClientSocket<T> *sock ) {
		BOOL ret = false;

		if ( sock != NULL ) {
			// Get a free instance of the structure
			// from the pool.
			MYOVERLAPPED *mov = Overlapped::Get();

			if ( mov != NULL ) {
				mov->OperationType = _ACCEPT;
				mov->nSession = sock->GetSession();
				ret = PostQueuedCompletionStatus( sock, mov );
				if ( !ret ) Overlapped::Release( mov );
			}
		}

		return ret;
	};

	// This method is just a wrapper. For more details, 
	// see MSDN for "GetQueuedCompletionStatus". The only
	// difference is, method is adjusted to handle 
	// 'ClientSocket<T>' (as registered with 'AssociateSocket(...)')
	// rather than pointer to the 'DWORD'.
	BOOL GetQueuedCompletionStatus( LPDWORD pdwBytesTransferred,
		ClientSocket<T> **sock, MYOVERLAPPED **lpOverlapped ) {
		return ::GetQueuedCompletionStatus( this->m_hIocp, pdwBytesTransferred, 
			(LPDWORD) sock, (LPOVERLAPPED*) lpOverlapped, m_dwTimeout );
	};
};

#endif