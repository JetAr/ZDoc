/********************************************************************
	File :			DistributionMeasure.cpp
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
#include "DistributionMeasure.h"

namespace PushFramework
{


DistributionMeasure::DistributionMeasure(std::string name)
    :Measure(name)
{
}

DistributionMeasure::~DistributionMeasure(void)
{
}

std::string DistributionMeasure::collectAndReset( std::string timeStamp )
{
    //Collect
    std::stringstream ss;
    ss << std::noskipws;

    ss << "<" << name << ">";

    for (mappedValuesT::iterator it = mappedValues.begin();
            it!=mappedValues.end();
            it++)
    {
        std::string serviceName = it->first;
        double value = it->second;

        ss << "<" << serviceName << " val=\"" << value << "\"/>";
    }

    ss << "</" << name << ">";

    std::string retVal = ss.str();


    //Reset
    mappedValues.clear();

    //
    return retVal;
}

void DistributionMeasure::addObservation( MeasureArgs& args )
{
    DistributionMeasureArgs& myArgs = (DistributionMeasureArgs&) args;
    if (mappedValues.find(myArgs.serviceName) == mappedValues.end())
    {
        mappedValues[myArgs.serviceName] = myArgs.value;
    }
    else
        mappedValues[myArgs.serviceName] += myArgs.value;
}

}
