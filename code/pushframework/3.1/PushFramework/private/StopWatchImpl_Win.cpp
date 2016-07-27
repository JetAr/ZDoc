/*
 * StopWatchImpl.cpp
 *
 *  Created on: May 27, 2011
 *      Author: sst
 */

#include "StdAfx.h"
#include "StopWatchImpl_Win.h"

namespace PushFramework
{



StopWatchImpl::~StopWatchImpl()
{

}

StopWatchImpl::StopWatchImpl()
{
    ZeroMemory(&m_QPFrequency, sizeof(m_QPFrequency));
    QueryPerformanceFrequency(&m_QPFrequency);

    ZeroMemory(&m_StartCounter,sizeof(m_StartCounter));
    QueryPerformanceCounter (&m_StartCounter);

    reset();
}

void StopWatchImpl::reset()
{
    ZeroMemory(&m_LastCounter,sizeof(m_LastCounter));
    QueryPerformanceCounter (&m_LastCounter);
}

double StopWatchImpl::getElapsedTime( bool bStart /*= true*/ )
{
    LARGE_INTEGER	curCounter;
    ZeroMemory(&curCounter, sizeof(curCounter));
    QueryPerformanceCounter (&curCounter);
    //
    __int64			m_ElapsedTime=(curCounter.QuadPart  - (bStart ? m_StartCounter.QuadPart :  m_LastCounter.QuadPart));

    if(!bStart)
        reset();

    return (static_cast<double>(m_ElapsedTime) / static_cast<double>(m_QPFrequency.QuadPart));
}




}