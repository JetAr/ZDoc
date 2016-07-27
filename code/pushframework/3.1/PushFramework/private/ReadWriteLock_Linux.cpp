#include "StdAfx.h"
#include "ReadWriteLock.h"


ReadWriteLock::ReadWriteLock(void)
{
	pthread_rwlock_init(&rwLock, NULL);
}

ReadWriteLock::~ReadWriteLock(void)
{
	pthread_rwlock_destroy(&rwLock);
}

void ReadWriteLock::acquireReadLock()
{
	pthread_rwlock_rdlock(&rwLock);
}
void ReadWriteLock::releaseReadLock()
{
	pthread_rwlock_unlock(&rwLock);
}
void ReadWriteLock::acquireWriteLock()
{
	pthread_rwlock_wrlock(&rwLock);
}
void ReadWriteLock::releaseWriteLock()
{
	pthread_rwlock_unlock(&rwLock);
}