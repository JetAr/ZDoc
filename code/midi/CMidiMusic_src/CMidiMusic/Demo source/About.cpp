// About.cpp : implementation file
//

#include "stdafx.h"
#include "MidiPlayer.h"
#include "About.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbout dialog


CAbout::CAbout(CWnd* pParent /*=NULL*/)
    : CDialog(CAbout::IDD, pParent)
{
    //{{AFX_DATA_INIT(CAbout)
    //}}AFX_DATA_INIT
}


void CAbout::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAbout)
    DDX_Control(pDX, IDC_STATIC_LINK, m_Link);
    DDX_Control(pDX, IDC_IMAGE, m_Image);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbout, CDialog)
    //{{AFX_MSG_MAP(CAbout)
    ON_BN_CLICKED(IDC_STATIC_LINK, OnStaticLink)
    ON_WM_CLOSE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbout message handlers
CFont *pFont,*pModFont;

BOOL CAbout::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    HBITMAP hbm;
    LOGFONT lf;
    HFONT hFont;
    hbm=(HBITMAP)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_BITMAP1),IMAGE_BITMAP,0,0,
                           LR_LOADMAP3DCOLORS |LR_LOADTRANSPARENT);
    m_Image.SetBitmap(hbm);
    pFont=m_Link.GetFont();
    pFont->GetObject(sizeof(lf),&lf);
    lf.lfUnderline=TRUE;
    hFont=CreateFontIndirect(&lf);
    pModFont=pFont->FromHandle(hFont);
    m_Link.SetFont(pModFont);
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CAbout::OnStaticLink()
{
    // TODO: Add your control notification handler code here
    //ShellExecute(this->m_hWnd,_T("open"),_T("http://www.codeproject.com"),_T(""),_T(""),SW_SHOWNORMAL);
    // Commeted on 27th November 2010 due to a memory corruption
}


void CAbout::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    pFont->DeleteObject();
    pModFont->DeleteObject();
    CDialog::OnClose();
}
