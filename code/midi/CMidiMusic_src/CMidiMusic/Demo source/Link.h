#if !defined(AFX_LINK_H__86243DE1_CD1F_11D5_B47B_00C0DFF9E2AC__INCLUDED_)
#define AFX_LINK_H__86243DE1_CD1F_11D5_B47B_00C0DFF9E2AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Link.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLink window

class CLink : public CStatic
{
// Construction
public:
    CLink();

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CLink)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CLink();

    // Generated message map functions
protected:
    //{{AFX_MSG(CLink)
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINK_H__86243DE1_CD1F_11D5_B47B_00C0DFF9E2AC__INCLUDED_)
