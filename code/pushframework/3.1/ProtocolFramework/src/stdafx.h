// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include "Plateform.h"

#ifdef Plateform_Windows

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#endif


#include <stdio.h>

#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string.h>

#include <map>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
using namespace std;


#ifdef Plateform_Linux
#include "linuxcs.h"
#endif