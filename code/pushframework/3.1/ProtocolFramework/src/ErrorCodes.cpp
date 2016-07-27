#include "stdafx.h"
#include "ErrorCodes.h"




ProtocolFramework::NetworkSerializeResult::Type ProtocolFramework::NetworkSerializeResult::convertEncodingFailure( EncodeResult::Type error )
{
	if(error == EncodeResult::FatalFailure)
		return FatalFailure;
	else if (error == EncodeResult::Failure)
		return Failure;
	else if (error == EncodeResult::InsufficientBufferSpace)
		return Retry;
	else
	{
		//Impossible.
		return Failure;
	}
}

ProtocolFramework::NetworkSerializeResult::Type ProtocolFramework::NetworkSerializeResult::convertSerializeFailure( SerializeResult::Type error )
{
	if(error == SerializeResult::Failure)
		return Failure;
	else if (error == SerializeResult::InsufficientBufferSpace)
		return Retry;
	else
	{
		//Impossible.
		return Failure;
	}
}

bool ProtocolFramework::NetworkDeserializeResult::isFailure( Type error )
{
	return error == Failure || error == Initializationfailure;
}
