#include "stdafx.h"
#include "MessageFactory.h"

namespace ProtocolFramework
{
	MessageFactory::MessageFactory(void)
	{
	}

	MessageFactory::~MessageFactory(void)
	{
	}

	unsigned int MessageFactory::getMaximumMessageSize()
	{
		return 8024;
	}

	bool ProtocolFramework::MessageFactory::preSerializeMessage( OutgoingPacket& outgoingPacket )
	{
		return true;
	}
}
