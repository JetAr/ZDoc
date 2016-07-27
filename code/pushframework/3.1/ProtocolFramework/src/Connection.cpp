#include "stdafx.h"
#include "Connection.h"
#include "Protocol.h"
#include "ProtocolContext.h"

namespace ProtocolFramework
{

	typedef unordered_map<Protocol*, ProtocolContext*> protocolContextMapT;
	struct Connection::Internal
	{
		Internal()
		{
			isConnectionInitialized = false;
		}
		~Internal()
		{
			//
		}
		protocolContextMapT protocolContextMap;
		bool isConnectionInitialized;
	};
	
	Connection::Connection(void)
	{
		internal = new Internal();
	}
	
	Connection::~Connection(void)
	{
		delete internal;
	}

	bool Connection::SetUpProtocolContexts()
	{
		Protocol* pProtocol = getProtocol();

		while (pProtocol)
		{
			ProtocolContext* pContext = (ProtocolContext*) pProtocol->borrowObject();
			if (!pContext)
			{
				return false;
			}

			internal->protocolContextMap[pProtocol] = pContext;

			pProtocol = pProtocol->getLowerProtocol();
		}

		return true;
	}

	ProtocolContext& Connection::getProtocolContext( Protocol* pProtocol )
	{
		protocolContextMapT::iterator it  = internal->protocolContextMap.find(pProtocol);
		return *(it->second);
	}

	Protocol* Connection::AdvanceInitialization( Buffer& outgoingBytes )
	{
		Protocol* pCurrentProtocol = getProtocol()->getLowestProtocol();

		while (pCurrentProtocol)
		{
			ProtocolContext& context = getProtocolContext(pCurrentProtocol);

			if (!context.isInitializationStarted())
			{
				context.setInitializationStarted();
				pCurrentProtocol->startSession(context, outgoingBytes);
				if (!outgoingBytes.isEmpty())
				{
					return pCurrentProtocol;
				}
			}

			if (!context.isInitialized())
			{
				return NULL;
			}

			pCurrentProtocol = pCurrentProtocol->getUpperProtocol();
		}
		
		return NULL;
	}

	void Connection::CheckConnectionInitialization()
	{
		if (internal->isConnectionInitialized)
		{
			return;
		}

		ProtocolContext& context = getProtocolContext(getProtocol());

		if (!context.isInitialized())
		{
			return;
		}

		internal->isConnectionInitialized = true;
		//

		InitializeConnection();
	}

	void Connection::Recycle()
	{
		internal->isConnectionInitialized = false;
		protocolContextMapT::iterator it = internal->protocolContextMap.begin();

		while (it != internal->protocolContextMap.end())
		{
			Protocol* protocol = it->first;
			ProtocolContext* context = it->second;

			protocol->returnObject(context);

			it++;
		}

		internal->protocolContextMap.clear();
	}

}
