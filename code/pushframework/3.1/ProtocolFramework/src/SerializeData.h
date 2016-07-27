#pragma once
#include "ProtocolFramework.h"

namespace ProtocolFramework
{
	class Protocol;
	struct PROTOCOLFRAMEWORK_API SerializeData
	{
		SerializeData(Protocol* pProtocol);
		~SerializeData(void);

		unsigned int nEncodingBytes;
		unsigned int nSerializationBytes;
		Protocol* pProtocol;
	};
}

