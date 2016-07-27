#pragma once
#include "ProtocolFramework.h"

namespace ProtocolFramework
{
	//Pooling is a necessity in real time servers: memory allocation and de-allocation takes too much time.
	//AbstractPool contains the logic of recycling objects allocated at start-up.
	//ProtocolFramework and PushFramework use this in many places: memory buffers, connections, messages, etc.
	class PROTOCOLFRAMEWORK_API AbstractPool
	{
	public:
		typedef void* ObjectPtr;

		AbstractPool();

		~AbstractPool(void);

		bool initialize(unsigned int nRequiredObjects);

		ObjectPtr borrowObject();

		void returnObject(ObjectPtr p);

	private:
		struct Internal;
		Internal* internal;

	protected:
		virtual void deleteImpl(ObjectPtr obj) = 0;
		virtual ObjectPtr createImpl() = 0;
		virtual void recycleObject(ObjectPtr p) = 0;
	};
}

