/********************************************************************
	File :			KeyedAveragedDistributionMeasure.cpp
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
#include "StdAfx.h"
#include "KeyedAveragedDistributionMeasure.h"

namespace PushFramework
{

double KeyedAveragedDistributionMeasure::getDispersion( innerObservationMapT& segment, double mean )
{
    double temp= 0;
    unsigned int count = 0;
    for (innerObservationMapT::iterator it=segment.begin();
            it!=segment.end();
            it++)
    {
        temp += pow(it->second - mean, 2);
        count++;
    }
    return count == 0 ? 0 : sqrt(temp/count);
}

double KeyedAveragedDistributionMeasure::getMean( innerObservationMapT& segment )
{
    double total = 0;
    unsigned int count = 0;
    for (innerObservationMapT::iterator it=segment.begin();
            it!=segment.end();
            it++)
    {
        total+= it->second;
        count++;
    }
    return count == 0 ? 0 : (total / count);
}

void KeyedAveragedDistributionMeasure::addInnerObservation( innerObservationMapT* pSegment, std::string key, double value )
{
    innerObservationMapT::iterator it = pSegment->find(key);
    if (it == pSegment->end())
    {
        (*pSegment)[key] = value;
    }
    else
        it->second += value;
}

void KeyedAveragedDistributionMeasure::addObservation( MeasureArgs& args )
{
    KeyedAveragedDistributionMeasureArgs& myArgs = (KeyedAveragedDistributionMeasureArgs&) args;

    mappedValuesT::iterator it = mappedValues.find(myArgs.serviceName);

    innerObservationMapT* pInnerObservation = NULL;

    if ( it == mappedValues.end())
    {
        pInnerObservation = new innerObservationMapT;
        mappedValues[myArgs.serviceName] = pInnerObservation;
    }
    else
        pInnerObservation = it->second;

    addInnerObservation(pInnerObservation, myArgs.key, myArgs.value);
}

KeyedAveragedDistributionMeasure::KeyedAveragedDistributionMeasure( std::string name ) :Measure(name)
{
}

std::string KeyedAveragedDistributionMeasure::collectAndReset( std::string timeStamp )
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
        innerObservationMapT* pSegment = it->second;

        double mean = getMean(*pSegment);
        double dispersion = getDispersion(*pSegment, mean);

        ss << "<" << serviceName << " mean=\"" << mean << "\" disp=\"" << dispersion << "\"/>";
    }

    ss << "</" << name << ">";

    std::string retVal = ss.str();

    //Reset
    while (!mappedValues.empty())
    {
        mappedValuesT::iterator it = mappedValues.begin();
        innerObservationMapT* pInnerValues = it->second;
        delete pInnerValues;
        mappedValues.erase(it);
    }


    //
    return retVal;
}

KeyedAveragedDistributionMeasure::~KeyedAveragedDistributionMeasure( void )
{
	while (!mappedValues.empty())
	{
		mappedValuesT::iterator it = mappedValues.begin();
		innerObservationMapT* pInnerValues = it->second;
		delete pInnerValues;
		mappedValues.erase(it);
	}
}

}
