// MidiPlayer.h : main header file for the MIDIPLAYER application
//

#if !defined(AFX_MIDIPLAYER_H__6F5E7645_AF4A_11D5_B47A_000001191531__INCLUDED_)
#define AFX_MIDIPLAYER_H__6F5E7645_AF4A_11D5_B47A_000001191531__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMidiPlayerApp:
// See MidiPlayer.cpp for the implementation of this class
//

class CMidiPlayerApp : public CWinApp
{
public:
    CMidiPlayerApp();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMidiPlayerApp)
public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

// Implementation

    //{{AFX_MSG(CMidiPlayerApp)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDIPLAYER_H__6F5E7645_AF4A_11D5_B47A_000001191531__INCLUDED_)
