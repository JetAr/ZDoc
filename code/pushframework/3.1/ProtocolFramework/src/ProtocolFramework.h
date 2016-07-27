// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PROTOCOLFRAMEWORK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PROTOCOLFRAMEWORK_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.


#include "Plateform.h"


//Export/Import symbols :
#ifdef Plateform_Windows

#ifdef PROTOCOLFRAMEWORK_EXPORTS
#define PROTOCOLFRAMEWORK_API __declspec(dllexport)
#else
#define PROTOCOLFRAMEWORK_API __declspec(dllimport)
#endif


#else

#ifdef Plateform_Linux
#define PROTOCOLFRAMEWORK_API
#endif

#endif