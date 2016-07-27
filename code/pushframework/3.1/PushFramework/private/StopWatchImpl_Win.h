/*
 * StopWatchImpl.h
 *
 *  Created on: May 27, 2011
 *      Author: sst
 */

#ifndef STOPWATCHIMPL_H_
#define STOPWATCHIMPL_H_

namespace PushFramework
{

class StopWatchImpl
{
public:
    virtual ~StopWatchImpl();


    StopWatchImpl();

    void reset();
    double getElapsedTime(bool bStart = true);
private:
    LARGE_INTEGER	m_QPFrequency;
    LARGE_INTEGER	m_StartCounter;
    LARGE_INTEGER	m_LastCounter;
};

}
#endif /* STOPWATCHIMPL_H_ */
