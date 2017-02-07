/*
Module : MidiPlayerDlg.h
Purpose: Defines the header implementation for the CMidiPlayerDlg class
Created: CJP / 20-09-2001
History: CJP / 23-01-2002

	1. Started the new version of MidiPlayer with MFC

	2. Fixed bugs with the combo list and the midi port selection

	3. Fixed problems with the player playback states

	4. Increased the maximum number of midi ports

Copyright (c) 2001 by C. Jiménez de Parga
All rights reserved.
*/

// Some headers required

#include "dmusic.h"
#include "VolumeOutMaster.h"
#include "Panel.h"

#if !defined(AFX_MIDIPLAYERDLG_H__6F5E7647_AF4A_11D5_B47A_000001191531__INCLUDED_)
#define AFX_MIDIPLAYERDLG_H__6F5E7647_AF4A_11D5_B47A_000001191531__INCLUDED_
#define MAX_PORTS 30

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMidiPlayerDlg dialog

class CMidiPlayerDlg : public CDialog
{
// Construction
public:
    CMidiPlayerDlg(CWnd* pParent = NULL);	// standard constructor
    void SetPictures();
    bool InitMidiLibrary();
    bool PopulateMidiPorts();
    bool InitControls();
    enum PlayerState {PLAYING,STOPPED,PAUSED,LOADED,INIT,LOADING} State;
    // Dialog Data
    //{{AFX_DATA(CMidiPlayerDlg)
    enum { IDD = IDD_MIDIPLAYER_DIALOG };
    CButton			m_CenterZ;
    CButton			m_CenterY;
    CButton			m_CenterX;
    CProgressCtrl	m_Progress;
    CSliderCtrl		m_SliderTempo;
    CSliderCtrl		m_SliderVolume;
    CSliderCtrl		m_SliderZ;
    CSliderCtrl		m_SliderY;
    CSliderCtrl		m_SliderX;
    CButton			m_ReverbButton;
    BOOL			m_b3D;
    BOOL			m_bRepeat;
    BOOL			m_bMidiFile;
    D3DVALUE        m_nPosX;
    D3DVALUE		m_nPosY;
    D3DVALUE		m_nPosZ;
    CString			strSongSel;
    MUSIC_TIME		mtIncrement;
    MUSIC_TIME		mtTotalTicks;
    CComboBox		m_PortList;
    CButton			m_3DButton;
    CButton			m_ReplayButton;
    CButton			m_BackwardButton;
    CButton			m_StopButton;
    CButton			m_AboutButton;
    CButton			m_ForwardButton;
    CButton			m_PauseButton;
    CButton			m_OpenButton;
    CButton			m_PlayButton;
    CMidiMusic		*m_pMidi;
    CVolumeOutMaster m_Volume;
    CDisplay		m_Display;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMidiPlayerDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    HICON m_hIcon;

    // Space to store the maximum midi ports. Assumed max 10 ports
    INFOPORT Info[MAX_PORTS];
    // Generated message map functions
    //{{AFX_MSG(CMidiPlayerDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void On3d();
    afx_msg void OnOpen();
    afx_msg void OnClose();
    afx_msg void OnPlay();
    afx_msg void OnSelchangePorts();
    afx_msg void OnStop();
    afx_msg void OnPause();
    afx_msg void OnForward();
    afx_msg void OnBackward();
    afx_msg void OnReverb();
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnReplay();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnAbout();
    afx_msg void OnCheckCenterx();
    afx_msg void OnCheckCentery();
    afx_msg void OnCheckCenterz();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDIPLAYERDLG_H__6F5E7647_AF4A_11D5_B47A_000001191531__INCLUDED_)
