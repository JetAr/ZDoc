#pragma once
#include "ProtocolFramework.h"
#include "Buffer.h"
#include "BufferPool.h"
//  

namespace ProtocolFramework
{
	// This will make use of BufferPool object to borrow memory segments instead of allocating memory by itself.
	// When memory is borrowed it is assigned to parent Buffer class so its dtor does not delete it.
	// The dtor of RecyclableBuffer will send back the segment to the pool to be recycled.
	class PROTOCOLFRAMEWORK_API RecyclableBuffer : public Buffer
	{
	public:
		enum Type
		{
			UnAllocated = 0,
			Single,
			Double,
			Multiple,
			Socket,
		};
		RecyclableBuffer(Type type = Single);
		~RecyclableBuffer(void);

		void doAllocate();
		void doAllocate( Type type );

	protected:
		Type type;
		MemorySegment* segment;
	};

}