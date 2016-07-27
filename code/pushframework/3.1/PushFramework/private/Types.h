

#include "../include/Plateform.h"


#ifdef Plateform_Windows
typedef DWORD ThreadIDType;
typedef HANDLE ThreadHandleType;
#else

#ifdef Plateform_Linux
typedef pthread_t ThreadIDType;
typedef pthread_t ThreadHandleType;
#endif

#endif


#ifdef Plateform_Linux
#ifndef SOCKET
typedef int SOCKET;
#endif

#ifndef SOCKADDR_IN
typedef sockaddr_in SOCKADDR_IN;
#endif

#ifndef IN_ADDR
typedef in_addr IN_ADDR;
#endif

#ifndef UINT
typedef unsigned int UINT;
#endif

#define Sleep(x) usleep(x * 1000)
#endif

