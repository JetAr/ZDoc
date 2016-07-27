#include "StdAfx.h"
#include "ReadWriteLock.h"


ScopedReadWriteLock::ScopedReadWriteLock( ReadWriteLock& _rwLock, bool isWriter /*= false*/ )
	:rwLock(_rwLock)
{
	this->isWriter = isWriter;

	if (isWriter)
	{
		rwLock.acquireWriteLock();
	}
	else
	{
		rwLock.acquireReadLock();
	}
}

ScopedReadWriteLock::~ScopedReadWriteLock()
{
	if (isWriter)
	{
		rwLock.releaseWriteLock();
	}
	else
	{
		rwLock.releaseReadLock();
	}	//
}
