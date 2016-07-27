#pragma once
#include "ProtocolFramework.h"
#include "RecyclableBuffer.h"

namespace ProtocolFramework
{
	class Protocol;
	class IncomingPacket;

	struct PROTOCOLFRAMEWORK_API DeserializeData
	{
		DeserializeData(Protocol* pProtocol);
		~DeserializeData(void);

		void clear();
		Protocol* pProtocol;
		IncomingPacket* pMessage;
		int nRoutingService;
		RecyclableBuffer protocolBytes;
	};
}
