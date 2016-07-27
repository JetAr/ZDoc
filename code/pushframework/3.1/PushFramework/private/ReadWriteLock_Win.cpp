#include "StdAfx.h"
#include "ReadWriteLock.h"


ReadWriteLock::ReadWriteLock(void)
{
	InitializeSRWLock(&rwLock);
}


ReadWriteLock::~ReadWriteLock(void)
{
}


void ReadWriteLock::acquireReadLock()
{
	AcquireSRWLockShared(&rwLock);
}
void ReadWriteLock::releaseReadLock()
{
	ReleaseSRWLockShared(&rwLock);
}
void ReadWriteLock::acquireWriteLock()
{
	AcquireSRWLockExclusive(&rwLock);
}
void ReadWriteLock::releaseWriteLock()
{
	ReleaseSRWLockExclusive(&rwLock);
}