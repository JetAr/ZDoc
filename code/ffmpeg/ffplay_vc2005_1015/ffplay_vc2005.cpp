// ffplay_vc2005.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ffplay_vc2005.h"
#include "ffplay_vc2005Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cffplay_vc2005App

BEGIN_MESSAGE_MAP(Cffplay_vc2005App, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// Cffplay_vc2005App construction

Cffplay_vc2005App::Cffplay_vc2005App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only Cffplay_vc2005App object

Cffplay_vc2005App theApp;


// Cffplay_vc2005App initialization

BOOL Cffplay_vc2005App::InitInstance()
{
	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	Cffplay_vc2005Dlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
