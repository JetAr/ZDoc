#include "stdafx.h"
#include "Protocol.h"
#include "Buffer.h"
#include "ErrorCodes.h"
#include "ProtocolContext.h"

namespace ProtocolFramework
{
	
	Protocol::Protocol( void )
	{
		pUpperProtocol = NULL;
		pLowerProtocol = NULL;
	}

	Protocol::~Protocol( void )
	{
		//
	}

	Protocol* Protocol::getLowerProtocol()
	{
		return pLowerProtocol;
	}

	Protocol* Protocol::getUpperProtocol()
	{
		return pLowerProtocol;
	}

	Protocol* Protocol::getLowestProtocol()
	{
		if (pLowerProtocol)
		{
			return getLowestProtocol();
		}

		return this;
	}

	void Protocol::addLowerProtocolLayer( Protocol* pProtocol )
	{
		pLowerProtocol = pProtocol;
		pProtocol->pUpperProtocol = this;
	}

	unsigned int Protocol::getRequiredOutputSize( unsigned int maxInputSize )
	{
		return maxInputSize;
	}

	void Protocol::deleteImpl( ObjectPtr obj )
	{
		ProtocolContext* context = (ProtocolContext*) obj;
		delete context;
	}

	AbstractPool::ObjectPtr Protocol::createImpl()
	{
		return createNewProtocolContext();
	}

	void Protocol::recycleObject( ObjectPtr p )
	{
		ProtocolContext* context = (ProtocolContext*) p;
		context->recycleIntern();
	}

	ProtocolContext* Protocol::createNewProtocolContext()
	{
		return new ProtocolContext();
	}

	unsigned int Protocol::getRequiredRecyclableBuffers( unsigned int nMaxConnections, unsigned int nMaxConcurrentCalls )
	{
		return 0;
	}

}

