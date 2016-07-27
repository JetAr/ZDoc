/********************************************************************
	File :			DistributionMeasure.h
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
#ifndef DistributionMeasure__INCLUDED
#define DistributionMeasure__INCLUDED

#pragma once
#include "Measure.h"


namespace PushFramework
{


class DistributionMeasureArgs : public MeasureArgs
{
public:
    std::string serviceName;
    double value;
};



class DistributionMeasure :
    public Measure
{
    typedef std::map<std::string, double> mappedValuesT;
public:
    DistributionMeasure(std::string name);
    virtual ~DistributionMeasure(void);

    virtual void addObservation(MeasureArgs& args);
    virtual std::string collectAndReset(std::string timeStamp);
private:
    mappedValuesT mappedValues;
    //

};

}

#endif // DistributionMeasure__INCLUDED
