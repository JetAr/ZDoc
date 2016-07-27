/********************************************************************
	File :			QueueOptions.h
	Creation date :	2012/07/13
		
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
#ifndef QueueOptions__INCLUDED
#define QueueOptions__INCLUDED

#pragma once
#include "PushFramework.h"

namespace PushFramework
{
    struct PUSHFRAMEWORK_API QueueOptions
    {
        QueueOptions();
        ~QueueOptions();
        bool requireRegistration;
        unsigned int maxPackets;
        unsigned int priority;
        unsigned int packetsQuota;
        unsigned int fillRateThrottlingPeriod;
        unsigned int fillRateThrottlingMaxPackets;
		bool ignorePreviousPackets;
		unsigned int maxExpireDurationSecs;
    };
}
#endif // QueueOptions__INCLUDED
