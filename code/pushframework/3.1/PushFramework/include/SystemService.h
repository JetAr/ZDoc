/********************************************************************
	File :			SystemService.h
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


*********************************************************************/#ifndef SystemService__INCLUDED
#define SystemService__INCLUDED

#pragma once

#include "PushFramework.h"

namespace PushFramework
{


class SystemServiceImpl;

class PUSHFRAMEWORK_API SystemService
{
    friend class SystemServiceImpl;
public:
    SystemService(const wchar_t* serviceName);
    virtual ~SystemService(void);

    void Run();
    void IndicateRunning();
    bool Install(const wchar_t* displayName);
    bool UnInstall();

protected:
    virtual void OnStart()= 0;
    virtual void OnStop() = 0;
    virtual bool OnPause()
    {
        return false;
    }
    virtual bool OnResume()
    {
        return false;
    }
private:
    SystemServiceImpl* pImpl;
};

}
#endif // SystemService__INCLUDED
