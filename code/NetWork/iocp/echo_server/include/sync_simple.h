#ifndef ___SYNC_SIMPLE_H_INCLUDED___
#define ___SYNC_SIMPLE_H_INCLUDED___

#include <windows.h>

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// Implementation of the critical section
class QMutex {
private:
	CRITICAL_SECTION	m_Cs;

public:
	QMutex() { ::InitializeCriticalSection(&this->m_Cs); }
	~QMutex() { ::DeleteCriticalSection(&this->m_Cs); }
	void Lock() { ::EnterCriticalSection(&this->m_Cs); }
	BOOL TryLock() { return (BOOL) ::TryEnterCriticalSection(&this->m_Cs); }
	void Unlock() { ::LeaveCriticalSection(&this->m_Cs); }
};

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// Implementation of the semaphore
class QSemaphore {
private:
	HANDLE	m_hSemaphore;
	long m_lMaximumCount;

public:
	QSemaphore(long lMaximumCount) {
		this->m_hSemaphore = ::CreateSemaphore(NULL, lMaximumCount, lMaximumCount, NULL);

		if (this->m_hSemaphore == NULL) throw "Call to CreateSemaphore() failed. Could not create semaphore.";
		this->m_lMaximumCount = lMaximumCount;
	};

	~QSemaphore() { ::CloseHandle(this->m_hSemaphore); };

	long GetMaximumCount() const { return this->m_lMaximumCount; };
	void Inc() { ::WaitForSingleObject(this->m_hSemaphore, INFINITE); };
	void Dec() { ::ReleaseSemaphore(this->m_hSemaphore, 1, NULL); };
	void Dec(long lCount) { ::ReleaseSemaphore(this->m_hSemaphore, lCount, NULL); };
};

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// Implementation of the read-write mutex.
// Multiple threads can have read access at the same time.
// Write access is exclusive for only one thread.
class ReadWriteMutex {
private:
	QMutex		m_qMutex;
	QSemaphore	m_qSemaphore;

public:
	ReadWriteMutex(long lMaximumReaders): m_qSemaphore(lMaximumReaders) {};

	void lockRead() { m_qSemaphore.Inc(); };
	void unlockRead() { m_qSemaphore.Dec(); };

	void lockWrite() {
		m_qMutex.Lock();
		for (int i = 0; i < maxReaders(); ++i) m_qSemaphore.Inc();
		m_qMutex.Unlock();
	};
	
	void unlockWrite() {  m_qSemaphore.Dec(m_qSemaphore.GetMaximumCount()); };
	int maxReaders() const { return m_qSemaphore.GetMaximumCount(); };
};

#endif