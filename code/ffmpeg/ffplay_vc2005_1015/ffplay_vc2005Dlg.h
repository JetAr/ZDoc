// ffplay_vc2005Dlg.h : header file
//

#pragma once

void ff_log_callback(void*avcl, int level, const char*fmt, va_list vl);

// Cffplay_vc2005Dlg dialog
class Cffplay_vc2005Dlg : public CDialog
{
// Construction
public:
	Cffplay_vc2005Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FFPLAY_VC2005_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
public:
	afx_msg void OnBnClickedButtonStop();
};
