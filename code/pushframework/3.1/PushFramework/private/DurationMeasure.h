/********************************************************************
	File :			DurationMeasure.h
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
#ifndef DurationMeasure__INCLUDED
#define DurationMeasure__INCLUDED

#pragma once
#include "Measure.h"


namespace PushFramework
{


class DurationMeasureArgs : public MeasureArgs
{
public:
    double duration;
};
class DurationMeasure :
    public Measure
{
public:
    DurationMeasure(std::string name);
    ~DurationMeasure(void);

private:
    virtual void addObservation(MeasureArgs& args);
    virtual std::string collectAndReset(std::string timeStamp);
private:
    typedef std::vector<double> durationVectT;
    durationVectT durationVect;
    //
    double getMean();
    double getDispersion(double mean);
};

}

#endif // DurationMeasure__INCLUDED
