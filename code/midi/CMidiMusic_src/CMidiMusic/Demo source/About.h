#if !defined(AFX_ABOUT_H__DAFE6F44_BFF8_11D5_B47A_00C0DFF9E2AC__INCLUDED_)
#define AFX_ABOUT_H__DAFE6F44_BFF8_11D5_B47A_00C0DFF9E2AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Link.h"
// About.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAbout dialog

class CAbout : public CDialog
{
// Construction
public:
    CAbout(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CAbout)
    enum { IDD = IDD_ABOUT };
    CLink	m_Link;
    CStatic	m_Image;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAbout)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CAbout)
    virtual BOOL OnInitDialog();
    afx_msg void OnStaticLink();
    afx_msg void OnClose();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUT_H__DAFE6F44_BFF8_11D5_B47A_00C0DFF9E2AC__INCLUDED_)
