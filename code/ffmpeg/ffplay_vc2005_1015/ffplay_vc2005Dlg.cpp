// ffplay_vc2005Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "ffplay_vc2005.h"
#include "ffplay_vc2005Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cffplay_vc2005Dlg dialog




Cffplay_vc2005Dlg::Cffplay_vc2005Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cffplay_vc2005Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	av_log_set_callback(ff_log_callback);
}

void Cffplay_vc2005Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cffplay_vc2005Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &Cffplay_vc2005Dlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &Cffplay_vc2005Dlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()


// Cffplay_vc2005Dlg message handlers

BOOL Cffplay_vc2005Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cffplay_vc2005Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cffplay_vc2005Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Cffplay_vc2005Dlg::OnBnClickedButtonStart()
{
	// TODO: Add your control notification handler code here
	main1(GetDlgItem(IDC_STATIC_VIDEO)->m_hWnd);
}

void Cffplay_vc2005Dlg::OnBnClickedButtonStop()
{
	// TODO: Add your control notification handler code here
	quit();
}

void ff_log_callback(void*avcl, int level, const char*fmt, va_list vl)
{
	static char log[4096] = { 0 };
	vsnprintf(log, sizeof(log), fmt, vl);

	OutputDebugStringA(log);
}