#include "StdAfx.h"
#include "SystemServiceImpl_Win.h"
#include "..\include\SystemService.h"

namespace PushFramework
{
SystemServiceImpl* pMe;

SystemServiceImpl::SystemServiceImpl(const wchar_t* serviceName, SystemService* pFacade)
{
    this->serviceName = serviceName;
    this->pFacade = pFacade;
    pMe = this;
}

SystemServiceImpl::~SystemServiceImpl(void)
{
}

void SystemServiceImpl::Run()
{
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { (wchar_t*) serviceName.c_str(), (LPSERVICE_MAIN_FUNCTION) _SvcMain },
        { NULL, NULL }
    };

    // This call returns when the service has stopped.
    // The process should simply terminate when the call returns.

    StartServiceCtrlDispatcher( DispatchTable );
}

void SystemServiceImpl::IndicateRunning()
{
    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
}

bool SystemServiceImpl::Install( std::wstring displayName )
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szPath[MAX_PATH];

    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
    {
        return false;
    }

    // Get a handle to the SCM database.

    schSCManager = OpenSCManager(
                       NULL,                    // local computer
                       NULL,                    // ServicesActive database
                       SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager)
    {
        return false;
    }

    // Create the service

    schService = CreateService(
                     schSCManager,              // SCM database
                     serviceName.c_str(),                   // name of service
                     displayName.c_str(),                   // service name to display
                     SERVICE_ALL_ACCESS,        // desired access
                     SERVICE_WIN32_OWN_PROCESS, // service type
                     SERVICE_DEMAND_START,      // start type
                     SERVICE_ERROR_NORMAL,      // error control type
                     szPath,                    // path to service's binary
                     NULL,                      // no load ordering group
                     NULL,                      // no tag identifier
                     NULL,                      // no dependencies
                     NULL,                      // LocalSystem account
                     NULL);                     // no password

    if (schService == NULL)
    {
        CloseServiceHandle(schSCManager);
        return false;
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return true;
}

bool SystemServiceImpl::UnInstall()
{
    SC_HANDLE service, scm;
    SERVICE_STATUS ss;

    // open a connection to the SCM
    scm = OpenSCManager(0, 0,SC_MANAGER_ALL_ACCESS);
    if (!scm)
    {
        return false;
    }

    //open the service
    service = OpenService(scm,serviceName.c_str(),SERVICE_ALL_ACCESS|DELETE );
    if(service == NULL)
    {
        // clean up
        CloseServiceHandle(scm);
        return false;
    }

    ControlService(service,SERVICE_CONTROL_STOP,&ss);
    //delete the service

	BOOL bRet = DeleteService(service);

	// clean up
	CloseServiceHandle(service);
	CloseServiceHandle(scm);   

    return bRet != FALSE;
}

VOID WINAPI SystemServiceImpl::_SvcCtrlHandler( DWORD dwCtrl )
{
    pMe->SvcCtrlHandler(dwCtrl);
}

VOID WINAPI SystemServiceImpl::SvcCtrlHandler( DWORD dwCtrl )
{
    // Handle the requested control code.

    switch(dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        //
        ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
        // Signal the service to stop.
        pFacade->OnStop();
        return;

    case SERVICE_CONTROL_INTERROGATE:
        break;

        // Pause the service
    case SERVICE_CONTROL_PAUSE:
        ReportSvcStatus(SERVICE_PAUSE_PENDING, NO_ERROR, 0);
        if (pFacade->OnPause())
        {
            ReportSvcStatus(SERVICE_PAUSED, NO_ERROR, 0);
        }
        else
        {
            ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
        }
        break;

        // Resume from a pause
    case SERVICE_CONTROL_CONTINUE:
        ReportSvcStatus(SERVICE_CONTINUE_PENDING, NO_ERROR, 0);
        if (pFacade->OnResume())
        {
            ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
        }
        else
        {
            ReportSvcStatus(SERVICE_PAUSED, NO_ERROR, 0);
        }
        break;

    default:
        break;
    }
}

VOID SystemServiceImpl::ReportSvcStatus( DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint )
{
    static DWORD dwCheckPoint = 1;

    SERVICE_STATUS          gSvcStatus;
    // Fill in the SERVICE_STATUS structure.
    // These SERVICE_STATUS members remain as set here
    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gSvcStatus.dwServiceSpecificExitCode = 0;

    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;


    if (dwCurrentState == SERVICE_START_PENDING ||
            dwCurrentState == SERVICE_CONTINUE_PENDING ||
            dwCurrentState == SERVICE_PAUSE_PENDING ||
            dwCurrentState == SERVICE_STOP_PENDING)
        gSvcStatus.dwControlsAccepted = 0;
    else
        gSvcStatus.dwControlsAccepted =  SERVICE_ACCEPT_STOP |
                                         SERVICE_ACCEPT_PAUSE_CONTINUE;


    if ( (dwCurrentState == SERVICE_RUNNING) ||
            (dwCurrentState == SERVICE_STOPPED) )
        gSvcStatus.dwCheckPoint = 0;
    else gSvcStatus.dwCheckPoint = dwCheckPoint++;

    // Report the status of the service to the SCM.
}

void SystemServiceImpl::SvcMain( DWORD, LPTSTR* )
{
    // Register the handler function for the service
    gSvcStatusHandle = RegisterServiceCtrlHandler(
                           serviceName.c_str(),
                           _SvcCtrlHandler);

    if( !gSvcStatusHandle )
    {
        return;
    }

    // Report initial status to the SCM
    ReportSvcStatus( SERVICE_START_PENDING, NO_ERROR, 3000 );

    // Perform service-specific initialization and work.
    pFacade->OnStart();//responsible for calling indicateRunning

    ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
}

VOID WINAPI SystemServiceImpl::_SvcMain( DWORD dwArgc, LPTSTR *lpszArgv )
{
    pMe->SvcMain(dwArgc, lpszArgv);
}
}