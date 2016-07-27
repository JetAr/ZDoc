#include "StdAfx.h"
#include "LogicalConnectionPool.h"
#include "ServerImpl.h"
#include "LogicalConnectionImpl.h"
#include "../include/Server.h"
#include "../include/LogicalConnection.h"

namespace PushFramework
{
	LogicalConnectionPool logicalConnectionPool;

	LogicalConnectionPool::LogicalConnectionPool(void)
	{
	}


	LogicalConnectionPool::~LogicalConnectionPool(void)
	{
	}

	void LogicalConnectionPool::deleteImpl( ObjectPtr obj )
	{
		LogicalConnectionImpl* logicalConnection = (LogicalConnectionImpl*) obj;
		delete logicalConnection;
	}

	void LogicalConnectionPool::recycleObject( ObjectPtr obj )
	{
		LogicalConnectionImpl* logicalConnection = (LogicalConnectionImpl*) obj;
		logicalConnection->Recycle();
	}

	AbstractPool::ObjectPtr LogicalConnectionPool::createImpl()
	{
		LogicalConnection* logicalConnection = pServerImpl->getFacade()->createLogicalConnection();
		return &logicalConnection->getImpl();
	}

}
