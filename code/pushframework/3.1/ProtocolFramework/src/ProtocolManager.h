#pragma once
#include "ProtocolFramework.h"
#include "ErrorCodes.h"

namespace ProtocolFramework
{

	struct DeserializeData;
	struct SerializeData;
	class Connection;
	class OutgoingPacket;
	class Buffer;

	// This class is here to allow for the symmetric use of code (Client-side and server-side).
	// In other terms, ProtocolManager class should have been an internal class of Push Framework.
	// Client-side implementation (See TCPSocket project in demos) use this class.
	class PROTOCOLFRAMEWORK_API ProtocolManager
	{
	public:
		ProtocolManager(void);
		~ProtocolManager(void);

		NetworkSerializeResult::Type serializeOutgoingPacket(Connection& connection, OutgoingPacket& outgoingMsg, Buffer& outputBuffer, SerializeData& serializeData);
		NetworkSerializeResult::Type serializeOutgoinBytes(Connection& connection, Buffer& outgoingBytes, Buffer& outputBuffer, SerializeData& serializeData);

		NetworkDeserializeResult::Type tryDeserializeIncomingPacket(Connection& connection, DeserializeData& deserializeData);

		bool processIncomingData(Connection& connection, Buffer& incomingBytes);
	};
}
