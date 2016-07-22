///////////////////////////////////////////////////////////////////////////////
//
// File           : $Workfile: SocketServer.cpp $
// Version        : $Revision: 8 $
// Function       :
//
// Author         : $Author: Len $
// Date           : $Date: 12/08/02 12:07 $
//
// Notes          :
//
// Modifications  :
//
// $Log: /Web Articles/SocketServers/EchoServer/EchoServer/SocketServer.cpp $
//
// 8     12/08/02 12:07 Len
// Handle new connection reset and connection error events.
//
// 7     6/07/02 17:44 Len
// Buffers and sockets are now flushed when we shutdown the server rather
// than in the server's destructor. This removes the need for us to flush
// them to ensure we get the notifications.
//
// 6     29/06/02 18:47 Len
// Changes due to the thread event changes and default worker thread
// implementation.  We now only have to derive from the socket server,
// rather than from socket server and worker thread and all notifications
// come via the socket server base class.
//
// 5     28/06/02 15:52 Len
// Handle the new connection closure notifications.
//
// 4     18/06/02 18:35 Len
// Removed ReuseAddress() as it's not required and it's an error to set it
// on the listening socket - you shouldn't need to and if you do it's more
// than likely a bug somewhere!
//
// 3     29/05/02 12:39 Len
// Lint options.
//
// 2     27/05/02 10:43 Len
// Use CSocket class to simplify the creation of the listening socket and
// to prevent resource leaks in the presence of exceptions.
//
// 1     21/05/02 11:39 Len
//
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2002 JetByte Limited.
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

#include "JetByteTools\Win32Tools\Utils.h"
#include "JetByteTools\Win32Tools\Exception.h"

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//
// Member not defined
//lint -esym(1526, CSocketServer::CSocketServer)
//lint -esym(1526, CSocketServer::operator=)
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Using directives
///////////////////////////////////////////////////////////////////////////////

using JetByteTools::Win32::CIOCompletionPort;
using JetByteTools::Win32::CIOBuffer;
using JetByteTools::Win32::Output;
using JetByteTools::Win32::ToString;
using JetByteTools::Win32::_tstring;
using JetByteTools::Win32::CException;
using JetByteTools::Win32::GetLastErrorMessage;

///////////////////////////////////////////////////////////////////////////////
// CSocketServer
///////////////////////////////////////////////////////////////////////////////

CSocketServer::CSocketServer(
    const std::string &welcomeMessage,
    unsigned long addressToListenOn,
    unsigned short portToListenOn,
    size_t maxFreeSockets,
    size_t maxFreeBuffers,
    size_t bufferSize /* = 1024 */,
    size_t numThreads /* = 0 */)
    :  JetByteTools::Win32::CSocketServer(addressToListenOn, portToListenOn, maxFreeSockets, maxFreeBuffers, bufferSize, numThreads),
       m_welcomeMessage(welcomeMessage)
{

}

void CSocketServer::OnStartAcceptingConnections()
{
    Output(_T("OnStartAcceptingConnections"));
}

void CSocketServer::OnStopAcceptingConnections()
{
    Output(_T("OnStopAcceptingConnections"));
}

void CSocketServer::OnShutdownInitiated()
{
    Output(_T("OnShutdownInitiated"));
}

void CSocketServer::OnShutdownComplete()
{
    Output(_T("OnShutdownComplete"));
}

void CSocketServer::OnConnectionEstablished(
    Socket *pSocket,
    CIOBuffer * /*pAddress*/)
{
    Output(_T("OnConnectionEstablished"));

    pSocket->Write(m_welcomeMessage.c_str(), m_welcomeMessage.length());

    pSocket->Read();
}

void CSocketServer::OnConnectionClientClose(
    Socket * /*pSocket*/)
{
    Output(_T("OnConnectionClientClose"));
}

void CSocketServer::OnConnectionReset(
    Socket * /*pSocket*/)
{
    Output(_T("OnConnectionReset"));
}

bool CSocketServer::OnConnectionClosing(
    Socket *pSocket)
{
    Output(_T("OnConnectionClosing"));

    // we'll handle socket closure so that we can do a lingering close
    // note that this is not ideal as this code is executed on the an
    // IO thread. If we had a thread pool we could fire this off to the
    // thread pool to handle.

    pSocket->Close();

    return true;
}

void CSocketServer::OnConnectionClosed(
    Socket * /*pSocket*/)
{
    Output(_T("OnConnectionClosed"));
}

void CSocketServer::OnConnectionCreated()
{
    Output(_T("OnConnectionCreated"));
}

void CSocketServer::OnConnectionDestroyed()
{
    Output(_T("OnConnectionDestroyed"));
}

void CSocketServer::OnConnectionError(
    ConnectionErrorSource source,
    Socket *pSocket,
    CIOBuffer *pBuffer,
    DWORD lastError)
{
    const LPCTSTR errorSource = (source == ZeroByteReadError ? _T(" Zero Byte Read Error:") : (source == ReadError ? _T(" Read Error:") : _T(" Write Error:")));

    Output(_T("OnConnectionError - Socket = ") + ToString(pSocket) + _T(" Buffer = ") + ToString(pBuffer) + errorSource + GetLastErrorMessage(lastError));
}

void CSocketServer::OnError(
    const JetByteTools::Win32::_tstring &message)
{
    Output(_T("OnError - ") + message);
}

void CSocketServer::OnBufferCreated()
{
    Output(_T("OnBufferCreated"));
}

void CSocketServer::OnBufferAllocated()
{
    Output(_T("OnBufferAllocated"));
}

void CSocketServer::OnBufferReleased()
{
    Output(_T("OnBufferReleased"));
}

void CSocketServer::OnBufferDestroyed()
{
    Output(_T("OnBufferDestroyed"));
}

void CSocketServer::OnThreadCreated()
{
    Output(_T("OnThreadCreated"));
}

void CSocketServer::OnThreadBeginProcessing()
{
    Output(_T("OnThreadBeginProcessing"));
}

void CSocketServer::OnThreadEndProcessing()
{
    Output(_T("OnThreadEndProcessing"));
}

void CSocketServer::OnThreadDestroyed()
{
    Output(_T("OnThreadDestroyed"));
}

void CSocketServer::ReadCompleted(
    Socket *pSocket,
    CIOBuffer *pBuffer)
{
    try
    {
        EchoMessage(pSocket, pBuffer);

        pSocket->Read();
    }
    catch(const CException &e)
    {
        Output(_T("ReadCompleted - Exception - ") + e.GetWhere() + _T(" - ") + e.GetMessage());
        pSocket->Shutdown();
    }
    catch(...)
    {
        Output(_T("ReadCompleted - Unexpected exception"));
        pSocket->Shutdown();
    }
}

void CSocketServer::EchoMessage(
    Socket *pSocket,
    CIOBuffer *pBuffer) const
{
    pSocket->Write(pBuffer);
}

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -restore
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////
