/********************************************************************
	File :			Utilities.cpp
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
#include "Utilities.h"

#include <stdio.h>
#include <string.h>

namespace PushFramework
{


Utilities::Utilities(void)
{
}

Utilities::~Utilities(void)
{
}
std::wstring Utilities::stringBuilder( const wchar_t *fmt, ... )
{
    return L"";
}

std::wstring Utilities::getSystemError()
{
    return L"";
}

std::string Utilities::stringBuilderA( const char *fmt, ... )
{
    va_list args;
    va_start(args, fmt);

    if (!fmt)
        return "";

    int result = -1;
    int length = 1024;
    char *buffer = 0;


    while (result == -1)
    {
        if (buffer)
            delete [] buffer;
        buffer = new char [length + 1];
        ::memset(buffer, 0, (length + 1) * sizeof (char));

        result = vsnprintf(buffer, length, fmt, args);

        if(result == -1)
            return "";
    }
    va_end(args);

    std::string s(buffer);
    delete [] buffer;
    return s;
}

std::string Utilities::getCurrentTime()
{
    char buff[20];
    time_t now = time(NULL);
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buff);
}


void Utilities::getPath(char* path, int len)
{
    char linkname[64]; /* /proc/<pid>/exe */
    pid_t pid;

    /* Get our PID and build the name of the link in /proc */
    pid = getpid();
    snprintf(linkname, sizeof (linkname), "/proc/%i/exe", pid);

    /* Now read the symbolic link */
    int ret = readlink(linkname, path, len);
    /* Ensure proper NUL termination */
    path[ret] = 0;

    for(int i= ret; i>= 0; i--)
    {
        if(path[i] == '/')
        {
            path[i] = 0;
            break;
        }
    }
}
ThreadIDType Utilities::getCurrentThreadID()
{
    return pthread_self();
}
int Utilities::getProcessorsCount()
{
    return sysconf(_SC_NPROCESSORS_ONLN);

}

}

