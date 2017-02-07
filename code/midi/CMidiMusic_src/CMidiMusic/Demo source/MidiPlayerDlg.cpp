/*
Module : MidiPlayerDlg.cpp
Purpose: Defines the code implementation for the CMidiPlayerDlg class
Created: CJP / 20-09-2001
History: CJP / 23-01-2002

	1. Started the new version of MidiPlayer with MFC

	2. Fixed bugs with the combo list and the midi port selection

	3. Fixed problems with the player playback states

	4. Increased the maximum number of midi ports

Copyright (c) 2001 by C. Jiménez de Parga
All rights reserved.
*/


#include "stdafx.h"
#include "MidiPlayer.h"
#include "MidiPlayerDlg.h"
#include "About.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMidiPlayerDlg dialog

CMidiPlayerDlg::CMidiPlayerDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CMidiPlayerDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CMidiPlayerDlg)
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_pMidi=new CMidiMusic; // Create the class dinamically
    m_b3D=TRUE;				// Start in 3d mode
    m_bRepeat=FALSE;
    m_nPosX=m_nPosY=m_nPosZ=0;
    State=INIT;
}

void CMidiPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMidiPlayerDlg)
    DDX_Control(pDX, IDC_CHECK_CENTERZ, m_CenterZ);
    DDX_Control(pDX, IDC_CHECK_CENTERY, m_CenterY);
    DDX_Control(pDX, IDC_CHECK_CENTERX, m_CenterX);
    DDX_Control(pDX, IDC_PROGRESS, m_Progress);
    DDX_Control(pDX, IDC_TEMPO, m_SliderTempo);
    DDX_Control(pDX, IDC_VOLUME, m_SliderVolume);
    DDX_Control(pDX, IDC_Z, m_SliderZ);
    DDX_Control(pDX, IDC_Y, m_SliderY);
    DDX_Control(pDX, IDC_X, m_SliderX);
    DDX_Control(pDX, IDC_REVERB, m_ReverbButton);
    DDX_Control(pDX, IDC_PORTS, m_PortList);
    DDX_Control(pDX, IDC_3D, m_3DButton);
    DDX_Control(pDX, IDC_REPLAY, m_ReplayButton);
    DDX_Control(pDX, IDC_BACKWARD, m_BackwardButton);
    DDX_Control(pDX, IDC_STOP, m_StopButton);
    DDX_Control(pDX, IDC_ABOUT, m_AboutButton);
    DDX_Control(pDX, IDC_FORWARD, m_ForwardButton);
    DDX_Control(pDX, IDC_PAUSE, m_PauseButton);
    DDX_Control(pDX, IDC_OPEN, m_OpenButton);
    DDX_Control(pDX, IDC_PLAY, m_PlayButton);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMidiPlayerDlg, CDialog)
    //{{AFX_MSG_MAP(CMidiPlayerDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_3D, On3d)
    ON_BN_CLICKED(IDC_OPEN, OnOpen)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_PLAY, OnPlay)
    ON_CBN_SELCHANGE(IDC_PORTS, OnSelchangePorts)
    ON_BN_CLICKED(IDC_STOP, OnStop)
    ON_BN_CLICKED(IDC_PAUSE, OnPause)
    ON_BN_CLICKED(IDC_FORWARD, OnForward)
    ON_BN_CLICKED(IDC_BACKWARD, OnBackward)
    ON_BN_CLICKED(IDC_REVERB, OnReverb)
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_REPLAY, OnReplay)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_ABOUT, OnAbout)
    ON_BN_CLICKED(IDC_CHECK_CENTERX, OnCheckCenterx)
    ON_BN_CLICKED(IDC_CHECK_CENTERY, OnCheckCentery)
    ON_BN_CLICKED(IDC_CHECK_CENTERZ, OnCheckCenterz)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMidiPlayerDlg message handlers

// Entry point to the dialog

BOOL CMidiPlayerDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // TODO: Add extra initialization here
    // Set the pictures on the buttons
    SetPictures();

    // Initialize the library, the controls and set a timer for player events notifiaction
    if (InitMidiLibrary())
    {
        PopulateMidiPorts();
        InitControls();
        SetTimer(1,100,NULL);
    }
    else
    {
        delete m_pMidi;
        EndDialog(0);
    }

    return TRUE;  // return TRUE  unless you set the focus to a control
}


// Member function that initialize the parameters of the controls

bool CMidiPlayerDlg::InitControls()
{
    m_3DButton.SetCheck(1);
    m_SliderX.SetRange(-299,299,TRUE);
    m_SliderX.SetPos(0);
    m_SliderY.SetRange(-299,299,TRUE);
    m_SliderY.SetPos(0);
    m_SliderZ.SetRange(-299,299,TRUE);
    m_SliderZ.SetPos(0);
    m_SliderTempo.SetRange(0,8,TRUE);
    m_SliderTempo.SetPos(4);
    m_pMidi->SetMaxDistance(D3DVALUE(5.2));
    m_pMidi->SetMode(DS3DMODE_HEADRELATIVE);
    m_pMidi->SetPosition(0,0,0);
    m_Display.InitDisplay(this,8,15,157,75);
    m_Display.SetText(this,"Tick count:",5,5,RGB(255,255,0));
    m_Display.SetText(this,"Song title:",5,25,RGB(255,255,0));
    m_SliderVolume.SetRange(m_Volume.GetMinimalVolume(),m_Volume.GetMaximalVolume(),TRUE);
    m_SliderVolume.SetPos(m_Volume.GetCurrentVolume());
    m_CenterX.SetCheck(1);
    m_CenterY.SetCheck(1);
    m_CenterZ.SetCheck(1);
    return true;
}

// Member function to initialize the midi library

bool CMidiPlayerDlg::InitMidiLibrary()
{
    if FAILED(m_pMidi->Initialize(m_b3D))
    {
        CString strMsg="Can't initialize DirectMusic objects."\
                       "Ensure you have an audio card device and DirectX 8.0 or above"\
                       "installed in your system.";
        AfxMessageBox(strMsg);
        return false;
    }
    return true;
}

// Member function to populate the midi ports at start up

bool CMidiPlayerDlg::PopulateMidiPorts()
{
    DWORD dwcount=0;
    int nSel=-1,nCount=0;

    while ((m_pMidi->PortEnumeration(dwcount,&Info[dwcount])==S_OK) && (dwcount<MAX_PORTS))
    {
        if (Info[dwcount].dwClass==DMUS_PC_OUTPUTCLASS)
        {
            m_PortList.AddString(Info[dwcount].szPortDescription);
            m_PortList.SetItemDataPtr(nCount,(LPINFOPORT)&Info[dwcount]);
            if (((Info[dwcount].dwFlags & DMUS_PC_SOFTWARESYNTH)) && (nSel<0))
            {
                m_pMidi->SelectPort(&Info[dwcount]);
                nSel=nCount;
            }
            nCount++;
        }
        dwcount++;
    }

    m_PortList.SetCurSel(nSel);
    return TRUE;
}



// Member function to set the pictures on the buttons

void CMidiPlayerDlg::SetPictures()
{
    HICON hIcon;

    hIcon=AfxGetApp()->LoadIcon(IDI_ICON1);
    m_OpenButton.SetIcon(hIcon);
    hIcon=AfxGetApp()->LoadIcon(IDI_ICON2);
    m_ForwardButton.SetIcon(hIcon);
    hIcon=AfxGetApp()->LoadIcon(IDI_ICON3);
    m_PauseButton.SetIcon(hIcon);
    hIcon=AfxGetApp()->LoadIcon(IDI_ICON4);
    m_AboutButton.SetIcon(hIcon);
    hIcon=AfxGetApp()->LoadIcon(IDI_ICON5);
    m_BackwardButton.SetIcon(hIcon);
    hIcon=AfxGetApp()->LoadIcon(IDI_ICON6);
    m_ReplayButton.SetIcon(hIcon);
    hIcon=AfxGetApp()->LoadIcon(IDI_ICON7);
    m_StopButton.SetIcon(hIcon);
    hIcon=AfxGetApp()->LoadIcon(IDI_ICON8);
    m_PlayButton.SetIcon(hIcon);

}

// Member function called when painting is needed

void CMidiPlayerDlg::OnPaint()
{

    CPaintDC dc(this);
    dc.BitBlt(m_Display.m_x,m_Display.m_y,m_Display.m_w,m_Display.m_h,
              &m_Display.m_memDC,0,0,SRCCOPY);
    CDialog::OnPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.

HCURSOR CMidiPlayerDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

// Member function to handle the 3D button

void CMidiPlayerDlg::On3d()
{
    // TODO: Add your control notification handler code here
    m_b3D = !m_b3D;
    m_3DButton.SetCheck(m_b3D);

}


// Member class called when a file is requiered

void CMidiPlayerDlg::OnOpen()
{
    // TODO: Add your control notification handler code here
    char szFilters[]="Midi Files (*.mid)|*.mid||";
    char szCurdir[_MAX_PATH];
    CFileDialog CFileDlg (TRUE, "Mid", "*.mid",OFN_FILEMUSTEXIST, szFilters, this);
    GetCurrentDirectory(_MAX_PATH,szCurdir);
    CFileDlg.m_ofn.lpstrInitialDir=szCurdir;

    // If file selected then proceed to load and check it
    if(CFileDlg.DoModal() == IDOK)
    {
        if (State!=INIT) m_pMidi->Stop();

        State=LOADING;

        if (strstr(CFileDlg.m_ofn.lpstrFile,".mid"))
            m_bMidiFile=TRUE;
        else m_bMidiFile=FALSE;

        if FAILED(m_pMidi->LoadMidiFromFile(CFileDlg.m_ofn.lpstrFile))
        {
            State=INIT;
            AfxMessageBox("Error loading file");
            return;
        }

        // Set the controls' parameters depending on the segment length
        strSongSel=CFileDlg.m_ofn.lpstrFile;
        m_pMidi->GetLength(&mtTotalTicks);
        m_Progress.SetRange32(0,mtTotalTicks);
        mtIncrement=mtTotalTicks/20;
        m_Display.EraseText(this,"                                                                                ",
                            5,40);
        m_Display.SetText(this,CFileDlg.m_ofn.lpstrFileTitle,5,40,RGB(0,200,255));
        State=LOADED;
    }
}


// Handle the closing message

void CMidiPlayerDlg::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    KillTimer(1);
    delete m_pMidi;
    CDialog::OnClose();
}

// Handle the play button

void CMidiPlayerDlg::OnPlay()
{
    // TODO: Add your control notification handler code here
    if (State!=INIT)
    {
        if (State!=PAUSED)
        {
            m_pMidi->Stop();
            m_Display.EraseText(this,"999999999999",65,5);
        }
        m_pMidi->Play();
        State=PLAYING;
    }
}


// This member function is called when the user changes the midi port

void CMidiPlayerDlg::OnSelchangePorts()
{
    // TODO: Add your control notification handler code here
    int nSel;
    INFOPORT InfoSel;

    OnStop();

    nSel=m_PortList.GetCurSel();	// Get the item index on the list
    KillTimer(1);
    delete m_pMidi;					// Delete the object CMidiMusic

    m_pMidi=new CMidiMusic();			// Create a new CMidiMusic object

    InfoSel=*(LPINFOPORT)m_PortList.GetItemDataPtr(nSel); // Get a pointer to INFOPORT item in the array

    if (!(InfoSel.dwFlags & DMUS_PC_SOFTWARESYNTH))		  // Check synthesizer mode
    {
        m_b3D=FALSE;
        m_3DButton.SetCheck(0);
    }

    m_pMidi->Initialize(m_b3D);							 // Initialize the object
    m_pMidi->SelectPort(&InfoSel);						 // Set the selected port

    if (!strSongSel.IsEmpty())							 // If there was already a file selected
    {
        m_pMidi->LoadMidiFromFile(strSongSel);
        State=LOADED;
    }

    m_SliderTempo.SetPos(4);

    SetTimer(1,100,NULL);								// Set the timer again
}


// On Stop handler

void CMidiPlayerDlg::OnStop()
{
    // TODO: Add your control notification handler code here
    if (State==PLAYING)
    {
        m_pMidi->Stop();
        State=STOPPED;
    }
}

// On Pause handler

void CMidiPlayerDlg::OnPause()
{
    // TODO: Add your control notification handler code here
    if (State==PLAYING)
    {
        m_pMidi->Pause();
        State=PAUSED;
    }
    else if (State==PAUSED)
        OnPlay();

}


// Member function to handle the forward button

void CMidiPlayerDlg::OnForward()
{
    // TODO: Add your control notification handler code here
    if (State==PLAYING)
    {
        MUSIC_TIME mtNow;
        m_pMidi->GetSeek(&mtNow);
        mtNow+=mtIncrement;
        if (mtNow<mtTotalTicks) m_pMidi->Seek(mtNow);
        else m_pMidi->Seek(mtIncrement);
        m_pMidi->Play();
    }
}

// Member function to handle the rewind button

void CMidiPlayerDlg::OnBackward()
{
    // TODO: Add your control notification handler code here
    if (State==PLAYING)
    {
        MUSIC_TIME mtNow;
        m_pMidi->GetSeek(&mtNow);
        mtNow-=mtIncrement;
        if (mtNow>0)
            m_pMidi->Seek(mtNow);
        else m_pMidi->Seek(mtTotalTicks-mtIncrement);
        m_pMidi->Play();
    }
}

// Handle the Reverb button

void CMidiPlayerDlg::OnReverb()
{
    // TODO: Add your control notification handler code here
    static BOOL bReverb=TRUE;
    m_ReverbButton.SetCheck(bReverb);
    m_pMidi->SetEffect(bReverb,SET_CHORUS | SET_REVERB);
    bReverb= !bReverb;
}


// Control the 3D sliders

void CMidiPlayerDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default
    char strLabel[5];

    if (pScrollBar==(CScrollBar*)&m_SliderX)
    {
        m_nPosX=(float)m_SliderX.GetPos();
        m_nPosX/=100;
        m_pMidi->SetPosition(m_nPosX,m_nPosY,m_nPosZ);
        sprintf(strLabel,"%.1f",m_nPosX);
        SendDlgItemMessage(IDC_COUNTER_X,WM_SETTEXT,0,(LPARAM)strLabel);
        m_CenterX.SetCheck(0);
    }
    else if (pScrollBar==(CScrollBar*)&m_SliderY)
    {
        m_nPosY=(float)m_SliderY.GetPos();
        m_nPosY/=100;
        m_pMidi->SetPosition(m_nPosX,m_nPosY,m_nPosZ);
        sprintf(strLabel,"%.1f",m_nPosY);
        SendDlgItemMessage(IDC_COUNTER_Y,WM_SETTEXT,0,(LPARAM)strLabel);
        m_CenterY.SetCheck(0);
    }
    else if (pScrollBar==(CScrollBar*)&m_SliderZ)
    {
        m_nPosZ=(float)m_SliderZ.GetPos();
        m_nPosZ/=100;
        m_pMidi->SetPosition(m_nPosX,m_nPosY,m_nPosZ);
        sprintf(strLabel,"%.1f",m_nPosZ);
        SendDlgItemMessage(IDC_COUNTER_Z,WM_SETTEXT,0,(LPARAM)strLabel);
        m_CenterZ.SetCheck(0);
    }
    CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

// Handle the volume and tempo sliders

void CMidiPlayerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default
    int nVolume;
    float nTempo;
    if (pScrollBar==(CScrollBar*)&m_SliderVolume)
    {
        nVolume=m_SliderVolume.GetPos();
        m_Volume.SetCurrentVolume(nVolume);
    }
    else if (pScrollBar==(CScrollBar*)&m_SliderTempo)
    {
        nTempo=(float)m_SliderTempo.GetPos();
        nTempo=nTempo/4;
        m_pMidi->SetMasterTempo(nTempo);
    }
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


// Toggle the file repeat function

void CMidiPlayerDlg::OnReplay()
{
    // TODO: Add your control notification handler code here
    m_bRepeat=!m_bRepeat;
    (m_bRepeat) ? m_Display.SetText(this,"Replay activated",5,55,RGB(0,255,0)) :
    m_Display.EraseText(this,"Replay Activated",5,55);
}

// Timer message handler

void CMidiPlayerDlg::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    switch(State)
    {
    case STOPPED:
        m_Display.SetText(this,"Stopped    ",100,55,RGB(255,255,255));
        break;
    case PLAYING:
        m_Display.SetText(this,"Playing    ",100,55,RGB(255,255,255));
        break;
    case PAUSED:
        m_Display.SetText(this,"Paused     ",100,55,RGB(255,255,255));
        break;
    case LOADING:
        m_Display.SetText(this,"Loading    ",100,55,RGB(255,255,255));
        break;
    case LOADED:
        m_Display.SetText(this,"Loaded     ",100,55,RGB(255,255,255));
        break;
    case INIT:
        m_Display.SetText(this,"Initialized",100,55,RGB(255,255,255));
        break;
    default:
        break;
    }

    // In case is playing then display the tick count and check for repeat again

    if (State==PLAYING)
    {
        MUSIC_TIME mtTicks,mtLength;
        char strTicks[19];
        m_pMidi->GetLength(&mtLength);
        m_pMidi->GetTicks(&mtTicks);
        sprintf(strTicks,"%d",mtTicks);
        strcat(strTicks,"          ");
        m_Display.SetText(this,strTicks,65,5,RGB(0,200,255));
        if ((mtTicks>=mtLength) && (m_bMidiFile))
        {
            State=STOPPED;
            if (m_bRepeat) OnPlay();
        }
        m_Progress.SetPos(mtTicks);
    }
    CDialog::OnTimer(nIDEvent);
}

// About button handler

void CMidiPlayerDlg::OnAbout()
{
    // TODO: Add your control notification handler code here
    CAbout AboutDlg;
    AboutDlg.DoModal();
}


// Center the X-axis

void CMidiPlayerDlg::OnCheckCenterx()
{
    // TODO: Add your control notification handler code here
    m_nPosX=0;
    m_pMidi->SetPosition(0,m_nPosY,m_nPosZ);
    m_SliderX.SetPos(0);
    m_CenterX.SetCheck(1);
    SendDlgItemMessage(IDC_COUNTER_X,WM_SETTEXT,0,(LPARAM)"0");
}

// Center the Y-axis

void CMidiPlayerDlg::OnCheckCentery()
{
    // TODO: Add your control notification handler code here
    m_nPosY=0;
    m_pMidi->SetPosition(m_nPosX,0,m_nPosZ);
    m_SliderY.SetPos(0);
    m_CenterY.SetCheck(1);
    SendDlgItemMessage(IDC_COUNTER_Y,WM_SETTEXT,0,(LPARAM)"0");
}

// Center the Z-axis

void CMidiPlayerDlg::OnCheckCenterz()
{
    // TODO: Add your control notification handler code here
    m_nPosZ=0;
    m_pMidi->SetPosition(m_nPosX,m_nPosY,0);
    m_SliderZ.SetPos(0);
    m_CenterZ.SetCheck(1);
    SendDlgItemMessage(IDC_COUNTER_Z,WM_SETTEXT,0,(LPARAM)"0");
}
