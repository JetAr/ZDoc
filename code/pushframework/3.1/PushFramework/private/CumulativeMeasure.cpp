/********************************************************************
	File :			CumulativeMeasure.cpp
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
#include "CumulativeMeasure.h"



namespace PushFramework
{


CumulMeasure::CumulMeasure( std::string name, bool bIsContinous/*=false*/ )
    :Measure(name)
{
    this->bIsContinous = bIsContinous;
    cumulValue = 0;
}

CumulMeasure::~CumulMeasure( void )
{
    //
}

void CumulMeasure::addObservation( MeasureArgs& args )
{
    CumulMeasureArgs& myArgs = (CumulMeasureArgs&) args;
    cumulValue += myArgs.dwValue;
}

std::string CumulMeasure::collectAndReset( std::string timeStamp )
{
    //Collect
    std::stringstream ss;
    ss << std::noskipws;

    ss << "<" << name << " val=\"" << cumulValue << "\"/>";

    std::string retVal = ss.str();


    //Reset to zero if not continuous.
    if (!bIsContinous)
        cumulValue = 0;
    //
    return retVal;
}

}
