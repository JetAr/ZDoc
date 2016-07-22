// echo_server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define _WIN32_WINNT 0x0400

#include <stdio.h>
#include <conio.h>
#include <time.h>
#include "INCLUDE/server_service.h"

#define BUFF_SIZE 8
#define MAX_CONNECTIONS 10
#define NO_THREADS 4
#define TIME_OUT 10
#define PORT 8080

//---------------------------------------------------------------------------------
struct Attachment
{
    volatile time_t				tmLastActionTime;
    char						sString[BUFF_SIZE];
    DWORD						dwStringSize; // current string size

    Attachment()
    {
        Clear();
    };

    bool Commit( DWORD dwBytesTransferred )
    {
        DWORD dwSize = dwStringSize + dwBytesTransferred;

        if ( dwBytesTransferred <= 0 ) return false;
        if ( dwSize >= BUFF_SIZE ) return false;

        dwStringSize = dwSize;
        sString[dwStringSize] = 0;
        return true;
    };

    // as requested by the API of the framework
    void Clear()
    {
        memset(this, 0, sizeof(Attachment) );
    };

    // as requested by the API of the framework
    void ResetTime( bool toZero )
    {
        if (toZero) tmLastActionTime = 0;
        else
        {
            time_t	lLastActionTime;
            time(&lLastActionTime);
            tmLastActionTime = lLastActionTime;
        }
    };

    // as requested by the API of the framework
    long GetTimeElapsed()
    {
        time_t tmCurrentTime;

        if (0 == tmLastActionTime) return 0;

        time(&tmCurrentTime);
        return static_cast<long>(tmCurrentTime - tmLastActionTime);
    };
};

//---------------------------------------------------------------------------------
typedef ClientSocket<Attachment> MyCSocket;
typedef ServerSocket<Attachment> MySSocket;
typedef IOCPSimple<Attachment> MyIOCPSimple;
typedef ISockEvent<Attachment> MyISockEvent;
typedef ServerService<Attachment> MyServerService;
//---------------------------------------------------------------------------------

class MyISockEventHandler: public MyISockEvent
{
public:
    MyISockEventHandler() {};
    ~MyISockEventHandler() {};

    // empty method, not used
    virtual void OnClose( MyCSocket *pSocket, MYOVERLAPPED *pOverlap,
                          MySSocket *pServerSocket, MyIOCPSimple *pHIocp ) {};

    // empty method, not used
    virtual void OnPending( MyCSocket *pSocket, MYOVERLAPPED *pOverlap,
                            MySSocket *pServerSocket, MyIOCPSimple *pHIocp ) {};

    virtual void OnAccept( MyCSocket *pSocket, MYOVERLAPPED *pOverlap,
                           MySSocket *pServerSocket, MyIOCPSimple *pHIocp )
    {
        int nRet;
        DWORD dwSize;
        char *temp;

        dwSize = BUFF_SIZE - 1;
        temp = pSocket->GetAttachment()->sString;

        // initiate the reading with OnAccept
        nRet = pSocket->ReadFromSocket( temp, dwSize );
        pSocket->GetAttachment()->ResetTime( false );

        if ( nRet == SOCKET_ERROR )
        {
            pServerSocket->Release( pSocket );
        }
    };

    virtual void OnReadFinalized( MyCSocket *pSocket, MYOVERLAPPED *pOverlap,
                                  DWORD dwBytesTransferred, MySSocket *pServerSocket, MyIOCPSimple *pHIocp )
    {
        int nRet;
        DWORD dwSize, dwPos;
        char *temp;

        // finalize the filling of the buffer
        pSocket->GetAttachment()->Commit( dwBytesTransferred );

        dwSize = BUFF_SIZE - 1;
        dwPos = pSocket->GetAttachment()->dwStringSize;
        temp = pSocket->GetAttachment()->sString;

        nRet = pSocket->ReadFromSocket(	temp + dwPos, dwSize - dwPos );
        pSocket->GetAttachment()->ResetTime( false );

        if ( nRet == SOCKET_ERROR )
        {
            pServerSocket->Release( pSocket );
        }
        else if ( nRet == RECV_BUFFER_EMPTY )
        {
            // means that dwSize - dwPos == 0, so send the data
            // back to the socket

            nRet = pSocket->WriteToSocket( temp, dwSize );
            if ( nRet == SOCKET_ERROR )
            {
                pServerSocket->Release( pSocket );
            }
        }
    };

    virtual void OnWriteFinalized( MyCSocket *pSocket, MYOVERLAPPED *pOverlap,
                                   DWORD dwBytesTransferred, MySSocket *pServerSocket, MyIOCPSimple *pHIocp )
    {
        char *temp = pSocket->GetAttachment()->sString;

        // clean the attachment
        pSocket->GetAttachment()->Clear();

        // and once again
        OnAccept(pSocket, NULL,pServerSocket, NULL);
    };
};

//---------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    int	nRet;
    MyServerService *sService;
    MyISockEventHandler *mSockHndl;
    WSAData	wsData;

    nRet = WSAStartup(MAKEWORD(2,2),&wsData);
    if ( nRet < 0 )
    {
        Log::LogMessage(L"Can't load winsock.dll.\n");
        return -1;
    }

    try
    {
        Overlapped::Init( MAX_CONNECTIONS );
        mSockHndl = new MyISockEventHandler();

        sService = new MyServerService((MyISockEvent *) mSockHndl, PORT,
                                       MAX_CONNECTIONS, NO_THREADS, TIME_OUT, false);
        sService->start();

        printf("hit <ENTER> to stop ...\n");
        while( !_kbhit() ) ::Sleep(100);

        delete sService;
        delete mSockHndl;
    }
    catch (const char *err)
    {
        printf("%s\n", err);
    }
    catch (const wchar_t *err)
    {
        wprintf(L"%ls\n", err);
    }

    WSACleanup();
    return 0;
}

