#pragma once
#include "ProtocolFramework.h"

namespace ProtocolFramework
{
	struct MemorySegment;

	// BufferPool is used by Push Framework to contains pools of memory buffers according to memory size.
	class PROTOCOLFRAMEWORK_API BufferPool
	{
	public:
		
		BufferPool(void);
		~BufferPool(void);

		bool create(int type, unsigned int nCount, unsigned int nSize);

		MemorySegment* getMemorySegment(int type);

		void returnMemorySegment(MemorySegment* segment, int type);

	private:
		struct Internal;
		Internal* internal;
	};

	extern PROTOCOLFRAMEWORK_API BufferPool pool;
}