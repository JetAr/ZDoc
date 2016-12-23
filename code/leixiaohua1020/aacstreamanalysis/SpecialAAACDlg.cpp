/*
 * AAC分析器
 * AAC Analysis
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 */
#include "stdafx.h"
#include "SpecialAAAC.h"
#include "SpecialAAACDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
public:
//	afx_msg void OnDestroy();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CSpecialAAACDlg 对话框




CSpecialAAACDlg::CSpecialAAACDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CSpecialAAACDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSpecialAAACDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_A_AAC_BASICINFO, m_aaacbasicinfo);
    DDX_Control(pDX, IDC_A_AAC_INPUTURL, m_aaacinputurl);
    //  DDX_Control(pDX, IDC_A_AAC_CONSOLE, m_aaacconsole);
    DDX_Control(pDX, IDC_A_AAC_OUTPUT_FMT, m_aaacoutputfmt);
    DDX_Control(pDX, IDC_A_AAC_OUTPUT_SR, m_aaacoutputsr);
    DDX_Control(pDX, IDC_A_AAC_LANG, m_aaaclang);
    DDX_Control(pDX, IDC_A_AAC_PROGRESS, m_aaacprogress);
    DDX_Control(pDX, IDC_A_AAC_PROGRESS_TEXT, m_aaacprogresstext);
    DDX_Control(pDX, IDC_A_AAC_LIST_MAXNUM, m_aaaclistmaxnum);
    DDX_Control(pDX, IDC_A_AAC_LIST, m_aaaclist);
}

BEGIN_MESSAGE_MAP(CSpecialAAACDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_A_AAC_INPUTURL_OPEN, &CSpecialAAACDlg::OnBnClickedAAacInputurlOpen)
    ON_BN_CLICKED(IDC_A_AAC_ABOUT, &CSpecialAAACDlg::OnBnClickedAAacAbout)
    ON_WM_DROPFILES()
    ON_CBN_SELCHANGE(IDC_A_AAC_LANG, &CSpecialAAACDlg::OnSelchangeAAacLang)
    ON_MESSAGE(WM_REFRESHUI,&CSpecialAAACDlg::OnRefreshUI)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

//处理线程
UINT Thread_Process(LPVOID lpParam)
{

    CSpecialAAACDlg *dlg=(CSpecialAAACDlg *)lpParam;

    int argc=6;
    char **argv=(char **)malloc(MAX_URL_LENGTH);
    argv[0]=(char *)malloc(MAX_URL_LENGTH);
    argv[1]=(char *)malloc(MAX_URL_LENGTH);
    argv[2]=(char *)malloc(MAX_URL_LENGTH);
    argv[3]=(char *)malloc(MAX_URL_LENGTH);
    argv[4]=(char *)malloc(MAX_URL_LENGTH);
    argv[5]=(char *)malloc(MAX_URL_LENGTH);

    strcpy(argv[0],"dummy");

    strcpy(argv[1],"-f");

    switch(dlg->m_aaacoutputfmt.GetCurSel())
    {
    case 0:
        strcpy(argv[2],"1");
        break;
    case 1:
        strcpy(argv[2],"2");
        break;
    default:
        strcpy(argv[2],"1");
        break;
    }
    strcpy(argv[3],"-b");
    switch(dlg->m_aaacoutputsr.GetCurSel())
    {
    case 0:
        strcpy(argv[4],"1");
        break;
    case 1:
        strcpy(argv[4],"2");
        break;
    case 2:
        strcpy(argv[4],"3");
        break;
    case 3:
        strcpy(argv[4],"4");
        break;
    case 4:
        strcpy(argv[4],"5");
        break;
    default:
        strcpy(argv[4],"1");
        break;
    }
    strcpy(argv[5],dlg->fileurl);
    int retval=aac_decode(dlg,argc,argv);
    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);
    free(argv[4]);
    free(argv[5]);
    free(argv);

    if(retval!=0)
        return -1;

    //dlg->ShowBInfo();
    dlg->SendMessage(WM_REFRESHUI,0,0);
    //FIX:100%
    dlg->m_aaacprogress.SetPos(100);
    CString progresstext(_T("100%"));
    dlg->m_aaacprogresstext.SetWindowText(progresstext);


    return 0;
}


// CSpecialAAACDlg 消息处理程序

BOOL CSpecialAAACDlg::OnInitDialog()
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
    //----------
    m_aaacbasicinfo.EnableHeaderCtrl(FALSE);
    m_aaacbasicinfo.SetVSDotNetLook();
    //把第一列调整宽一些-----------------------
    HDITEM item;
    item.cxy=120;
    item.mask=HDI_WIDTH;
    m_aaacbasicinfo.GetHeaderCtrl().SetItem(0, &item);
    text.LoadString(IDS_INFO_FRAMEINFO);
    FrameInfogroup=new CMFCPropertyGridProperty(text);
    text.LoadString(IDS_INFO_CONTAINERINFO);
    Containergroup=new CMFCPropertyGridProperty(text);
    text.LoadString(IDS_INFO_METAINFO);
    MetaDatagroup=new CMFCPropertyGridProperty(text);
    cinfo.Format(_T(""));
    UpdateData(FALSE);

    //------------

    DWORD dwExStyle=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE;

    m_aaaclist.ModifyStyle(0,LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS);
    m_aaaclist.SetExtendedStyle(dwExStyle);

    m_aaaclist.InsertColumn(0,_T("Index"),LVCFMT_CENTER,50,0);
    m_aaaclist.InsertColumn(1,_T("Size(B)"),LVCFMT_CENTER,50,0);
    m_aaaclist.InsertColumn(2,_T("Sample Num"),LVCFMT_CENTER,70,0);
    m_aaaclist.InsertColumn(3,_T("Duration"),LVCFMT_CENTER,60,0);
    m_aaaclist.InsertColumn(4,_T("Ele Num"),LVCFMT_CENTER,50,0);
    m_aaaclist.InsertColumn(5,_T("Ele Name"),LVCFMT_CENTER,60,0);
    m_aaaclist.InsertColumn(6,_T("Ele Info"),LVCFMT_CENTER,100,0);

    m_aaacoutputfmt.InsertString(0,_T("Microsoft WAV format"));
    m_aaacoutputfmt.InsertString(1,_T("RAW PCM data"));

    m_aaacoutputsr.InsertString(0,_T("16 bit PCM data"));
    m_aaacoutputsr.InsertString(1,_T("24 bit PCM data"));
    m_aaacoutputsr.InsertString(2,_T("32 bit PCM data"));
    m_aaacoutputsr.InsertString(3,_T("32 bit floating point data"));
    m_aaacoutputsr.InsertString(4,_T("64 bit floating point data"));

    m_aaacoutputfmt.SetCurSel(0);
    m_aaacoutputsr.SetCurSel(0);

    l_index=0;
    m_aaaclistmaxnum.SetCheck(1);

    //--------------------
    m_aaacinputurl.EnableFileBrowseButton(
        NULL,
        _T("AAC Raw Files (*.aac)|*.aac|M4A Files (*.m4a)|*.m4a|All Files (*.*)|*.*||")
    );
    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码
    m_aaaclang.InsertString(0,_T("Chinese"));
    m_aaaclang.InsertString(1,_T("English"));
    //---------
    m_aaacprogress.SetRange(0,100);
    m_aaacprogress.SetPos(0);
    m_aaacprogresstext.SetWindowText(_T("0%"));

    //_CrtSetBreakAlloc(822);

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSpecialAAACDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSpecialAAACDlg::OnPaint()
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
    //画对话框不应该添加到OnInitDialog中，否则没有效果
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSpecialAAACDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


// SpecialAAAC 消息处理程序


void CSpecialAAACDlg::OnBnClickedAAacInputurlOpen()
{

    //---------
    SystemClear();
    CString strFilePath;
    //m_aaacinputurl.GetWindowText(fileurl,MAX_URL_LENGTH);
    m_aaacinputurl.GetWindowText(strFilePath);

    if(strFilePath.IsEmpty()==TRUE)
    {
        text.LoadString(IDS_INPUTURL_ERROR);
        AfxMessageBox(text);
        return ;
    }

#ifdef _UNICODE
    USES_CONVERSION;
    strcpy(fileurl,W2A(strFilePath));
#else
    strcpy(fileurl,strFilePath);
#endif
    UpdateData(TRUE);

    m_aaacprogress.SetPos(0);

    pThreadProcess=AfxBeginThread(Thread_Process,this);//开启线程

}

int CSpecialAAACDlg::AppendBInfo(CString dst_group,CString property_name,CString value,CString remark)
{
    if(dst_group.Compare(_T("FrameInfo"))==0)
    {
        FrameInfogroup->AddSubItem(new CMFCPropertyGridProperty(property_name,(variant_t)value, remark));
    }
    else if(dst_group.Compare(_T("Container"))==0)
    {
        Containergroup->AddSubItem(new CMFCPropertyGridProperty(property_name, (variant_t)value, remark));
    }
    else if(dst_group.Compare(_T("MetaData"))==0)
    {
        MetaDatagroup->AddSubItem(new CMFCPropertyGridProperty(property_name, (variant_t)value, remark));
    }
    return 0;
}

int CSpecialAAACDlg::ShowBInfo()
{
    m_aaacbasicinfo.AddProperty(FrameInfogroup);
    m_aaacbasicinfo.AddProperty(Containergroup);
    m_aaacbasicinfo.AddProperty(MetaDatagroup);
    return TRUE;
}

//设置CInfo
int CSpecialAAACDlg::AppendLInfo(int size,int samplenum,int duration,int elenum,char *elename,char *eleinfo)
{

    CString temp_index,temp_size,temp_samplenum,temp_duration,temp_elenum,temp_elename,temp_eleinfo;

    if(m_aaaclistmaxnum.GetCheck()==1&&l_index>5000)
    {
        return 0;
    }
    int nIndex=0;

    temp_index.Format(_T("%d"),l_index);
    temp_size.Format(_T("%d"),size);
    temp_samplenum.Format(_T("%d"),samplenum);
    temp_duration.Format(_T("%dms"),duration);
    temp_elenum.Format(_T("%d"),elenum);
    temp_elename.Format(_T("%s"),elename);
    temp_eleinfo.Format(_T("%s"),eleinfo);

    //获取当前记录条数
    nIndex=m_aaaclist.GetItemCount();
    //“行”数据结构
    LV_ITEM lvitem;
    lvitem.mask=LVIF_TEXT;
    lvitem.iItem=nIndex;
    lvitem.iSubItem=0;
    //注：vframe_index不可以直接赋值！
    //务必使用f_index执行Format!再赋值！
    lvitem.pszText=temp_index.GetBuffer();
    //------------------------
    m_aaaclist.InsertItem(&lvitem);
    m_aaaclist.SetItemText(nIndex,1,temp_size);
    m_aaaclist.SetItemText(nIndex,2,temp_samplenum);
    m_aaaclist.SetItemText(nIndex,3,temp_duration);
    m_aaaclist.SetItemText(nIndex,4,temp_elenum);
    m_aaaclist.SetItemText(nIndex,5,temp_elename);
    m_aaaclist.SetItemText(nIndex,6,temp_eleinfo);
    l_index++;

    return 0;
}

void CSpecialAAACDlg::SystemClear()
{
    cinfo.Format(_T(""));

    l_index=0;
    m_aaaclist.DeleteAllItems();

    FrameInfogroup->RemoveAllOptions();
    Containergroup->RemoveAllOptions();
    MetaDatagroup->RemoveAllOptions();

    m_aaacbasicinfo.DeleteProperty(FrameInfogroup);
    m_aaacbasicinfo.DeleteProperty(Containergroup);
    m_aaacbasicinfo.DeleteProperty(MetaDatagroup);

    delete FrameInfogroup;
    delete Containergroup;
    delete MetaDatagroup;

    text.LoadString(IDS_INFO_FRAMEINFO);
    FrameInfogroup=new CMFCPropertyGridProperty(text);
    text.LoadString(IDS_INFO_CONTAINERINFO);
    Containergroup=new CMFCPropertyGridProperty(text);
    text.LoadString(IDS_INFO_METAINFO);
    MetaDatagroup=new CMFCPropertyGridProperty(text);

}

void CSpecialAAACDlg::OnBnClickedAAacAbout()
{
    CAboutDlg dlg;
    dlg.DoModal();
}


void CSpecialAAACDlg::OnDropFiles(HDROP hDropInfo)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CDialogEx::OnDropFiles(hDropInfo);
    LPTSTR pFilePathName =(LPTSTR)malloc(MAX_URL_LENGTH);
    ::DragQueryFile(hDropInfo, 0, pFilePathName,MAX_URL_LENGTH);  // 获取拖放文件的完整文件名，最关键！
    m_aaacinputurl.SetWindowText(pFilePathName);
    ::DragFinish(hDropInfo);   // 注意这个不能少，它用于释放Windows 为处理文件拖放而分配的内存
    free(pFilePathName);
}


void CSpecialAAACDlg::OnSelchangeAAacLang()
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
    switch(m_aaaclang.GetCurSel())
    {
    case 0:
        WritePrivateProfileStringA("Settings","language","Chinese",conf_path);
        break;
    case 1:
        WritePrivateProfileStringA("Settings","language","English",conf_path);
        break;
    default:
        break;
    }
    //重启软件
    char exe_path[300]= {0};
    //获得exe绝对路径
    GetModuleFileNameA(NULL,(LPSTR)exe_path,300);
    ShellExecuteA( NULL,"open",exe_path,NULL,NULL,SW_SHOWNORMAL);
    exit(0);
}


LRESULT CSpecialAAACDlg::OnRefreshUI(WPARAM wparam,LPARAM lparam)
{
    ShowBInfo();
    return 0;
}

void CSpecialAAACDlg::OnDestroy()
{
    if(FrameInfogroup&&Containergroup&&MetaDatagroup)
    {
        FrameInfogroup->RemoveAllOptions();
        Containergroup->RemoveAllOptions();
        MetaDatagroup->RemoveAllOptions();

        m_aaacbasicinfo.DeleteProperty(FrameInfogroup);
        m_aaacbasicinfo.DeleteProperty(Containergroup);
        m_aaacbasicinfo.DeleteProperty(MetaDatagroup);

        delete FrameInfogroup;
        delete Containergroup;
        delete MetaDatagroup;
    }
    CDialogEx::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
}
