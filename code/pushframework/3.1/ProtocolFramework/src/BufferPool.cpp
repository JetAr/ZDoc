#include "stdafx.h"
#include "BufferPool.h"
#include "AbstractPool.h"
#include "MemorySegment.h"

namespace ProtocolFramework
{
	class SegmentPool : public AbstractPool
	{
	public:
		SegmentPool(unsigned int nSize)
		{
			this->nSize = nSize;
		}
		~SegmentPool()
		{
		}

	private:
		unsigned int nSize;

	protected:
		virtual void deleteImpl(ObjectPtr obj)
		{
			MemorySegment* segment = (MemorySegment*) obj;
			delete segment;
		};

		virtual ObjectPtr createImpl()
		{
			MemorySegment* segment = new MemorySegment();
			segment->data = new char[nSize];
			segment->size = nSize;

			return segment;
		}
		virtual void recycleObject(ObjectPtr p)
		{
			//
		};
	};

	//
	struct BufferPool::Internal
	{
		Internal()
		{

		}
		~Internal()
		{
			unordered_map<unsigned int, SegmentPool*>::iterator it = segmentsBySize.begin();
			while (it != segmentsBySize.end())
			{
				delete it->second;
				segmentsBySize.erase(it);
				it = segmentsBySize.begin();
			}
		}
		unordered_map<unsigned int, SegmentPool*> segmentsBySize;
	};

	BufferPool pool;


	BufferPool::BufferPool(void)
	{
		internal = new Internal();
	}

	BufferPool::~BufferPool(void)
	{
		delete internal;
	}

	bool BufferPool::create(int type, unsigned int nCount, unsigned int nSize )
	{
		SegmentPool* segPool = new SegmentPool(nSize);

		if (!segPool->initialize(nCount))
			return false;

		internal->segmentsBySize[type] = segPool;

		return true;
	}


	MemorySegment* BufferPool::getMemorySegment( int type )
	{
		return (MemorySegment*) internal->segmentsBySize[type]->borrowObject();
	}

	void BufferPool::returnMemorySegment( MemorySegment* segment, int type )
	{
		internal->segmentsBySize[type]->returnObject(segment);
	}

}