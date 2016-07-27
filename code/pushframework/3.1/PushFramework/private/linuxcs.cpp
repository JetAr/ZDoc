#include "linuxcs.h"
#include <pthread.h>

BOOL EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    pthread_mutex_t *lpMutex = (pthread_mutex_t*)lpCriticalSection;

    if(lpMutex == NULL)
        return FALSE;

    if(pthread_mutex_lock(lpMutex) != 0)
        return FALSE;

    return TRUE;
}

BOOL TryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    pthread_mutex_t *lpMutex = (pthread_mutex_t*)lpCriticalSection;

    if(lpMutex == NULL)
        return FALSE;

     if(pthread_mutex_trylock(lpMutex) != 0)
        return FALSE;

    return TRUE;
}

BOOL LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    pthread_mutex_t *lpMutex = (pthread_mutex_t*)lpCriticalSection;

    if(lpMutex == NULL)
        return FALSE;

    if(pthread_mutex_unlock(lpMutex) != 0)
        return FALSE;

    return TRUE;
}

BOOL InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    pthread_mutex_t *lpMutex = (pthread_mutex_t*)lpCriticalSection;
    pthread_mutexattr_t mutexattr;

    if(lpMutex == NULL)
        return FALSE;

    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
    //pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_NORMAL);

    if(pthread_mutex_init(lpMutex, &mutexattr) != 0)
        return FALSE;

    pthread_mutexattr_destroy(&mutexattr);

    return TRUE;
}

BOOL DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
    pthread_mutex_t *lpMutex = (pthread_mutex_t*)lpCriticalSection;
    if(lpMutex == NULL)
        return FALSE;

    if(pthread_mutex_trylock(lpMutex))
        pthread_mutex_unlock(lpMutex);
    else
        pthread_mutex_unlock(lpMutex);

    pthread_mutex_destroy(lpMutex);

    return TRUE;
}

