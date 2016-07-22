#ifndef ___SOCKET_SERVER_H_INCLUDED___
#define ___SOCKET_SERVER_H_INCLUDED___

#include <winsock2.h>
//z #include <Ws2tcpip.h>
#include "INCLUDE/mem_manager.h"
#include "INCLUDE/socket_client.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// A class for handling basic server socket operations.
template<class T>
class ServerSocket
{
private:
    // Server socket.
    SOCKET	m_ServSock;

    // Pool of client sockets.
    QueuedBlocks<ClientSocket<T> > m_SockPool;

protected:
public:
    // Set the size of the socket pool to the maximum number of accepted
    // client connections.
    ServerSocket(unsigned int nPort, unsigned int nMaxClients,
                 bool blnCreateAsync = false, bool blnBindLocal = true): m_SockPool(nMaxClients)
    {

        int nRet;
        unsigned long lngMode = 1;
        struct sockaddr_in sAddrIn;

        // Create the server socket, set the necessary
        // parameters for making it IOCP compatible.
        this->m_ServSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (this->m_ServSock <= 0)
        {
            throw "server socket creation failed.";
        }

        // Make server socket Synchronous (blocking)
        // or Asynchronous (non-blocking)
        if (blnCreateAsync)
        {
            nRet = ioctlsocket(this->m_ServSock, FIONBIO, (u_long FAR*) &lngMode);
            if (nRet < 0)
            {
                closesocket(this->m_ServSock);
                throw "server socket creation failed.";
            }
        }

        // Fill the structure for binding operation
        sAddrIn.sin_family = AF_INET;
        sAddrIn.sin_port = htons(nPort);

        // Bind to the localhost ("127.0.0.1") to accept connections only from
        // localhost or
        // "0.0.0.0" (INADDR_ANY) to accept connections from any IP address.
        if (blnBindLocal)
            if (blnBindLocal) sAddrIn.sin_addr.s_addr = inet_addr("127.0.0.1");
            else sAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

        // Bind the structure to the created server socket.
        nRet = bind(this->m_ServSock, (struct sockaddr *) &sAddrIn, sizeof(sAddrIn));
        if (nRet < 0)
        {
            closesocket(this->m_ServSock);
            throw "server socket failed to bind on given port.";
        }

        // Set server socket in listen mode and set the listen queue to 20.
        nRet = listen(this->m_ServSock, 20);
        if (nRet < 0)
        {
            closesocket(this->m_ServSock);
            throw "server scket failed to listen.";
        }
    };

    // Returns a pointer to the ClientSocket instance, bind
    // to the accepted client socket (incoming connection).
    ClientSocket<T>* Accept()
    {
        SOCKET sAccept;
        struct sockaddr_in sForeignAddIn;
        int nLength = sizeof(struct sockaddr_in);
        ClientSocket<T> *ret = NULL;

        // Check if there is something in the listen queue.
        sAccept = WSAAccept(this->m_ServSock, (struct sockaddr *) &sForeignAddIn,
                            &nLength, NULL, NULL);

        // Check for errors.
        if (sAccept != INVALID_SOCKET)
        {
            // Get a pointer to the free ClientSocket
            // instance from the pool.
            ret = m_SockPool.GetFromQueue();
            if (ret == NULL)
            {
                // There are no free instances in the pool, maximum
                // number of accepted client connections is reached.
                ::shutdown(sAccept,2);
                ::closesocket(sAccept);
            }
            else
            {
                // Everything is fine, so associate the instance
                // with the client socket.
                ret->Lock();
                ret->Associate(sAccept, &sForeignAddIn);
                ret->UnLock();
            }
        }

        return ret;
    };

    // Release the client socket, will try closing connection and
    // will place it back to the pool.
    void Release(ClientSocket<T> *sock)
    {
        if (sock != NULL)
        {
            // check if client socket object is
            // assigned a socket handler. If yes,
            // close it.
            if (sock->IsBusy()) sock->CloseSocket();

            // Place it back to the pool.
            m_SockPool.Release(sock);
        }
    };

    vector<ClientSocket<T> *> *GetPool()
    {
        return m_SockPool.GetBlocks();
    };

    ~ServerSocket()
    {
        shutdown(this->m_ServSock, 2);
        closesocket(this->m_ServSock);
    };
};

#endif