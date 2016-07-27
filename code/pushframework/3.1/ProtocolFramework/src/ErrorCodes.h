#pragma once
#include "ProtocolFramework.h"

namespace ProtocolFramework
{
	namespace ContextStates
	{
		enum
		{
			InitStarted = 1,
			InitEnded = 2
		};
	}

	namespace EncodeResult
	{
		enum Type
		{
			FatalFailure,
			Failure,
			Success,
			InsufficientBufferSpace,
		};
	}

	namespace SerializeResult
	{
		enum Type
		{
			Failure = 0,
			InsufficientBufferSpace,
			Success
		};
	}

	namespace NetworkSerializeResult
	{
		enum Type
		{
			FatalFailure,
			Failure = 0,
			Retry,
			Success
		};

		PROTOCOLFRAMEWORK_API Type convertEncodingFailure(EncodeResult::Type error);

		PROTOCOLFRAMEWORK_API Type convertSerializeFailure(SerializeResult::Type error);
	}


	namespace DecodeResult
	{
		enum Type
		{
			Close,
			Failure,
			Content,
			ProtocolBytes,
			WantMoreData,
			NoContent,
		};
	}

	namespace DeserializeResult
	{
		enum Type
		{
			Close,
			Failure,
			Success,
			DiscardContent,
		};
	}

	namespace NetworkDeserializeResult
	{
		enum Type
		{
			Initializationfailure,
			Failure = -1,
			Content = 0,
			ProtocolBytes,
			WantMoreData,
			Close,
		};

		PROTOCOLFRAMEWORK_API bool isFailure(Type error);
	}

	
}
