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


// ChildWindow.cpp : implementation file
//

#include "stdafx.h"
#include "YUVviewer.h"
#include "ChildWindow.h"
#include "YUVviewerDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildWindow

IMPLEMENT_DYNCREATE(CChildWindow, CFrameWnd)

CChildWindow::CChildWindow()
{
}
CChildWindow::~CChildWindow()
{
	GlobalUnlock(hloc);
	GlobalFree(hloc);  
	
	free(RGBbuf);
	free(Y);
	free(Cb);
	free(Cr);
}

CChildWindow::CChildWindow( CFrameWnd *pParentWnd,int Width,int Height, BOOL bColor)
{    
	iWidth=Width;iHeight=Height;bColorImage=bColor;

  m_iCount = ((CYUVviewerDlg *)pParentWnd)->m_iCount;
  inSeqName = ((CYUVviewerDlg *)pParentWnd)->inSeqName[m_iCount];

	if(((CYUVviewerDlg *)pParentWnd)->m_nZoom == -1)
		m_nzoom = 1;
	else if(((CYUVviewerDlg *)pParentWnd)->m_nZoom == 0)
		m_nzoom = 2;
	
	nPicShowOrder=0;
	
	if (NULL== (Y = (unsigned char *)malloc(iWidth*iHeight) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}
	if (NULL== (Cb = (unsigned char *)malloc(iWidth*iHeight/4) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}
	if (NULL== (Cr = (unsigned char *)malloc(iWidth*iHeight/4) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}
	if (NULL== (RGBbuf = (unsigned char *)malloc(iWidth*iHeight*3) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}
	
	DWORD AttrStyle;
    
	hloc = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
   		sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));
	BmpInfo = (LPBITMAPINFO) GlobalLock(hloc);

	
	AttrStyle = //WS_OVERLAPPEDWINDOW;
		WS_OVERLAPPED|WS_CAPTION|WS_THICKFRAME|WS_MAXIMIZEBOX|WS_MINIMIZEBOX;
    Create(NULL,NULL,AttrStyle,rectDefault,pParentWnd);
}

void CChildWindow::ShowGrayImage(CDC *pDC,BYTE *lpImage)
{
	int i;         
	int nNum1,nNum2;
	HANDLE hMem;
	BYTE *lpBuf;

	BmpInfo->bmiHeader.biBitCount = 8;

    hMem=GlobalAlloc(GHND,iWidth*iHeight);
	lpBuf=(BYTE *)GlobalLock(hMem);
	
    //将倒置的图象放正 
	//Make the inverse image normal
    for(i=0;i<iHeight;i++){
		nNum1=(iHeight-i-1)*iWidth;
		nNum2=i*iWidth;
		memcpy(lpBuf+nNum1,lpImage+nNum2,iWidth);
	}

	pDC->SetStretchBltMode(STRETCH_DELETESCANS);
	StretchDIBits(pDC->m_hDC,0,0,iWidth,iHeight,
							 0,0,iWidth,iHeight,
					  lpBuf,BmpInfo, DIB_RGB_COLORS,SRCCOPY);  

	GlobalUnlock(hMem);
	GlobalFree(hMem);
}

void CChildWindow::ShowImage(CDC *pDC,BYTE *lpImage)
{
	BmpInfo->bmiHeader.biBitCount = 24;

	pDC->SetStretchBltMode(STRETCH_DELETESCANS);
	StretchDIBits(pDC->m_hDC,0,0,m_nzoom*iWidth,m_nzoom*iHeight,
							 0,0,iWidth,iHeight,
					  lpImage, BmpInfo, DIB_RGB_COLORS,SRCCOPY);
}

void CChildWindow::CenterWindow(int width,int height)
{
    RECT rc;
    RECT rw;
    
    int cyBorder, cxBorder;
    int cyTotal,cxTotal;
    int cyMenuAndCaption;

    int cw, ch;

  	RECT r;
	  int nCx=GetSystemMetrics(SM_CXSCREEN),nCy=GetSystemMetrics(SM_CYSCREEN);

    cyBorder = GetSystemMetrics(SM_CYBORDER);
    cxBorder = GetSystemMetrics(SM_CXBORDER);

    // Figure out the height of the menu, toolbar, and caption
    GetWindowRect(&rw);
    GetClientRect(&rc);

    ClientToScreen ((LPPOINT) &rc);
    cyMenuAndCaption = (rc.top - rw.top) ;
  
    cyTotal =height  +
              cyMenuAndCaption +
              cyBorder * 2 ;
    cxTotal=width+cxBorder * 2;            

	r.left=(nCx-cxTotal)/2;r.top=(nCy-cyTotal)/2;
	r.right=(nCx+cxTotal)/2;r.bottom=(nCy+cyTotal)/2;
	
	MoveWindow(&r);//,FALSE);

  GetClientRect(&rc);
  while( (rc.right-rc.left != width-1) || (rc.bottom-rc.top != height-1) )
  {
  
    if(rc.right-rc.left < width-1)
      cxTotal ++;
    else if(rc.right-rc.left > width-1)
      cxTotal --;
    if(rc.bottom-rc.top < height-1)
      cyTotal ++;
    else if(rc.bottom-rc.top > height-1)
      cyTotal --;

	  r.left=(nCx-cxTotal)/2;r.top=(nCy-cyTotal)/2;
	  r.right=(nCx+cxTotal)/2;r.bottom=(nCy+cyTotal)/2;
	
    cw = rc.right-rc.left+1;
    ch = rc.bottom-rc.top+1;

    MoveWindow(&r);//,FALSE);
    GetClientRect(&rc);

    if( (cw == rc.right-rc.left+1) && (ch == rc.bottom-rc.top+1) )  // client size unchanged
      break;
  }
}


BEGIN_MESSAGE_MAP(CChildWindow, CFrameWnd)
	//{{AFX_MSG_MAP(CChildWindow)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildWindow message handlers

void CChildWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if(!Y || !RGBbuf) return;
	
	char msg[128];
	wsprintf(msg,"#%d - %s",nPicShowOrder, inSeqName);
	SetWindowText(msg);
	
	if(bColorImage){    //彩色	//Colorful
		conv.YV12_to_RGB24(Y,Cb,Cr,RGBbuf,iWidth,iHeight);
		ShowImage(&dc,RGBbuf);
	}
	else {                    //黑白	//Monochrome
		ShowGrayImage(&dc,Y);
	} 

	// Do not call CFrameWnd::OnPaint() for painting messages
}

int CChildWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
    int i;
	HANDLE hloc1;
	RGBQUAD *argbq;

	hloc1 = LocalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE,(sizeof(RGBQUAD) * 256));
	argbq = (RGBQUAD *) LocalLock(hloc1);

	for(i=0;i<256;i++) {
		argbq[i].rgbBlue=i;
		argbq[i].rgbGreen=i;
		argbq[i].rgbRed=i;
		argbq[i].rgbReserved=0;
	}

	BmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BmpInfo->bmiHeader.biPlanes = 1;
	if(bColorImage) 
		BmpInfo->bmiHeader.biBitCount = 24;
	else BmpInfo->bmiHeader.biBitCount = 8;
	BmpInfo->bmiHeader.biCompression = BI_RGB;
	BmpInfo->bmiHeader.biWidth = iWidth;
	BmpInfo->bmiHeader.biHeight = iHeight;

	memcpy(BmpInfo->bmiColors, argbq, sizeof(RGBQUAD) * 256);

	LocalUnlock(hloc1);
	LocalFree(hloc1);

	return 0;
}
