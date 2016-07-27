#include "stdafx.h"
#include "ProtocolContext.h"
#include "ErrorCodes.h"


namespace ProtocolFramework
{

	ProtocolContext::ProtocolContext( void ):
		receivedData(RecyclableBuffer::Double)
	{
		state = 0;
	}

	ProtocolContext::~ProtocolContext( void )
	{
		//
	}

	bool ProtocolContext::isInitialized()
	{
		return (state & ContextStates::InitEnded) != 0;
	}

	bool ProtocolContext::isInitializationStarted()
	{
		return state & ContextStates::InitStarted;
	}

	void ProtocolContext::setInitialized()
	{
		state |= ContextStates::InitEnded;
	}

	Buffer& ProtocolContext::getDataBuffer()
	{
		return receivedData;
	}

	void ProtocolContext::setInitializationStarted()
	{
		state |= ContextStates::InitStarted;
	}

	void ProtocolContext::recycle()
	{
		//
	}

	void ProtocolContext::recycleIntern()
	{
		receivedData.clearBytes();
		state = 0;
		recycle();
	}

}
