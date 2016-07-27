#include "StdAfx.h"
#include "MonitorConnection.h"

namespace PushFramework
{

	MonitorConnection::MonitorConnection(const string& str)
		: LogicalConnectionImpl(NULL)
	{
		monitorKey = str;
	}


	MonitorConnection::~MonitorConnection(void)
	{
	}

	const char* MonitorConnection::getKey()
	{
		return monitorKey.c_str();
	}

	bool MonitorConnection::IsMonitor()
	{
		return true;
	}
}
