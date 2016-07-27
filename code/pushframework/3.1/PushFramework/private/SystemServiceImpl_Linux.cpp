#include "StdAfx.h"
#include "SystemServiceImpl_Linux.h"
#include "../include/SystemService.h"

#include <signal.h>

namespace PushFramework
{
SystemServiceImpl* pMe;
sigset_t    mask;

SystemServiceImpl::SystemServiceImpl(const wchar_t* serviceName, SystemService* pFacade)
{
    this->pFacade = pFacade;
    pMe = this;
}

SystemServiceImpl::~SystemServiceImpl(void)
{
}

void * thr_fn(void *arg);

void SystemServiceImpl::Run()
{
    /*
     * Become a daemon.
     */
    daemonize();


    /*
     * Restore SIGHUP default and block all signals.
     */
    struct sigaction    sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
    {
        //can't restore SIGHUP default
        return;
    }

    int err;

    sigfillset(&mask);
    if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
    {
        //SIG_BLOCK error
        return;
    }
    /*
     * Create a thread to handle SIGHUP and SIGTERM.
     */
    pthread_t           tid;
    pthread_create(&tid, NULL, thr_fn, this);
    /*
     * Proceed with the rest of the daemon.
     */
    pFacade->OnStart();
}

void SystemServiceImpl::ProcessSignals()
{
    int err, signo;

    for (;;)
    {
        err = sigwait(&mask, &signo);
        if (err != 0)
        {
            syslog(LOG_ERR, "sigwait failed");
            exit(1);
        }

        switch (signo)
        {
        case SIGHUP:
            syslog(LOG_INFO, "Re-reading configuration file");
            //Nothing is done.
            break;

        case SIGTERM:
            syslog(LOG_INFO, "got SIGTERM; exiting");
            pMe->pFacade->OnStop();
            break;

        default:
            syslog(LOG_INFO, "unexpected signal %d\n", signo);
        }
    }
}
void SystemServiceImpl::daemonize()
{
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0)
    {
        //cannot fork daemon process
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */

    /* Create a new SID for the child process */
    int sid = setsid();
    if (sid < 0)
    {
        /* Log any failure */
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0)
    {
        /* Log any failure here */
        exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void *
thr_fn(void *arg)
{
    SystemServiceImpl* pMe = reinterpret_cast<SystemServiceImpl*> (arg);
    pMe->ProcessSignals();
}
}
