/********************************************************************
	File :			QueueOptions.cpp
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
#include "StdAfx.h"
#include "../include/QueueOptions.h"

namespace PushFramework
{
    //

    QueueOptions::QueueOptions()
    {
        requireRegistration = true;
        maxPackets = 100;
        priority = 1;
        packetsQuota = 10;
        fillRateThrottlingPeriod = 0;
        fillRateThrottlingMaxPackets = 0;
		ignorePreviousPackets = false;
		maxExpireDurationSecs = 0;
    }

    QueueOptions::~QueueOptions()
    {
        //
    }

}
