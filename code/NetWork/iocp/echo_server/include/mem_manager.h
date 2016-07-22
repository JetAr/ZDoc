#ifndef ___MEM_MANAGER_H_INCLUDED___
#define ___MEM_MANAGER_H_INCLUDED___

#pragma warning(disable:4786)

#include <vector>
#include <queue>
#include <set>
#include "INCLUDE/sync_simple.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// A priority queue that allows pre-setting capacity of the container.

template<class T>
class mpriority_queue : public priority_queue<T, vector<T>, less<typename vector<T>::value_type> > {
public:
	void reserve(size_type _N) { c.reserve(_N); };
	size_type capacity() const { return c.capacity(); };
};


///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// A template class allowing re-using of the memory blocks.

template<class T>
class QueuedBlocks {
private:
	QMutex			m_qMutex;
	set< T* >		m_quBlocks;
	vector< T* >	m_allBlocks;

public:
	QueuedBlocks(int nInitSize = 1): m_qMutex(), m_quBlocks(), m_allBlocks() {
		int i;
		int nSize = (nInitSize <= 0)?1:nInitSize;

		// allocate and push the blocks to the queue
		for (i = 0; i < nSize; i++) {
			T *t = new T();
			if (t != NULL) {
				t->Clear();
				m_quBlocks.insert( t );
				m_allBlocks.push_back( t );
			}
		}
	};

	// get a free block from the queue, if one cannot be found
	// then NULL is returned
	T* GetFromQueue() {
		T* t = NULL;

		m_qMutex.Lock();
		if (!m_quBlocks.empty()) {
			set<T*>::iterator itPos = m_quBlocks.begin();
			t = *itPos;
			m_quBlocks.erase( t );
		}
		m_qMutex.Unlock();

		return t;
	};

	// get a free block from the queue, if one cannot be found
	// then a new one is created
	T* Get() {
		T* t = GetFromQueue();
		if (t == NULL) {
			t = new T();
			if (t != NULL) {
				t->Clear();
				m_qMutex.Lock();
				m_allBlocks.push_back( t );
				m_qMutex.Unlock();
			}
		}

		return t;
	};

	// Release the used block, place it
	// back to the queue. For performance reason,
	// we assume that the block was previously taken
	// from the queue.
	void Release(T* t) {
		if (t != NULL) {
			t->Clear();
			m_qMutex.Lock();
			m_quBlocks.insert( t );
			m_qMutex.Unlock();
		}
	};

	// Return all the blocks ever allocated.
	vector<T*> *GetBlocks() { return &m_allBlocks; };

	~QueuedBlocks() {
		m_qMutex.Lock();
		m_quBlocks.clear();

		vector<T*>::iterator itPos = m_allBlocks.begin();
		for (; itPos < m_allBlocks.end(); itPos++) {
			T* t = *itPos;
			t->Clear();
			delete t;
		}

		m_allBlocks.clear();
		m_qMutex.Unlock();
	};
};

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// A template class used for providing free blocks as well 
// as releasing unnecessary ones. Uses "QueuedBlocks" which 
// allows reusing of the blocks.
template<class T>
class StaticBlocks {
private:
	static QueuedBlocks<T> *blocks;

public:
	static void Init(int nSize = 1) {
		if (blocks == NULL) blocks = new QueuedBlocks<T>(nSize);
	};

	static T *Get() {
		if (blocks == NULL) return NULL;
		return blocks->Get();
	};

	static void Release(T *b) {
		if (blocks != NULL) blocks->Release(b);
	};
};

#endif
