/********************************************************************
	File :			ServerOptions.h
	Creation date :	2010/6/27

	License :			Copyright 2010 Ahmed Charfeddine, http://www.pushframework.com

				   Licensed under the Apache License, Version 2.0 (the "License");
				   you may not use this file except in compliance with the License.
				   You may obtain a copy of the License at

					   http://www.apache.org/licenses/LICENSE-2.0

				   Unless required by applicable law or agreed to in writing, software
				   distributed under the License is distributed on an "AS IS" BASIS,
				   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
				   See the License for the specific language governing permissions and
				   limitations under the License.


*********************************************************************/
#ifndef ServerOptions__INCLUDED
#define ServerOptions__INCLUDED

#pragma once
#include "PushFramework.h"

namespace PushFramework
{

class PUSHFRAMEWORK_API ServerOptions
{
public:
	ServerOptions(void);
	~ServerOptions(void);

	unsigned int nMaxConnections;
	unsigned int uLoginExpireDuration;
	bool challengeClients;
	unsigned int nWorkersCount;
	unsigned int nStreamers;
	bool isProfilingEnabled;
	unsigned int samplingRate;
	bool isMonitorEnabled;
	short monitorPort;
	char password[256];
	unsigned int uMaxClientIdleTime;
	//
	unsigned int nSocketBufferSize;
	unsigned int nMaxPendingOutgoingMessages;
};

extern PUSHFRAMEWORK_API ServerOptions options;
}
#endif // ServerOptions__INCLUDED
