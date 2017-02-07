// Link.cpp : implementation file
//

#include "stdafx.h"
#include "midiplayer.h"
#include "Link.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLink

CLink::CLink()
{
}

CLink::~CLink()
{
}


BEGIN_MESSAGE_MAP(CLink, CStatic)
    //{{AFX_MSG_MAP(CLink)
    ON_WM_CTLCOLOR_REFLECT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLink message handlers

HBRUSH CLink::CtlColor(CDC* pDC, UINT nCtlColor)
{
    // TODO: Change any attributes of the DC here
    pDC->SetTextColor(RGB(0,0,255));
    // TODO: Return a non-NULL brush if the parent's handler should not be called
    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH)GetStockObject(NULL_BRUSH);
}

