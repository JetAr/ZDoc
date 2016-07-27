/********************************************************************
	File :			SystemService.cpp
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
#include "../include/SystemService.h"

#ifdef Plateform_Windows
#include "SystemServiceImpl_Win.h"
#else
#ifdef Plateform_Linux
#include "SystemServiceImpl_Linux.h"
#endif
#endif

namespace PushFramework
{

SystemService::SystemService( const wchar_t* serviceName )
{
    pImpl = new SystemServiceImpl(serviceName, this);
}
SystemService::~SystemService(void)
{
    delete pImpl;
}

void SystemService::Run()
{
    pImpl->Run();
}

void SystemService::IndicateRunning()
{
    pImpl->IndicateRunning();
}

bool SystemService::Install( const wchar_t* displayName )
{
    return pImpl->Install(displayName);
}

bool SystemService::UnInstall()
{
    return pImpl->UnInstall();
}

}
