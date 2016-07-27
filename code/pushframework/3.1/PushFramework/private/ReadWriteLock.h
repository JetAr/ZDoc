#pragma once
#ifdef Plateform_Windows
typedef SRWLOCK RWLockType;
#else

#ifdef Plateform_Linux
typedef pthread_rwlock_t RWLockType;
#endif

#endif
class ReadWriteLock
{
public:
	ReadWriteLock(void);
	~ReadWriteLock(void);

	void acquireReadLock();
	void releaseReadLock();
	void acquireWriteLock();
	void releaseWriteLock();

private:
	RWLockType rwLock;
};


class ScopedReadWriteLock
{
public:
	ScopedReadWriteLock(ReadWriteLock& rwLock, bool isWriter = false);
	~ScopedReadWriteLock();

private:
	ReadWriteLock& rwLock;
	bool isWriter;

};
