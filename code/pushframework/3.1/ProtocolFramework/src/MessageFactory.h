#pragma once
#include "ProtocolFramework.h"
#include "ErrorCodes.h"

namespace ProtocolFramework
{
	class OutgoingPacket;
	class IncomingPacket;
	class Buffer;
	
	// This class contains the logic of serialization/de-serialization of messages. Messages are serialized to bytes before they
	// are sent to protocol then to network.
	// When protocol decodes received bytes, it extracts bytes related to content and send them here for deserialization.
	// De-serialization will transform the content bytes into a structured object that business code can act upon.
	// Serialization takes a message object as input and produces bytes.
	// If you are planning to use only one protocol, then you can implement this class inside your protocol project.
	// However, if you plan to use many protocols, then implement this at the server level.
	class PROTOCOLFRAMEWORK_API MessageFactory
	{
	public:
		MessageFactory(void);
		~MessageFactory(void);

		virtual SerializeResult::Type serializeMessage(OutgoingPacket& outgoingPacket, Buffer& buffer) = 0;
		virtual DeserializeResult::Type deserializeMessage(Buffer& contentBytes, IncomingPacket*& pMessage, int& nRoutingService) = 0;
		virtual bool preSerializeMessage(OutgoingPacket& outgoingPacket);
		virtual void disposeIncomingPacket(IncomingPacket* pPacket) = 0;
		virtual void disposeOutgoingPacket(OutgoingPacket* pPacket) = 0;
		virtual unsigned int getMaximumMessageSize();

	};
}

