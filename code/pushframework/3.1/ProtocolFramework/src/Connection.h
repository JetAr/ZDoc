#pragma once
#include "ProtocolFramework.h"

namespace ProtocolFramework
{
	class Protocol;
	class ProtocolContext;
	class MessageFactory;
	class Buffer;
	
	// This class is here to allow for the symmetric use of code (Client-side and server-side).
	// In other terms, Connection class should have been an internal class of Push Framework.
	// Client-side implementation (See TCPSocket project in demos) use this class.
	class PROTOCOLFRAMEWORK_API Connection
	{
	public:
		Connection(void);
		~Connection(void);

		bool SetUpProtocolContexts();
		ProtocolContext& getProtocolContext(Protocol* pProtocol);
		Protocol* AdvanceInitialization(Buffer& outgoingBytes);
		void CheckConnectionInitialization();
		void Recycle();
	public:
		virtual Protocol* getProtocol() = 0;
		virtual MessageFactory& getMessageFactory() = 0;
	private:
		struct Internal;
		Internal* internal;
		//
	protected:
		virtual void InitializeConnection() = 0;
	};

}
