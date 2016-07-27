#include "StdAfx.h"
#include "StopWatch.h"

#ifdef Plateform_Windows
#include "StopWatchImpl_Win.h"
#else
#ifdef Plateform_Linux
#include "StopWatchImpl_Linux.h"
#endif
#endif


namespace PushFramework
{

StopWatch::StopWatch()
{
    pImpl = new StopWatchImpl;
}

StopWatch::~StopWatch(  )
{
	delete pImpl;
}

void StopWatch::reset()
{
    pImpl->reset();
}

double StopWatch::getElapsedTime( bool bStart /*= true*/ )
{
    return pImpl->getElapsedTime();
}
}
