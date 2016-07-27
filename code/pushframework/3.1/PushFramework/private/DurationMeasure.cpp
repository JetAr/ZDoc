/********************************************************************
	File :			DurationMeasure.cpp
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
#include "DurationMeasure.h"



namespace PushFramework
{

    DurationMeasure::DurationMeasure( std::string name )
        :Measure(name)
    {
        //
    }
DurationMeasure::~DurationMeasure(void)
{
}

double DurationMeasure::getMean()
{
    double total = 0;
    unsigned int count = 0;
    for (durationVectT::iterator it=durationVect.begin();
            it!=durationVect.end();
            it++)
    {
        total+= *it;
        count++;
    }
    return count == 0 ? 0 : (total / count);
}

double DurationMeasure::getDispersion( double mean )
{
    double temp= 0;
    unsigned int count = 0;
    for (durationVectT::iterator it=durationVect.begin();
            it!=durationVect.end();
            it++)
    {
        temp += pow( (*it) - mean,2);
        count++;
    }
    return count == 0 ? 0 : sqrt(temp/count);
}

std::string DurationMeasure::collectAndReset( std::string timeStamp )
{
    //Calculate Avg and dispersion
    double mean = getMean();
    double dispersion = getDispersion(mean);

    std::stringstream ss;
    ss << std::noskipws;

    ss << "<" << name << " mean=\"" << mean << "\" disp=\"" << dispersion << "\" />";

    std::string retVal = ss.str();


    //Reset
    durationVect.clear();

    return retVal;
}



void DurationMeasure::addObservation( MeasureArgs& args )
{
    DurationMeasureArgs& myArgs = (DurationMeasureArgs&) args;
    durationVect.push_back(myArgs.duration);
}

}
