/********************************************************************
	File :			Utilities.h
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
#ifndef Utilities__INCLUDED
#define Utilities__INCLUDED

#pragma once


#include "Types.h"

namespace PushFramework
{


class Utilities
{
public:
    Utilities(void);
    ~Utilities(void);
    static std::wstring stringBuilder( const wchar_t *fmt, ... );
    static std::string stringBuilderA( const char *fmt, ... );
    static std::wstring getSystemError();
    static std::string getCurrentTime();
    static void getPath(char* path, int len);
    static ThreadIDType getCurrentThreadID();
    static int getProcessorsCount();
};

}

#endif // Utilities__INCLUDED
