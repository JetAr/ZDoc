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
    va_list args;
    va_start(args, fmt);

    if (!fmt) return L"";
    int   result = -1, length = 1024;
    wchar_t *buffer = 0;
    while (result == -1)
    {
        if (buffer)
            delete [] buffer;
        buffer = new wchar_t [length + 1];
        memset(buffer, 0, (length + 1)* sizeof(wchar_t));

        result = _vsnwprintf_s(buffer, length, _TRUNCATE, fmt, args);
        length *= 2;
    }
    va_end(args);

    std::wstring s(buffer);
    delete [] buffer;
    return s;
}

std::wstring Utilities::getSystemError()
{
    DWORD dwIOError = GetLastError();

    LPVOID lpMsgBuf;

    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwIOError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR) &lpMsgBuf,
        0, NULL );

    std::wstring str( (LPCWSTR) lpMsgBuf);
    return str;
}

std::string Utilities::stringBuilderA( const char *fmt, ... )
{
    va_list args;
    va_start(args, fmt);

    if (!fmt) return "";
    int   result = -1, length = 1024;
    char *buffer = 0;
    while (result == -1)
    {
        if (buffer)
            delete [] buffer;
        buffer = new char [length + 1];
        memset(buffer, 0, (length + 1)* sizeof(char));

        result = _vsnprintf_s(buffer, length, _TRUNCATE, fmt, args);
    }
    va_end(args);

    std::string s(buffer);
    delete [] buffer;
    return s;
}

std::string Utilities::getCurrentTime()
{
    char buff[20];
	
	struct tm result;
    time_t now = time(NULL);
	localtime_s(&result, &now);
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", &result);
    return string(buff);
}

void Utilities::getPath(char* path, int len)
{
    char szPath[MAX_PATH];
    if( !GetModuleFileNameA( NULL, szPath, MAX_PATH ) )
        return;
    std::string strPath(szPath);
    std::string::size_type pos1 = strPath.rfind('\\');
    std::string::size_type pos2 = strPath.rfind('/');
    std::string::size_type pos = (pos1==-1) ? pos2 : (pos2==-1) ? pos1 : (max(pos1, pos2));
    szPath[pos] = '\0';

    strcpy_s(path, len, szPath);
}
ThreadIDType Utilities::getCurrentThreadID()
{
    return ::GetCurrentThreadId();
}
int Utilities::getProcessorsCount()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );
    return sysinfo.dwNumberOfProcessors;
}

}

