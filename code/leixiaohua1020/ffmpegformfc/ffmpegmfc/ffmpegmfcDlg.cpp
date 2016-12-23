/*
 * FFmpeg for MFC
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 *
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本工程将ffmpeg工程中的ffmpeg转码器（ffmpeg.c）移植到了VC环境下。
 * 并且使用MFC做了一套简单的界面。
 * This software transplant ffmpeg to Microsoft VC++ environment.
 * And use MFC to build a simple Graphical User Interface.
 */

#include "stdafx.h"
#include "ffmpegmfc.h"
#include "ffmpegmfcDlg.h"
#include "afxdialogex.h"

//CPP调用C
#ifdef __cplusplus
extern "C" {
#endif
#include "ffmpeg.h"
//上下文
extern FFMFCContext *ffmfcctx;
#ifdef __cplusplus
}
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//使用ffmpeg.exe
#define USE_FFMPEGEXE 1

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// 对话框数据
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CffmpegmfcDlg 对话框




CffmpegmfcDlg::CffmpegmfcDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CffmpegmfcDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CffmpegmfcDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS_CURRENT, m_progresscurrent);
    DDX_Control(pDX, IDC_PROGRESS_ALL, m_progressall);
    DDX_Control(pDX, IDC_PROGRESS_ALL_P, m_progressallp);
    DDX_Control(pDX, IDC_PROGRESS_CURRENT_P, m_progresscurrentp);
    DDX_Control(pDX, IDC_TASKLIST, m_tasklist);
}

BEGIN_MESSAGE_MAP(CffmpegmfcDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_START, &CffmpegmfcDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_PAUSE, &CffmpegmfcDlg::OnBnClickedPause)
    ON_BN_CLICKED(IDC_STOP, &CffmpegmfcDlg::OnBnClickedStop)
    ON_BN_CLICKED(IDC_TASKLIST_ADD, &CffmpegmfcDlg::OnBnClickedTasklistAdd)
    ON_BN_CLICKED(IDC_TASKLIST_EDIT, &CffmpegmfcDlg::OnBnClickedTasklistEdit)
    ON_BN_CLICKED(IDC_TASKLIST_DELETE, &CffmpegmfcDlg::OnBnClickedTasklistDelete)
    ON_BN_CLICKED(IDC_TASKLIST_CLEAR, &CffmpegmfcDlg::OnBnClickedTasklistClear)
    ON_NOTIFY ( NM_CUSTOMDRAW,IDC_TASKLIST,&CffmpegmfcDlg::OnCustomdrawMyList )
    ON_COMMAND(IDC_ABOUT, &CffmpegmfcDlg::OnAbout)
    ON_COMMAND(IDC_SYSINFO, &CffmpegmfcDlg::OnSysinfo)
    ON_COMMAND(ID_LANG_CN, &CffmpegmfcDlg::OnLangCn)
    ON_COMMAND(ID_LANG_EN, &CffmpegmfcDlg::OnLangEn)
    ON_NOTIFY(NM_DBLCLK, IDC_TASKLIST, &CffmpegmfcDlg::OnDblclkTasklist)
    ON_NOTIFY(NM_RCLICK, IDC_TASKLIST, &CffmpegmfcDlg::OnRclickTasklist)
    ON_WM_DROPFILES()
    ON_COMMAND(ID_WEBSITE, &CffmpegmfcDlg::OnWebsite)
    ON_COMMAND(IDCANCEL, &CffmpegmfcDlg::OnIdcancel)
    ON_MESSAGE(WM_SETPROGRESS, &CffmpegmfcDlg::OnSetProgress)
    ON_COMMAND(ID_OUTTRANSCODE, &CffmpegmfcDlg::OnOuttranscode)
    ON_COMMAND(ID_INTRANSCODE, &CffmpegmfcDlg::OnIntranscode)
END_MESSAGE_MAP()

//转码线程设置语言
//set convert thread's language
void Thread_LoadLanguage()
{
    //配置文件路径
    char conf_path[300]= {0};
    //获得exe绝对路径
    GetModuleFileNameA(NULL,(LPSTR)conf_path,300);//
    //获得exe文家夹路径
    strrchr( conf_path, '\\')[0]= '\0';//
    strcat(conf_path,"\\configure.ini");
    //存储属性的字符串
    char conf_val[300]= {0};

    if((_access(conf_path, 0 )) == -1 )
    {
        //配置文件不存在，直接返回
    }
    else
    {
        GetPrivateProfileStringA("Settings","language",NULL,conf_val,300,conf_path);
        if(strcmp(conf_val,"Chinese")==0)
        {
            SetThreadUILanguage(MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_DEFAULT));
        }
        else if(strcmp(conf_val,"English")==0)
        {
            SetThreadUILanguage(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
        }

    }
    return;
}
//全局变量--------
HANDLE hRead, hWrite;
BOOL cmdOver=FALSE;
//读取转码进度
DWORD WINAPI ReadProgress(LPVOID lpParam)
{
    CffmpegmfcDlg *dlg=(CffmpegmfcDlg *)lpParam;
    //读取命令行返回值
    BOOL gotDuration=FALSE;
    int VdLen=0;
    int CurLen=0;
    char buff[3000] = {0};
    DWORD dwRead = 0;
    DWORD bytes=0;
    DWORD ReadNum=0;

    std::string strRet;
    std::string FrmNum;

    int idx=-1;
    while (1)
    {

        Sleep(50);
        PeekNamedPipe(hRead, NULL, 0, NULL, &bytes, NULL);
        if (bytes!=0)
        {
            if(bytes>=3000)
                ReadNum=3000;
            else
                ReadNum=bytes;
            ReadFile(hRead, buff, ReadNum, &dwRead, NULL);
            strRet.append(buff, dwRead);
            //TRACE("%s",strRet);
            if (gotDuration==FALSE)
            {
                idx=strRet.find("Duration");
                if (idx>=0)
                {
                    std::string ztm1=strRet.substr(idx+10,2);
                    std::string ztm2=strRet.substr(idx+13,2);
                    std::string ztm3=strRet.substr(idx+16,2);
                    std::string ztm4=strRet.substr(idx+19,2);
                    VdLen=360000*atoi(ztm1.c_str())+6000*atoi(ztm2.c_str())+100*atoi(ztm3.c_str())+atoi(ztm4.c_str());
                    gotDuration=TRUE;
                }
            }

            idx=strRet.rfind("time=");
            if (idx>=0)
            {
                FrmNum=strRet.substr(idx+6,5);
                std::string tm1=strRet.substr(idx+5,2);
                std::string tm2=strRet.substr(idx+8,2);
                std::string tm3=strRet.substr(idx+11,2);
                std::string tm4=strRet.substr(idx+14,2);
                CurLen=360000*atoi(tm1.c_str())+6000*atoi(tm2.c_str())+100*atoi(tm3.c_str())+atoi(tm4.c_str());
                double PerCent=(double)CurLen/(double)VdLen;
                PerCent=PerCent*100;
                if (PerCent<=0.002)
                {
                    PerCent=0;
                }
                TRACE("当前转码进度%.4f%%\n",PerCent);
                dlg->SetProgress((int)PerCent,dlg->all_process);
            }
        }
        if (cmdOver==TRUE)
        {
            printf("当前转码进度100%\n");
            return 0;
        }
    }

    return 0;
}


//转码线程
//convert thread
UINT Thread_ffmpeg(LPVOID lpParam)
{

    CffmpegmfcDlg *dlg=(CffmpegmfcDlg *)lpParam;
    //当前线程的句柄
    ffmfcctx->ffmpeghwnd=GetCurrentThread();

    Thread_LoadLanguage();

    dlg->state=SYSTEM_PROCESS;
    dlg->SetBtn(SYSTEM_PROCESS);

    //设置进度-------
    int unfinish_num=0;
    for(int i=0; i<dlg->cmdargvlist.size(); i++)
    {
        if(dlg->tasklist[i].taskstate!=TASK_FINISH)
            unfinish_num++;
    }
    int all_process_step=0;
    if(unfinish_num!=0)
        all_process_step=100/unfinish_num;
    //---------------

    for(int i=0; i<dlg->cmdargvlist.size(); i++)
    {
        //检查系统状态，例如已经发出了“停止”命令，则直接退出循环
        if(dlg->state==SYSTEM_PREPARE)
            break;
        //如果是“完成”状态，直接跳过--------
        if(dlg->tasklist[i].taskstate==TASK_FINISH)
            continue;
        //FIX:更新状态----
        dlg->tasklist[i].taskstate=TASK_PROCESS;
        dlg->RefreshList();
        //----------------
        if(dlg->transcodemode==MODE_INTERNAL)
        {
            dlg->charlist=dlg->CStringListtoCharList(dlg->cmdargvlist[i]);
            ffmfc_ffmpeg(ffmfcctx,dlg->charlist_size, dlg->charlist);
            dlg->FreeCharList(dlg->charlist);
        }

        if(dlg->transcodemode==MODE_EXTERNAL)
        {
            cmdOver=FALSE;
            DWORD NOUSE=0;
            SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

            if (!CreatePipe(&hRead, &hWrite, &sa, 0))
            {
                continue;
            }
            //设置命令行进程启动信息(以隐藏方式启动命令并定位其输出到hWrite)
            STARTUPINFO si= {sizeof(STARTUPINFO)};
            GetStartupInfo(&si);
            si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
            si.wShowWindow = SW_HIDE;
            si.hStdError = hWrite;
            si.hStdOutput = hWrite;

            PROCESS_INFORMATION pi;

            CString paramstr=dlg->CStringListtoCString(dlg->cmdargvlist[i]);
            BOOL fRet1 = CreateProcess(NULL,paramstr.GetBuffer(),NULL,NULL,TRUE,NORMAL_PRIORITY_CLASS,NULL,NULL,&si,&pi);
            HANDLE TrdID=(HANDLE)CreateThread(NULL,0,ReadProgress,dlg,NULL,&NOUSE);
            if (fRet1 == TRUE)
            {
                WaitForSingleObject(pi.hThread,INFINITE);
                WaitForSingleObject(pi.hThread,INFINITE);
                cmdOver=TRUE;
                CloseHandle(pi.hThread);
                CloseHandle(pi.hProcess);
            }

            WaitForSingleObject(TrdID,INFINITE);
            CloseHandle(hWrite);

        }

        //FIX:更新状态----
        dlg->tasklist[i].taskstate=TASK_FINISH;
        dlg->RefreshList();
        //----------------
        //设置进度
        //FIX：让最后显示100%
        if(dlg->cmdargvlist.size()-i==1)
        {
            dlg->all_process=100;
        }
        else
        {
            dlg->all_process=dlg->all_process+all_process_step;
        }
        dlg->SetProgress(0,dlg->all_process);
    }
    dlg->state=SYSTEM_PREPARE;
    dlg->SetBtn(SYSTEM_PREPARE);


    return 0;
}



//初始化
//Init Dialog
BOOL CffmpegmfcDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标
    //--------------
    //_CrtSetBreakAlloc(803);

    // TODO: 在此添加额外的初始化代码

    //-----------------
    //整行选择；有表格线；表头；单击激活
    DWORD dwExStyle=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE;
    m_tasklist.ModifyStyle(0,LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS);
    m_tasklist.SetExtendedStyle(dwExStyle);

    CString resloader;
    resloader.LoadString(IDS_MAIN_TASKLIST_NUM);
    m_tasklist.InsertColumn(0,resloader,LVCFMT_CENTER,60,0);
    resloader.LoadString(IDS_MAIN_TASKLIST_INPUT);
    m_tasklist.InsertColumn(1,resloader,LVCFMT_CENTER,135,0);
    resloader.LoadString(IDS_MAIN_TASKLIST_SIZE);
    m_tasklist.InsertColumn(2,resloader,LVCFMT_CENTER,70,0);
    resloader.LoadString(IDS_MAIN_TASKLIST_OUPUT);
    m_tasklist.InsertColumn(3,resloader,LVCFMT_CENTER,140,0);
    resloader.LoadString(IDS_MAIN_TASKLIST_STATUS);
    m_tasklist.InsertColumn(4,resloader,LVCFMT_CENTER,70,0);



    //CMFCShellTreeCtrl
    //const DWORD dwViewStyle =WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS;
    //m_explorer.ModifyStyleEx(NULL,dwViewStyle);
    //进度条--
    m_progresscurrent.SetRange(0,100);
    m_progressall.SetRange(0,100);

    //-------------------------
    RefreshList();

    //ffmpeg
    ffmfcctx=(FFMFCContext *)malloc(sizeof(FFMFCContext));
    ffmfcctx->mainhwnd=this->GetSafeHwnd();
    ffmfcctx->progress=0;
    //TaskList菜单
    tasklistmenu.LoadMenu(IDR_MENU_TASKLIST);//编辑好的菜单资源
    ptasklistsubmenu = tasklistmenu.GetSubMenu(0);
    //系统状态
    state=SYSTEM_PREPARE;
    SetBtn(state);

    UpdateData(FALSE);
    //转码模式
    LoadTranscodemode();
    //-----------------
    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CffmpegmfcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CffmpegmfcDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CffmpegmfcDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


//“开始”按钮
//"Start" Button
void CffmpegmfcDlg::OnBnClickedStart()
{
    CString resloader;
    if(tasklist.size()<1)
    {
        resloader.LoadString(IDS_MSGBOX_NOTASK);
        AfxMessageBox(resloader);
        return;
    }
    SystemClear();
    GetAllCmdargv();
    pThreadffmpeg = AfxBeginThread(Thread_ffmpeg,this);//开启线程
}

//“暂停”按钮
//"Pause" Button
void CffmpegmfcDlg::OnBnClickedPause()
{
    CString resloader;
    switch(state)
    {
    case SYSTEM_PROCESS:
    {
        resloader.LoadString(IDS_MAIN_BUTTON_CONTINUE);
        GetDlgItem(IDC_PAUSE)->SetWindowText(resloader);
        //传命令
        ffmfcctx->dlgcmd=DLGCMD_PAUSE;
        state=SYSTEM_PAUSE;
        break;
    }
    case SYSTEM_PAUSE:
    {
        resloader.LoadString(IDS_MAIN_BUTTON_PAUSE);
        GetDlgItem(IDC_PAUSE)->SetWindowText(resloader);
        //传命令
        ffmfcctx->dlgcmd=DLGCMD_PROGRESS;
        state=SYSTEM_PROCESS;
        break;
    }
    }
}

//“停止”按钮
//"Stop" Button
void CffmpegmfcDlg::OnBnClickedStop()
{
    SetBtn(SYSTEM_PREPARE);
    ffmfcctx->dlgcmd=DLGCMD_STOP;
    state=SYSTEM_PREPARE;
}

//任务列表的“添加”按钮
//TaskList's "Add" Button
void CffmpegmfcDlg::OnBnClickedTasklistAdd()
{
    taskinfodlg=new TaskinfoDlg;
    taskinfodlg->maindlg=this;
    taskinfodlg->LoadTask(TASK_DLG_ADD,-1);
    taskinfodlg->Create(IDD_TASKINFO_DIALOG);
    taskinfodlg->ShowWindow(TRUE);

}

//添加一个任务
//Add a task
void CffmpegmfcDlg::TasklistAdd(Task task)
{
    tasklist.push_back(task);
    RefreshList();
}
//任务列表的“编辑”按钮
//TaskList's "Add" Button
void CffmpegmfcDlg::OnBnClickedTasklistEdit()
{
    CString resloader;
    POSITION pos =  m_tasklist.GetFirstSelectedItemPosition();
    int index = m_tasklist.GetNextSelectedItem(pos);   //取得当前行号
    if(index==-1)
    {
        resloader.LoadString(IDS_MSGBOX_NOTASKSEL);
        AfxMessageBox(resloader);
        return;
    }


    taskinfodlg=new TaskinfoDlg;
    taskinfodlg->maindlg=this;
    taskinfodlg->LoadTask(TASK_DLG_EDIT,index);
    taskinfodlg->Create(IDD_TASKINFO_DIALOG);

    taskinfodlg->ShowWindow(TRUE);

}
//编辑一个任务
//Edit a task
void CffmpegmfcDlg::TasklistEdit(Task task,int taskid)
{
    tasklist[taskid]=task;
    RefreshList();
}

//任务列表的“删除”按钮
//TaskList's "Delete" Button
void CffmpegmfcDlg::OnBnClickedTasklistDelete()
{
    //删除元素
    POSITION pos =  m_tasklist.GetFirstSelectedItemPosition();
    int index = m_tasklist.GetNextSelectedItem(pos);   //取得当前行号
    CString resloader;

    if(index!=-1)
    {
        resloader.LoadString(IDS_MSGBOX_TASKDELETE);
        if(AfxMessageBox(resloader,MB_OKCANCEL|MB_ICONEXCLAMATION)==IDCANCEL)
        {
            return;
        }
        tasklist.erase(tasklist.begin()+index);
    }
    else
    {
        resloader.LoadString(IDS_MSGBOX_NOTASKSEL);
        AfxMessageBox(resloader);
    }
    RefreshList();
}

//任务列表的“清空”按钮
//TaskList's "Clear" Button
void CffmpegmfcDlg::OnBnClickedTasklistClear()
{
    CString resloader;
    resloader.LoadString(IDS_MSGBOX_TASKCLEAR);
    if(AfxMessageBox(resloader,MB_OKCANCEL|MB_ICONEXCLAMATION)==IDCANCEL)
    {
        return;
    }
    tasklist.clear();
    RefreshList();
}

//任务列表加颜色
//TaskList's Color
void CffmpegmfcDlg::OnCustomdrawMyList ( NMHDR* pNMHDR, LRESULT* pResult )
{
    //This code based on Michael Dunn's excellent article on
    //list control custom draw at http://www.codeproject.com/listctrl/lvcustomdraw.asp

    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

    // Take the default processing unless we set this to something else below.
    *pResult = CDRF_DODEFAULT;

    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
    if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
    {
        // This is the notification message for an item.  We'll request
        // notifications before each subitem's prepaint stage.

        *pResult = CDRF_NOTIFYSUBITEMDRAW;
    }
    else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
    {

        COLORREF clrNewTextColor, clrNewBkColor;

        int    nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );

        CString strTemp = m_tasklist.GetItemText(nItem,4);
        CString resloader_prepare,resloader_process,resloader_finish;
        resloader_prepare.LoadString(IDS_MAIN_TASKLIST_STATUS_PREPARE);
        resloader_process.LoadString(IDS_MAIN_TASKLIST_STATUS_PROCESS);
        resloader_finish.LoadString(IDS_MAIN_TASKLIST_STATUS_FINISH);

        if(strTemp.Compare(resloader_prepare)==0)
        {
            clrNewTextColor = RGB(0,0,0);		//Set the text
            clrNewBkColor = RGB(255,253,166);		//背景设置成淡黄
        }
        else if(strTemp.Compare(resloader_process)==0)
        {
            clrNewTextColor = RGB(0,0,0);
            clrNewBkColor = RGB(255,166,166);		//背景设置成淡红
        }
        else if(strTemp.Compare(resloader_finish)==0)
        {
            clrNewTextColor = RGB(0,0,0);
            clrNewBkColor = RGB(178,255,166);		//背景设置成淡绿
        }
        else
        {
            clrNewTextColor = RGB(0,0,0);
            clrNewBkColor = RGB(255,255,255);
        }

        pLVCD->clrText = clrNewTextColor;
        pLVCD->clrTextBk = clrNewBkColor;

        // Tell Windows to paint the control itself.
        *pResult = CDRF_DODEFAULT;


    }
}

//系统复位
//Reset System
void CffmpegmfcDlg::SystemClear()
{
    cmdargvlist.clear();
    current_process=0;
    all_process=0;
}

//根据系统状态设置按钮
//Set Buuton's state
void CffmpegmfcDlg::SetBtn(Systemstate state)
{
    switch(state)
    {
    case SYSTEM_PREPARE:
    {
        GetDlgItem(IDC_START)->EnableWindow(TRUE);
        GetDlgItem(IDC_PAUSE)->EnableWindow(FALSE);
        GetDlgItem(IDC_STOP)->EnableWindow(FALSE);

        GetDlgItem(IDC_TASKLIST_ADD)->EnableWindow(TRUE);
        GetDlgItem(IDC_TASKLIST_EDIT)->EnableWindow(TRUE);
        GetDlgItem(IDC_TASKLIST_DELETE)->EnableWindow(TRUE);
        GetDlgItem(IDC_TASKLIST_CLEAR)->EnableWindow(TRUE);
        break;
    }
    case SYSTEM_PROCESS:
    {
        GetDlgItem(IDC_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_PAUSE)->EnableWindow(TRUE);
        GetDlgItem(IDC_STOP)->EnableWindow(TRUE);

        GetDlgItem(IDC_TASKLIST_ADD)->EnableWindow(FALSE);
        GetDlgItem(IDC_TASKLIST_EDIT)->EnableWindow(FALSE);
        GetDlgItem(IDC_TASKLIST_DELETE)->EnableWindow(FALSE);
        GetDlgItem(IDC_TASKLIST_CLEAR)->EnableWindow(FALSE);
        break;
    }
    case SYSTEM_PAUSE:
    {
        GetDlgItem(IDC_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_PAUSE)->EnableWindow(TRUE);
        GetDlgItem(IDC_STOP)->EnableWindow(TRUE);

        GetDlgItem(IDC_TASKLIST_ADD)->EnableWindow(TRUE);
        GetDlgItem(IDC_TASKLIST_EDIT)->EnableWindow(TRUE);
        GetDlgItem(IDC_TASKLIST_DELETE)->EnableWindow(TRUE);
        GetDlgItem(IDC_TASKLIST_CLEAR)->EnableWindow(TRUE);
        break;
    }

    }
}

//获取一个命令行参数
//Get a command's parameter
vector<CString> CffmpegmfcDlg::GetCmdargv(int taskid)
{
    vector<CString> cmdargv;
    Task task=tasklist[taskid];
    CString temp1;

    CString resloader_default;
    resloader_default.LoadString(IDS_TASKINFO_DEFAULT);
    //参数字符串中有程序名-----------------
    temp1.Format(_T("ffmpeg.exe"));
    cmdargv.push_back(temp1);
    //输入文件格式
    if(resloader_default.Compare(task.ifname)!=0)
    {
        temp1.Format(_T("-f"));
        cmdargv.push_back(temp1);
        temp1.Format(_T("%s"),task.ifname);
        cmdargv.push_back(temp1);
    }

    //输入文件路径
    temp1.Format(_T("-i"));
    cmdargv.push_back(temp1);
    temp1.Format(_T("%s"),task.infile);
    cmdargv.push_back(temp1);

    //分辨率
    //temp1.Format(_T("-s"));
    //cmdargv.push_back(temp1);
    //temp1.Format(_T("%dx%d",task.outreso_w,task.outreso_h);
    //cmdargv.push_back(temp1);


    //输出文件格式
    if(resloader_default.Compare(task.ofname)!=0)
    {
        temp1.Format(_T("-f"));
        cmdargv.push_back(temp1);
        temp1.Format(_T("%s"),task.ofname);
        cmdargv.push_back(temp1);
    }
    //包含视频
    if(task.video_stream!=-1)
    {
        //视频编码器
        if(resloader_default.Compare(task.vcname)!=0)
        {
            temp1.Format(_T("-vcodec"));
            cmdargv.push_back(temp1);
            temp1.Format(_T("%s"),task.vcname);
            cmdargv.push_back(temp1);
        }
        //是否无需转码
        if(task.vcname.Compare(_T("copy"))!=0)
        {
            //视频比特率
            if(task.vbitrate!=0)
            {
                temp1.Format(_T("-b:v"));
                cmdargv.push_back(temp1);
                temp1.Format(_T("%dk"),task.vbitrate);
                cmdargv.push_back(temp1);
            }
            //帧率
            if(task.framerate!=0.0)
            {
                temp1.Format(_T("-r"));
                cmdargv.push_back(temp1);
                temp1.Format(_T("%.3f"),task.framerate);
                cmdargv.push_back(temp1);
            }
        }
    }
    else
    {
        temp1.Format(_T("-vn"));
        cmdargv.push_back(temp1);
    }
    //包含音频
    if(task.audio_stream!=-1)
    {
        //音频编码器
        if(resloader_default.Compare(task.acname)!=0)
        {
            temp1.Format(_T("-acodec"));
            cmdargv.push_back(temp1);
            temp1.Format(_T("%s"),task.acname);
            cmdargv.push_back(temp1);
        }
        //是否无需转码
        if(task.acname.Compare(_T("copy"))!=0)
        {
            //音频比特率
            if(task.abitrate!=0)
            {
                temp1.Format(_T("-b:a"));
                cmdargv.push_back(temp1);
                temp1.Format(_T("%dk"),task.abitrate);
                cmdargv.push_back(temp1);
            }

            //音频采样率
            if(task.samplefmt!=0)
            {
                temp1.Format(_T("-ar"));
                cmdargv.push_back(temp1);
                temp1.Format(_T("%d"),task.samplefmt);
                cmdargv.push_back(temp1);
            }
            //音频声道数
            if(task.channels!=0)
            {
                temp1.Format(_T("-ac"));
                cmdargv.push_back(temp1);
                temp1.Format(_T("%d"),task.channels);
                cmdargv.push_back(temp1);
            }
        }
    }
    else
    {
        temp1.Format(_T("-an"));
        cmdargv.push_back(temp1);
    }
    //输出文件路径
    temp1.Format(_T("%s"),task.outfile);
    cmdargv.push_back(temp1);

    return cmdargv;
}

//获取所有的命令行参数
//Get all command's parameter
void CffmpegmfcDlg::GetAllCmdargv()
{
    for(int i=0; i<tasklist.size(); i++)
    {
        vector<CString> cmdargv=GetCmdargv(i);
        cmdargvlist.push_back(cmdargv);
    }
}

//CString的Vector转换为 char **
//Convert CString Vector to char **
char ** CffmpegmfcDlg::CStringListtoCharList(vector<CString> cmdargv)
{
    charlist_size=cmdargv.size();
    charlist=(char **)malloc(charlist_size*sizeof(char *));
    for(int i=0; i<charlist_size; i++)
    {
        //CString的GetLength不包含\0
        int strlen=cmdargv[i].GetLength()+1;
        charlist[i]=(char *)malloc(strlen);

#ifdef _UNICODE
        USES_CONVERSION;
        strcpy(charlist[i],W2A(cmdargv[i]));
#else
        strcpy(charlist[i],cmdargv[i]);
#endif
        TRACE("%s ",charlist[i]);
    }
    TRACE("\n");
    return charlist;
}

//CString的Vector合并为一个CString
//Convert CString Vector to a CString
CString CffmpegmfcDlg::CStringListtoCString(vector<CString> cmdargv)
{
    CString cmd;
    for(int i=0; i<cmdargv.size(); i++)
    {
        cmd.AppendFormat(_T("%s "),cmdargv[i]);

    }
    TRACE("%s",cmd);
    return cmd;
}

//释放一个 char **
//Free a char **
void CffmpegmfcDlg::FreeCharList(char **charlist)
{
    for(int i=0; i<charlist_size; i++)
    {
        //TRACE("%s ",charlist[i]);
        free(charlist[i]);
    }
    free(charlist);
}


//设置进度的消息响应函数
//Callback of Set Progress Message
LRESULT CffmpegmfcDlg::OnSetProgress(WPARAM wParam, LPARAM lParam )
{
    current_process=ffmfcctx->progress;
    SetProgress(current_process,all_process);
    return 0;
}
//设置进度
//Set Progress
void CffmpegmfcDlg::SetProgress(int current,int all)
{
    CString text;
    m_progresscurrent.SetPos(current);
    m_progressall.SetPos(all);
    text.Format(_T("%d%%"),current);
    m_progresscurrentp.SetWindowText(text);
    text.Format(_T("%d%%"),all);
    m_progressallp.SetWindowText(text);
}
//更新列表
//Refresh TaskList
void CffmpegmfcDlg::RefreshList()
{
    //重新载入列表
    m_tasklist.DeleteAllItems();
    int i=0;
    int nIndex=0;
    for(i=0; i<tasklist.size(); i++)
    {
        CString temp_index,tempstr;
        //获取当前记录条数
        nIndex=m_tasklist.GetItemCount();
        //“行”数据结构
        LV_ITEM lvitem;
        lvitem.mask=LVIF_TEXT;
        lvitem.iItem=nIndex;
        lvitem.iSubItem=0;
        //注：vframe_index不可以直接赋值！
        //务必使用f_index执行Format!再赋值！
        temp_index.Format(_T("%d"),nIndex);
        lvitem.pszText=temp_index.GetBuffer();
        //------------------------
        m_tasklist.InsertItem(&lvitem);

        tempstr=URLtoFilename(tasklist[i].infile);
        m_tasklist.SetItemText(nIndex,1,tempstr);

        tempstr=bytetoKBMBGB(tasklist[i].insize);
        m_tasklist.SetItemText(nIndex,2,tempstr);

        tempstr=URLtoFilename(tasklist[i].outfile);
        m_tasklist.SetItemText(nIndex,3,tempstr);

        CString resloader;
        switch(tasklist[i].taskstate)
        {
        case TASK_PREPARE:
        {
            resloader.LoadString(IDS_MAIN_TASKLIST_STATUS_PREPARE);
            tempstr.Format(resloader);
            break;
        }
        case TASK_PROCESS:
        {
            resloader.LoadString(IDS_MAIN_TASKLIST_STATUS_PROCESS);
            tempstr.Format(resloader);
            break;
        }
        case TASK_FINISH:
        {
            resloader.LoadString(IDS_MAIN_TASKLIST_STATUS_FINISH);
            tempstr.Format(resloader);
            break;
        }
        default:
        {
            resloader.LoadString(IDS_MAIN_TASKLIST_STATUS_PREPARE);
            tempstr.Format(resloader);
            break;
        }
        }

        m_tasklist.SetItemText(nIndex,4,tempstr);
    }
    //重绘
    m_tasklist.Invalidate();
}
//提取文件路径中的文件名
//Get filename in an URL
CString CffmpegmfcDlg::URLtoFilename(CString url)
{
    CString filename;
    int nPos=url.ReverseFind('\\');
    filename=url.Mid(nPos+1);
    return filename;
}

//字节单位的文件大小转换为KBMBGB
//Convert bytes to KBMBGB
CString CffmpegmfcDlg::bytetoKBMBGB(int filesize)
{
    //文件大小转换为字符串
    CString filesize_str;
    float filesize_f;
    const int GB = 1024 * 1024 * 1024;//定义GB的计算常量
    const int MB = 1024 * 1024;//定义MB的计算常量
    const int KB = 1024;//定义KB的计算常量

    if (filesize / GB >= 1) //如果当前Byte的值大于等于1GB
    {
        filesize_f=((float)filesize)/((float)GB);
        filesize_str.Format(_T("%.2fGB"),filesize_f);//将其转换成GB
    }
    else if (filesize / MB >= 1)  //如果当前Byte的值大于等于1MB
    {
        filesize_f=((float)filesize)/((float)MB);
        filesize_str.Format(_T("%.2fMB"),filesize_f);//将其转换成MB
    }
    else if (filesize / KB >= 1)  //如果当前Byte的值大于等于1KB
    {
        filesize_f=((float)filesize)/((float)KB);
        filesize_str.Format(_T("%.2fKB"),filesize_f);//将其转换成KGB
    }
    else
    {
        filesize_f=((float)filesize);
        filesize_str.Format(_T("%.2fB"),filesize_f);//显示Byte值
    }
    return filesize_str;
}
//“关于”对话框
//"About" Dialog
void CffmpegmfcDlg::OnAbout()
{
    CAboutDlg dlg;
    dlg.DoModal();
}

//“系统信息”对话框
//"System Information" Dialog
void CffmpegmfcDlg::OnSysinfo()
{
    sysinfodlg.DoModal();
}

//语言设置为中文
//Set Language to Chinese
void CffmpegmfcDlg::OnLangCn()
{
    //配置文件路径
    char conf_path[300]= {0};
    //获得exe绝对路径
    GetModuleFileNameA(NULL,(LPSTR)conf_path,300);//
    //获得exe文家夹路径
    strrchr( conf_path, '\\')[0]= '\0';//
    //_getcwd(realpath,MYSQL_S_LENGTH);
    printf("%s",conf_path);
    strcat(conf_path,"\\configure.ini");
    //写入配置文件
    WritePrivateProfileStringA("Settings","language","Chinese",conf_path);

    //重启软件
    char exe_path[300]= {0};
    //获得exe绝对路径
    GetModuleFileNameA(NULL,(LPSTR)exe_path,300);
    ShellExecuteA( NULL,"open",exe_path,NULL,NULL,SW_SHOWNORMAL);

    //先点一下暂停
    OnBnClickedStop();
    //释放资源
    free(ffmfcctx);


    CDialogEx::OnCancel();
}

//语言设置为英文
//Set Language to English
void CffmpegmfcDlg::OnLangEn()
{
    //配置文件路径
    char conf_path[300]= {0};
    //获得exe绝对路径
    GetModuleFileNameA(NULL,(LPSTR)conf_path,300);//
    //获得exe文家夹路径
    strrchr( conf_path, '\\')[0]= '\0';//
    //_getcwd(realpath,MYSQL_S_LENGTH);
    printf("%s",conf_path);
    strcat(conf_path,"\\configure.ini");
    //写入配置文件
    WritePrivateProfileStringA("Settings","language","English",conf_path);

    //重启软件
    char exe_path[300]= {0};
    //获得exe绝对路径
    GetModuleFileNameA(NULL,(LPSTR)exe_path,300);
    ShellExecuteA( NULL,"open",exe_path,NULL,NULL,SW_SHOWNORMAL);

    //先点一下暂停
    OnBnClickedStop();
    //释放资源
    free(ffmfcctx);

    CDialogEx::OnCancel();
}

//双击任务列表（修改任务）
//Double Click Tasklist
void CffmpegmfcDlg::OnDblclkTasklist(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    POSITION pos =  m_tasklist.GetFirstSelectedItemPosition();
    int index = m_tasklist.GetNextSelectedItem(pos);   //取得当前行号

    if(index!=-1)
    {
        OnBnClickedTasklistEdit();
    }

    *pResult = 0;
}

//右键任务列表（弹出菜单）
//Right Click Tasklist
void CffmpegmfcDlg::OnRclickTasklist(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    CPoint p;
    GetCursorPos(&p);//鼠标点的屏幕坐标
    ptasklistsubmenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, p.x, p.y, this);

    *pResult = 0;
}

//拖拽文件
//Drop file
void CffmpegmfcDlg::OnDropFiles(HDROP hDropInfo)
{

    CDialogEx::OnDropFiles(hDropInfo);
    LPTSTR pFilePathName =(LPTSTR)malloc(MAX_PATH);
    ::DragQueryFile(hDropInfo, 0, pFilePathName,MAX_PATH);  // 获取拖放文件的完整文件名，最关键！
    OnBnClickedTasklistAdd();

    taskinfodlg->SetInputandOutput(pFilePathName);
    ::DragFinish(hDropInfo);   // 注意这个不能少，它用于释放Windows 为处理文件拖放而分配的内存
    free(pFilePathName);

}

//网站
//Website
void CffmpegmfcDlg::OnWebsite()
{
    ShellExecuteA( NULL,"open","http://blog.csdn.net/leixiaohua1020",NULL,NULL,SW_SHOWNORMAL);
}

//退出
//Exit
void CffmpegmfcDlg::OnIdcancel()
{
    CString resloader;
    resloader.LoadString(IDS_MSGBOX_EXIT);
    if(IDOK==AfxMessageBox(resloader,MB_OKCANCEL|MB_ICONINFORMATION))
    {

        //先点一下暂停
        OnBnClickedStop();

        //释放资源
        free(ffmfcctx);
        CDialogEx::OnCancel();
    }
}


void CffmpegmfcDlg::OnOuttranscode()
{
    //配置文件路径
    char conf_path[300]= {0};
    //获得exe绝对路径
    GetModuleFileNameA(NULL,(LPSTR)conf_path,300);//
    //获得exe文家夹路径
    strrchr( conf_path, '\\')[0]= '\0';//
    //_getcwd(realpath,MYSQL_S_LENGTH);
    printf("%s",conf_path);
    strcat(conf_path,"\\configure.ini");
    //写入配置文件
    WritePrivateProfileStringA("Settings","transcodemode","External",conf_path);

    transcodemode=MODE_EXTERNAL;
}


void CffmpegmfcDlg::OnIntranscode()
{
    //配置文件路径
    char conf_path[300]= {0};
    //获得exe绝对路径
    GetModuleFileNameA(NULL,(LPSTR)conf_path,300);//
    //获得exe文家夹路径
    strrchr( conf_path, '\\')[0]= '\0';//
    //_getcwd(realpath,MYSQL_S_LENGTH);
    printf("%s",conf_path);
    strcat(conf_path,"\\configure.ini");
    //写入配置文件
    WritePrivateProfileStringA("Settings","transcodemode","Internal",conf_path);

    transcodemode=MODE_INTERNAL;
}

void CffmpegmfcDlg::LoadTranscodemode()
{
    //配置文件路径
    char conf_path[300]= {0};
    //获得exe绝对路径
    GetModuleFileNameA(NULL,(LPSTR)conf_path,300);//
    //获得exe文家夹路径
    strrchr( conf_path, '\\')[0]= '\0';//
    strcat(conf_path,"\\configure.ini");
    //存储属性的字符串
    char conf_val[300]= {0};

    if((_access(conf_path, 0 )) == -1 )
    {
        transcodemode=MODE_INTERNAL;
        //配置文件不存在，直接返回
    }
    else
    {
        //转码模式
        GetPrivateProfileStringA("Settings","transcodemode",NULL,conf_val,300,conf_path);
        if(strcmp(conf_val,"Internal")==0)
        {
            transcodemode=MODE_INTERNAL;
        }
        else if(strcmp(conf_val,"External")==0)
        {
            transcodemode=MODE_EXTERNAL;
        }
        else
        {
            transcodemode=MODE_INTERNAL;
        }
    }

    return;
}
