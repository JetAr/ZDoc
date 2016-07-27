/********************************************************************
	File :			KeyedAveragedDistributionMeasure.h
	Creation date :	2012/01/29

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
#ifndef KeyedAveragedDistributionMeasure__INCLUDED
#define KeyedAveragedDistributionMeasure__INCLUDED

#pragma once

#include "Measure.h"


namespace PushFramework
{

class KeyedAveragedDistributionMeasureArgs : public MeasureArgs
{
public:
    std::string serviceName;
    std::string key;
    double value;
};

class KeyedAveragedDistributionMeasure :
    public Measure
{
    typedef std::map<std::string, double> innerObservationMapT;
    typedef std::map<std::string, innerObservationMapT*> mappedValuesT;

public:
    KeyedAveragedDistributionMeasure(std::string name);
    virtual ~KeyedAveragedDistributionMeasure(void);

    virtual void addObservation( MeasureArgs& args );
    std::string collectAndReset( std::string timeStamp );
private:
    void addInnerObservation(innerObservationMapT* pSegment, std::string key, double value);
    double getMean(innerObservationMapT& segment);
    double getDispersion(innerObservationMapT& segment, double mean );
private:
    mappedValuesT mappedValues;
    //
};

}

#endif // KeyedAveragedDistributionMeasure__INCLUDED
