// ffplay_vc2005.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cffplay_vc2005App:
// See ffplay_vc2005.cpp for the implementation of this class
//

class Cffplay_vc2005App : public CWinApp
{
public:
	Cffplay_vc2005App();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cffplay_vc2005App theApp;