#include "StdAfx.h"
#include "ConnectionContextPool.h"
#include "../include/ConnectionContext.h"
#include "../include/Server.h"
#include "ServerImpl.h"

namespace PushFramework
{
	ConnectionContextPool connectionContextPool;

	//
	ConnectionContextPool::ConnectionContextPool(void)
	{
	}


	ConnectionContextPool::~ConnectionContextPool(void)
	{
	}

	void ConnectionContextPool::deleteImpl( ObjectPtr obj )
	{
		ConnectionContext* context = (ConnectionContext*) obj;
		delete context;
	}

	AbstractPool::ObjectPtr ConnectionContextPool::createImpl()
	{
		return pServerImpl->getFacade()->createConnectionContext();
	}

	void ConnectionContextPool::recycleObject( ObjectPtr obj )
	{
		ConnectionContext* context = (ConnectionContext*) obj;
		context->Recycle();
	}

}
