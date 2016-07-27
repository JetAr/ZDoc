/********************************************************************
	File :			StopWatchImpl_Linux.cpp
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
#include "StdAfx.h"
#include "StopWatchImpl_Linux.h"


namespace PushFramework
{



StopWatchImpl::~StopWatchImpl()
{
    // TODO Auto-generated destructor stub
}

StopWatchImpl::StopWatchImpl()
{
    gettimeofday(&tvBegin, NULL);
}

void StopWatchImpl::reset()
{

}

double StopWatchImpl::getElapsedTime( bool bStart /*= true*/ )
{
    timeval tvEnd, tvDiff;
    gettimeofday(&tvEnd, NULL);

    timeval_subtract(&tvDiff, &tvEnd, &tvBegin);


    double sec = tvDiff.tv_sec;
    double usec = tvDiff.tv_usec;

    return sec + 1000000 / usec;
}

int StopWatchImpl::timeval_subtract( struct timeval *result, struct timeval *t2, struct timeval *t1 )
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff < 0);
}
}
