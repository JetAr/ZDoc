
/************************************************************************
 *
 *  Ye-Kui Wang       wyk@ieee.org
 *  Juan-Juan Jiang   juanjuan_j@hotmail.com
 *  
 *  March 14, 2002
 *
 ************************************************************************/

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any
 * license fee or royalty on an "as is" basis.  The developers disclaim 
 * any and all warranties, whether express, implied, or statuary, including 
 * any implied warranties or merchantability or of fitness for a particular 
 * purpose.  In no event shall the copyright-holder be liable for any incidental,
 * punitive, or consequential damages of any kind whatsoever arising from 
 * the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs
 * and user's customers, employees, agents, transferees, successors,
 * and assigns.
 *
 * The developers does not represent or warrant that the programs furnished 
 * hereunder are free of infringement of any third-party patents.
 *
 * */



#if !defined(AFX_CHILDWINDOW_H__964DA0F6_4CD2_11D4_B1E9_0000B43F34EC__INCLUDED_)
#define AFX_CHILDWINDOW_H__964DA0F6_4CD2_11D4_B1E9_0000B43F34EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChildWindow.h : header file
//

#include "Convert.h"

/////////////////////////////////////////////////////////////////////////////
// CChildWindow frame

class CChildWindow : public CFrameWnd
{
	DECLARE_DYNCREATE(CChildWindow)
protected:
	CChildWindow();           // protected constructor used by dynamic creation

private:
	BOOL bColorImage;
	int iWidth,iHeight;
	int m_nzoom;

	HANDLE hloc;
	LPBITMAPINFO BmpInfo;

	ColorSpaceConversions conv;

public: 

	int nPicShowOrder;
  char *inSeqName;
  int m_iCount;

	LPBYTE Y,Cb,Cr,RGBbuf;
	CChildWindow(CFrameWnd* pParentWnd,int Width,int Height, BOOL bColor);
    ~CChildWindow();

	void ShowImage(CDC *dc,BYTE  *lpImage);
	void ShowGrayImage(CDC *dc,BYTE  *lpImage);

	void CenterWindow(int width,int height);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildWindow)
	//}}AFX_VIRTUAL


	// Generated message map functions
	//{{AFX_MSG(CChildWindow)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDWINDOW_H__964DA0F6_4CD2_11D4_B1E9_0000B43F34EC__INCLUDED_)
