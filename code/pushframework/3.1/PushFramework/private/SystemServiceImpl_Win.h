#pragma once
namespace PushFramework
{
class SystemService;

class SystemServiceImpl
{
public:
    SystemServiceImpl(const wchar_t* serviceName, SystemService* pFacade);
    ~SystemServiceImpl(void);

    void Run();
    void IndicateRunning();
    bool Install(std::wstring displayName);
    bool UnInstall();

private:
    std::wstring serviceName;
    SystemService* pFacade;
    SERVICE_STATUS_HANDLE   gSvcStatusHandle;

    static VOID WINAPI _SvcMain(DWORD, LPTSTR*);
    void SvcMain(DWORD, LPTSTR*);
    VOID ReportSvcStatus( DWORD dwCurrentState,	DWORD dwWin32ExitCode,	DWORD dwWaitHint);



    static VOID WINAPI _SvcCtrlHandler( DWORD dwCtrl );
    VOID WINAPI SvcCtrlHandler( DWORD dwCtrl );
};
}