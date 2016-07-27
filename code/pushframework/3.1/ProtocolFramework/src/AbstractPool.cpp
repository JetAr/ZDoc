#include "stdafx.h"
#include "AbstractPool.h"

namespace ProtocolFramework
{
	struct AbstractPool::Internal
	{
		Internal()
		{
			InitializeCriticalSection(&cs);
		}
		~Internal()
		{
			DeleteCriticalSection(&cs);
		}

		unordered_set<ObjectPtr> objectsInUse;
		vector<ObjectPtr> freeObjects;
		CRITICAL_SECTION cs;		
	};

	AbstractPool::AbstractPool()
	{
		internal = new Internal();
	}

	AbstractPool::~AbstractPool( void )
	{
		delete internal;
	}

	bool AbstractPool::initialize( unsigned int nRequiredObjects )
	{
		try
		{
			for (unsigned int i=0; i<nRequiredObjects; i++)
				internal->freeObjects.push_back(createImpl());
		}
		catch (...)
		{
			return false;
		}

		return true;		
	}

	AbstractPool::ObjectPtr AbstractPool::borrowObject()
	{
		EnterCriticalSection(&internal->cs);

		if (internal->freeObjects.empty())
		{
			LeaveCriticalSection(&internal->cs);
			return NULL;
		}

		ObjectPtr p = internal->freeObjects.back();
		internal->freeObjects.pop_back();

		internal->objectsInUse.insert(p);

		LeaveCriticalSection(&internal->cs);
		return p;
	}

	void AbstractPool::returnObject( ObjectPtr p )
	{
		EnterCriticalSection(&internal->cs);

		unordered_set<ObjectPtr>::iterator it = internal->objectsInUse.find(p);

		internal->objectsInUse.erase(it);
		internal->freeObjects.push_back(p);

		recycleObject(p);

		LeaveCriticalSection(&internal->cs);
	}
}