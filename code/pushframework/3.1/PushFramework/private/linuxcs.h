/********************************************************************************
*                                                                               *
* linuxcs.h - BWY Systems Linux Critical Section Library v1.0.0                 *
*                                                                               *
* This library provides a Win32 like critical section using pthread             *
* mutexes.                                                                      *
*                                                                               *
* Copyright (C) 2007-2010 BWY Systems.                                          *
*                                                                               *
* Permission is hereby granted, free of charge, to any person obtaining a copy  *
* of this software and associated documentation files (the "Software"), to deal *
* in the Software without restriction, including without limitation the rights  *
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell     *
* copies of the Software, and to permit persons to whom the Software is         *
* furnished to do so, subject to the following conditions:                      *
*                                                                               *
* The above copyright notice and this permission notice shall be included in    *
* all copies or substantial portions of the Software.                           *
*                                                                               *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER        *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, *
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN     *
* THE SOFTWARE.                                                                 *
********************************************************************************/
#ifdef WIN32
#define _LINUX_CRITICAL_SECTION_
#endif

#ifndef _LINUX_CRITICAL_SECTION_
#define _LINUX_CRITICAL_SECTION_

#include <pthread.h>


#ifndef BOOL
typedef int BOOL;
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

typedef pthread_mutex_t	CRITICAL_SECTION;
typedef CRITICAL_SECTION *LPCRITICAL_SECTION;

BOOL EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
BOOL TryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
BOOL LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
BOOL InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
BOOL DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);



#endif
