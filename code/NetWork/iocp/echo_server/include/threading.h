#ifndef ___THREADING_H_INCLUDED___
#define ___THREADING_H_INCLUDED___

#include <windows.h>
#include "INCLUDE/mem_manager.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
class CThread;
class CSimpleThreadPool;

///////////////////////////////////////////////////////////////////////////////////////////////
// The core interface of this threading implementation.
class IRunnable {
	friend class CThread;
	friend class CSimpleThreadPool;
protected:
	virtual void run() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// structure holds Thread Local Storage descriptor
struct TLSDescriptor {
	DWORD descriptor;

	TLSDescriptor() {
		descriptor = TlsAlloc();
		if (descriptor == -1) throw "TlsAlloc() failed to create descriptor";
	};

	~TLSDescriptor() { TlsFree(descriptor); };
};

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// Very basic thread class, implementing basic threading API
class CThread: public IRunnable {
private:
	static TLSDescriptor m_TLSDesc;
	volatile bool	m_bIsInterrupted;
	volatile bool	m_bIsRunning;

	int				m_nThreadPriority;
	IRunnable		*m_RunObj;
	QMutex			m_qMutex;

	// See ::CreateThread(...) within the start() method. This is
	// the thread's API function to be executed. Method executes
	// run() method of the CThread instance passed as parameter.
	static DWORD WINAPI StartThreadST(LPVOID PARAM) {
		CThread *_this = (CThread *) PARAM;

		if (_this != NULL) {
			_this->m_qMutex.Lock();

			// Set the pointer to the instance of the passed CThread
			// in the current Thread's Local Storage. Also see
			// currentThread() method.
			TlsSetValue(CThread::m_TLSDesc.descriptor, (LPVOID) _this);
			_this->run();
			_this->m_bIsRunning = false;
			_this->m_qMutex.Unlock();
		}
		return 0;
	};

protected:
	// It is not possible to instantiate CThread objects directly. Possible only by
	// specifying a IRunnable object to execute its run() method.
	CThread(int nPriority = THREAD_PRIORITY_NORMAL): m_qMutex() {
		this->m_bIsInterrupted = false;
		this->m_bIsRunning = false;
		this->m_nThreadPriority = nPriority;
		this->m_RunObj = NULL;
	};

	// this implementation of the run() will execute the passed IRunnable
	// object (if not null). Inheriting class is responsible for using this
	// method or overriding it with a different one.
	virtual void run() {
		if (this->m_RunObj != NULL) this->m_RunObj->run();
	};

public:
	CThread(IRunnable *RunTask, int nPriority = THREAD_PRIORITY_NORMAL): m_qMutex() {
		this->m_bIsInterrupted = false;
		this->m_bIsRunning = false;
		this->m_nThreadPriority = nPriority;

		if (this != RunTask) this->m_RunObj = RunTask;
		else throw "Self referencing not allowed.";
	};

	virtual ~CThread() {
		this->interrupt();
		// wait until thread ends
		this->join();
	};

	// Method returns the instance of a CThread responsible
	// for the context of the current thread.
	static CThread& currentThread() {
		CThread *thr = (CThread *) TlsGetValue(CThread::m_TLSDesc.descriptor);
		if (thr == NULL) throw "Call is not within a CThread context.";
		return *thr;
	};

	// Method signals thread to stop execution.
	void interrupt() { this->m_bIsInterrupted = true; };

	// Check if thread was signaled to stop.
	bool isInterrupted() { return this->m_bIsInterrupted; };

	// Method will wait for thread's termination.
	void join() {
		this->m_qMutex.Lock();
		this->m_qMutex.Unlock();
	};

	// Method starts the Thread. If thread is already started/running, method
	// will simply return.
	void start() {
		HANDLE	hThread;
		LPTHREAD_START_ROUTINE pStartRoutine = &CThread::StartThreadST;

		if (this->m_qMutex.TryLock()) {
			if (!this->m_bIsRunning) {
				this->m_bIsRunning = true;
				this->m_bIsInterrupted = false;

				hThread = ::CreateThread(NULL, 0, pStartRoutine, (PVOID) this, 0, NULL);
				if (hThread == NULL) {
					this->m_bIsRunning = false;
					this->m_qMutex.Unlock();
					throw "Failed to call CreateThread(). Thread not started.";
				}

				::SetThreadPriority(hThread, this->m_nThreadPriority);
				::CloseHandle(hThread);
			}

			this->m_qMutex.Unlock();
		}
	};
};

TLSDescriptor CThread::m_TLSDesc;

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// Helper class to submit tasks to the CSimpleThreadPool
class CPriorityTask {
private:
	int			m_nPriority;
	IRunnable	*m_pRunObj;

public:
	CPriorityTask(const CPriorityTask &t) {
		m_pRunObj = t.m_pRunObj;
		m_nPriority = t.m_nPriority;
	};

	CPriorityTask() {
		m_pRunObj = NULL;
		m_nPriority = 0;
	};

	CPriorityTask(IRunnable *pRunObj, int nPriority = 0) {
		m_pRunObj = pRunObj;
		m_nPriority = nPriority;
	};

	int getPriority() const { return m_nPriority; };
	IRunnable *getTask() const { return m_pRunObj; };

	~CPriorityTask() {};

	CPriorityTask& operator=(const CPriorityTask &t) {
		m_nPriority = t.m_nPriority;
		m_pRunObj = t.m_pRunObj;
		return *this;
	};
};

//Overload the < operator.
bool operator< (const CPriorityTask& pt1, const CPriorityTask& pt2) {
	return pt1.getPriority() < pt2.getPriority();	
}

//Overload the > operator.
bool operator> (const CPriorityTask& pt1, const CPriorityTask& pt2) {
	printf("compare > by ref\n");
	return pt1.getPriority() > pt2.getPriority();	
}

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// A class containing a collection of CThreadTask's.
// Every CThreadTask will execute same CSimpleThreadPool::run() method.
class CSimpleThreadPool: public IRunnable {
private:
	QMutex							m_qMutex;
	vector<CThread*>				m_arrThreadTasks;
	mpriority_queue<CPriorityTask>	m_PQueue;

	// Method will return a task from the queue,
	// if there are no tasks in the queue, method will return NULL.
	IRunnable *get() {
		IRunnable *ret = NULL;

		m_qMutex.Lock();
		if (!m_PQueue.empty()) {
			CPriorityTask t = m_PQueue.top();
			m_PQueue.pop();
			ret = t.getTask();
		}
		m_qMutex.Unlock();
		return ret;
	};

public:
	// How many threads are in the collection.
	int threads() const { m_arrThreadTasks.size(); };

	// Method starts pool's threads.
	void startAll() {
		for (size_t i = 0; i < m_arrThreadTasks.size(); i++) {
			m_arrThreadTasks[i]->start();
		}
	};

	// Constructor creates the thread pool and sets capacity for the task queue.
	CSimpleThreadPool(unsigned int nThreadsCount, unsigned int nQueueCapacity = 16): 
		m_qMutex(), m_PQueue() {
		UINT i;
		CThread *thTask = NULL;

		if (nThreadsCount <= 0) throw "Invalid number of threads supplied.";
		if (nQueueCapacity <= 0) throw "Invalid capacity supplied.";

		// Set initial capacity of the tasks Queue.
		m_PQueue.reserve(nQueueCapacity);

		// Initialize thread pool.
		for (i = 0; i < nThreadsCount; i++) {
			thTask = new CThread(this);
			if (thTask != NULL) m_arrThreadTasks.push_back(thTask);
		}
	};

	// Submit a new task to the pool
	void submit(IRunnable *pRunObj, int nPriority = 0) {
		if (this == pRunObj) throw "Self referencing not allowed.";

		m_qMutex.Lock();
		m_PQueue.push(CPriorityTask(pRunObj, nPriority));
		m_qMutex.Unlock();
	};

	// Method will execute task's run() method within its CThread context.
	virtual void run() {
		IRunnable *task;

		while (!CThread::currentThread().isInterrupted()) {
			// Get a task from the queue.
			task = get();

			// Execute the task.
			if (task != NULL) task->run();
			::Sleep(2);
		}
	};

	virtual ~CSimpleThreadPool() {
		vector<CThread*>::iterator itPos = m_arrThreadTasks.begin();

		for (; itPos < m_arrThreadTasks.end(); itPos++) delete *itPos;
		m_arrThreadTasks.clear();

		while (!m_PQueue.empty()) { m_PQueue.pop(); }
	};

	// Method signals threads to stop and waits for termination
	void shutdown() {
		for (size_t i = 0; i < m_arrThreadTasks.size(); i++) {
			m_arrThreadTasks[i]->interrupt();
			m_arrThreadTasks[i]->join();
		}
	};
};

#endif