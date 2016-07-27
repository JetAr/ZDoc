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
    void IndicateRunning()
    {
        //
    }
    bool Install(std::wstring displayName)
    {
        //Nothing to do.
        return true;
    }
    bool UnInstall()
    {
        //Nothing to do.

        return true;
    }

    void ProcessSignals();



private:
    SystemService* pFacade;
    void daemonize();

};
}

