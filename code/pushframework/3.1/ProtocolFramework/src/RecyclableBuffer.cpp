#include "stdafx.h"
#include "RecyclableBuffer.h"
#include "MemorySegment.h"

namespace ProtocolFramework
{

	RecyclableBuffer::RecyclableBuffer( Type type )
	{
		doAllocate(type);
	}

	RecyclableBuffer::~RecyclableBuffer( void )
	{
		if (segment)
		{
			pool.returnMemorySegment(segment, type);
			segment = NULL;
		}
	}

	void RecyclableBuffer::doAllocate()
	{
		segment = pool.getMemorySegment(type);
		if (segment == NULL)
		{
			//assert(0);
			return;
		}

		Buffer::assign(segment->data, segment->size);
	}

	void RecyclableBuffer::doAllocate( Type type )
	{
		this->type = type;
		segment = NULL;
		if (type != UnAllocated)
		{
			doAllocate();
		}
	}

}
