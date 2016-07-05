// YUVviewer.h : main header file for the YUVVIEWER application
//

#if !defined(AFX_YUVVIEWER_H__4E0633F3_1519_4B6F_9EF8_C0739CEC5085__INCLUDED_)
#define AFX_YUVVIEWER_H__4E0633F3_1519_4B6F_9EF8_C0739CEC5085__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CYUVviewerApp:
// See YUVviewer.cpp for the implementation of this class
//

class CYUVviewerApp : public CWinApp
{
public:
	CYUVviewerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CYUVviewerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CYUVviewerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YUVVIEWER_H__4E0633F3_1519_4B6F_9EF8_C0739CEC5085__INCLUDED_)
