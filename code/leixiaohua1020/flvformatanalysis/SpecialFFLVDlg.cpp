/*
 *
 * FLV 格式分析器
 * FLV Format Analysis
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * FLV封装格式分析工具
 * FLV Container Format Analysis Tools
 *
 *
 */
#include "stdafx.h"
#include "SpecialFFLV.h"
#include "SpecialFFLVDlg.h"
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


// CSpecialFFLVDlg 对话框




CSpecialFFLVDlg::CSpecialFFLVDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CSpecialFFLVDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSpecialFFLVDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_F_FLV_INPUTURL, m_fflvinputurl);
    DDX_Control(pDX, IDC_F_FLV_OUTPUT_A, m_fflvoutputa);
    DDX_Control(pDX, IDC_F_FLV_OUTPUT_V, m_fflvoutputv);
    DDX_Control(pDX, IDC_F_FLV_BASICINFO, m_fflvbasicinfo);
    DDX_Control(pDX, IDC_F_FLV_TAGLIST, m_fflvtaglist);
    DDX_Control(pDX, IDC_F_FLV_TAGLIST_MAXNUM, m_fflvtaglistmaxnum);
    DDX_Control(pDX, IDC_F_FLV_LANG, m_fflvlang);
}

BEGIN_MESSAGE_MAP(CSpecialFFLVDlg, CDialogEx)
    ON_BN_CLICKED(IDC_F_FLV_INPUTURL_OPEN, &CSpecialFFLVDlg::OnBnClickedFFlvInputurlOpen)
    ON_NOTIFY ( NM_CUSTOMDRAW,IDC_F_FLV_TAGLIST, OnCustomdrawMyList )
    ON_BN_CLICKED(IDC_F_FLV_ABOUT, &CSpecialFFLVDlg::OnBnClickedFFlvAbout)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DROPFILES()
    ON_CBN_SELCHANGE(IDC_F_FLV_LANG, &CSpecialFFLVDlg::OnSelchangeFFlvLang)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CSpecialFFLVDlg 消息处理程序

BOOL CSpecialFFLVDlg::OnInitDialog()
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
    //整行选择；有表格线；表头；单击激活
    DWORD dwExStyle=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE;
    //报表风格；单行选择；高亮显示选择行
    //视频


    m_fflvtaglist.ModifyStyle(0,LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS);
    m_fflvtaglist.SetExtendedStyle(dwExStyle);

    text.LoadString(IDS_LIST_TITLE_NUM);
    m_fflvtaglist.InsertColumn(0,text,LVCFMT_CENTER,50,0);
    text.LoadString(IDS_LIST_TITLE_TYPE);
    m_fflvtaglist.InsertColumn(1,text,LVCFMT_CENTER,75,0);
    text.LoadString(IDS_LIST_TITLE_DATASIZE);
    m_fflvtaglist.InsertColumn(2,text,LVCFMT_CENTER,75,0);
    text.LoadString(IDS_LIST_TITLE_TIMESTAMP);
    m_fflvtaglist.InsertColumn(3,text,LVCFMT_CENTER,75,0);
    text.LoadString(IDS_LIST_TITLE_STREAMID);
    m_fflvtaglist.InsertColumn(4,text,LVCFMT_CENTER,75,0);
    text.LoadString(IDS_LIST_TITLE_TAGDATAFB);
    m_fflvtaglist.InsertColumn(5,text,LVCFMT_CENTER,150,0);


    //Propertylist初始化------------------------
    m_fflvbasicinfo.EnableHeaderCtrl(FALSE);
    m_fflvbasicinfo.EnableDescriptionArea();
    m_fflvbasicinfo.SetVSDotNetLook();
    m_fflvbasicinfo.MarkModifiedProperties();
    //把第一列调整宽一些-----------------------
    HDITEM item;
    item.cxy=90;
    item.mask=HDI_WIDTH;
    m_fflvbasicinfo.GetHeaderCtrl().SetItem(0, &item);

    //----
    Headergroup=new CMFCPropertyGridProperty(_T("File Header"));
    //-----------
    tl_index=0;
    m_fflvtaglistmaxnum.SetCheck(1);
    //-------------
    m_fflvinputurl.EnableFileBrowseButton(
        NULL,
        _T("FLV Files (*.flv)|*.flv|All Files (*.*)|*.*||")
    );
    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码
    m_fflvlang.InsertString(0,_T("Chinese"));
    m_fflvlang.InsertString(1,_T("English"));

    //_CrtSetBreakAlloc(822);

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSpecialFFLVDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSpecialFFLVDlg::OnPaint()
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
HCURSOR CSpecialFFLVDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}



void CSpecialFFLVDlg::OnBnClickedFFlvInputurlOpen()
{
    SystemClear();
    CString strFilePath;
    m_fflvinputurl.GetWindowText(strFilePath);

#ifdef _UNICODE
    USES_CONVERSION;
    strcpy(fileurl,W2A(strFilePath));
#else
    strcpy(fileurl,strFilePath);
#endif


    flvparse(this,fileurl);

    ShowBInfo();
}
//添加一条记录
//每个字段的含义：类型，数据大小，时间戳，streamid，data的第一个字节
int CSpecialFFLVDlg::AppendTLInfo(int type,int datasize,int timestamp,int streamid,char data_f_b)
{
    //如果选择了“最多输出5000条”，判断是否超过5000条
    if(m_fflvtaglistmaxnum.GetCheck()==1&&tl_index>5000)
    {
        return 0;
    }
    CString temp_index,temp_type,temp_datasize,temp_timestamp,temp_streamid;
    CString temp_tagdatafirstbyte;

    unsigned char x;

    int nIndex=0;
    switch(type)
    {
    case 8:
    {
        text.LoadString(IDS_UNKNOWN);
        x=data_f_b&0xF0;
        x=x>>4;
        switch (x)
        {
        case 0:
            temp_tagdatafirstbyte.AppendFormat(_T("Linear PCM, platform endian"));
            break;
        case 1:
            temp_tagdatafirstbyte.AppendFormat(_T("ADPCM"));
            break;
        case 2:
            temp_tagdatafirstbyte.AppendFormat(_T("MP3"));
            break;
        case 3:
            temp_tagdatafirstbyte.AppendFormat(_T("Linear PCM, little endian"));
            break;
        case 4:
            temp_tagdatafirstbyte.AppendFormat(_T("Nellymoser 16-kHz mono"));
            break;
        case 5:
            temp_tagdatafirstbyte.AppendFormat(_T("Nellymoser 8-kHz mono"));
            break;
        case 6:
            temp_tagdatafirstbyte.AppendFormat(_T("Nellymoser"));
            break;
        case 7:
            temp_tagdatafirstbyte.AppendFormat(_T("G.711 A-law logarithmic PCM"));
            break;
        case 8:
            temp_tagdatafirstbyte.AppendFormat(_T("G.711 mu-law logarithmic PCM"));
            break;
        case 9:
            temp_tagdatafirstbyte.AppendFormat(_T("reserved"));
            break;
        case 10:
            temp_tagdatafirstbyte.AppendFormat(_T("AAC"));
            break;
        case 11:
            temp_tagdatafirstbyte.AppendFormat(_T("Speex"));
            break;
        case 14:
            temp_tagdatafirstbyte.AppendFormat(_T("MP3 8-Khz"));
            break;
        case 15:
            temp_tagdatafirstbyte.AppendFormat(_T("Device-specific sound"));
            break;
        default:
            temp_tagdatafirstbyte.AppendFormat(_T("%s"),text);
            break;
        }
        temp_tagdatafirstbyte.AppendFormat(_T("|"));
        x=data_f_b&0x0C;
        x=x>>2;
        switch (x)
        {
        case 0:
            temp_tagdatafirstbyte.AppendFormat(_T("5.5-kHz"));
            break;
        case 1:
            temp_tagdatafirstbyte.AppendFormat(_T("11-kHz"));
            break;
        case 2:
            temp_tagdatafirstbyte.AppendFormat(_T("22-kHz"));
            break;
        case 3:
            temp_tagdatafirstbyte.AppendFormat(_T("44-kHz"));
            break;
        default:
            temp_tagdatafirstbyte.AppendFormat(_T("%s"),text);
            break;
        }
        temp_tagdatafirstbyte.AppendFormat(_T("|"));
        x=data_f_b&0x02;
        x=x>>1;
        switch (x)
        {
        case 0:
            temp_tagdatafirstbyte.AppendFormat(_T("8Bit"));
            break;
        case 1:
            temp_tagdatafirstbyte.AppendFormat(_T("16Bit"));
            break;
        default:
            temp_tagdatafirstbyte.AppendFormat(_T("%s"),text);
            break;
        }
        temp_tagdatafirstbyte.AppendFormat(_T("|"));
        x=data_f_b&0x01;
        switch (x)
        {
        case 0:
            temp_tagdatafirstbyte.AppendFormat(_T("Mono"));
            break;
        case 1:
            temp_tagdatafirstbyte.AppendFormat(_T("Stereo"));
            break;
        default:
            temp_tagdatafirstbyte.AppendFormat(_T("%s"),text);
            break;
        }
        text.LoadString(IDS_LIST_DATA_TYPE_AUDIO);
        break;
    }
    case 9:
    {
        text.LoadString(IDS_UNKNOWN);
        x=data_f_b&0xF0;
        x=x>>4;
        switch (x)
        {
        case 1:
            temp_tagdatafirstbyte.AppendFormat(_T("key frame"));
            break;
        case 2:
            temp_tagdatafirstbyte.AppendFormat(_T("inter frame"));
            break;
        case 3:
            temp_tagdatafirstbyte.AppendFormat(_T("disposable inter frame"));
            break;
        case 4:
            temp_tagdatafirstbyte.AppendFormat(_T("generated keyframe "));
            break;
        case 5:
            temp_tagdatafirstbyte.AppendFormat(_T("video info/command frame"));
            break;
        default:
            temp_tagdatafirstbyte.AppendFormat(_T("%s"),text);
            break;
        }
        temp_tagdatafirstbyte.AppendFormat(_T("|"));
        x=data_f_b&0x0F;
        switch (x)
        {
        case 1:
            temp_tagdatafirstbyte.AppendFormat(_T("JPEG"));
            break;
        case 2:
            temp_tagdatafirstbyte.AppendFormat(_T("Sorenson H.263"));
            break;
        case 3:
            temp_tagdatafirstbyte.AppendFormat(_T("Screen video"));
            break;
        case 4:
            temp_tagdatafirstbyte.AppendFormat(_T("On2 VP6"));
            break;
        case 5:
            temp_tagdatafirstbyte.AppendFormat(_T("On2 VP6 with alpha channel"));
            break;
        case 6:
            temp_tagdatafirstbyte.AppendFormat(_T("Screen video version 2"));
            break;
        case 7:
            temp_tagdatafirstbyte.AppendFormat(_T("AVC"));
            break;
        default:
            temp_tagdatafirstbyte.AppendFormat(_T("%s"),text);
            break;
        }
        text.LoadString(IDS_LIST_DATA_TYPE_VIDEO);
        break;
    }
    case 18:
    {
        text.LoadString(IDS_LIST_DATA_TYPE_SCRIPT);
        break;
    }
    default:
    {
        text.LoadString(IDS_UNKNOWN);
        break;
    }
    }
    temp_type.Format(_T("%s"),text);
    temp_index.Format(_T("%d"),tl_index);
    temp_datasize.Format(_T("%d"),datasize);
    temp_timestamp.Format(_T("%d"),timestamp);
    temp_streamid.Format(_T("%d"),streamid);
    //获取当前记录条数
    nIndex=m_fflvtaglist.GetItemCount();
    //“行”数据结构
    LV_ITEM lvitem;
    lvitem.mask=LVIF_TEXT;
    lvitem.iItem=nIndex;
    lvitem.iSubItem=0;
    //注：vframe_index不可以直接赋值！
    //务必使用f_index执行Format!再赋值！
    lvitem.pszText=temp_index.GetBuffer();
    //------------------------
    m_fflvtaglist.InsertItem(&lvitem);
    m_fflvtaglist.SetItemText(nIndex,1,temp_type);
    m_fflvtaglist.SetItemText(nIndex,2,temp_datasize);
    m_fflvtaglist.SetItemText(nIndex,3,temp_timestamp);
    m_fflvtaglist.SetItemText(nIndex,4,temp_streamid);
    m_fflvtaglist.SetItemText(nIndex,5,temp_tagdatafirstbyte);
    tl_index++;
    return TRUE;
}
int CSpecialFFLVDlg::AppendBInfo(CString dst_group,CString property_name,CString value,CString remark)
{
    if(dst_group.Compare(_T("Header"))==0)
    {
        Headergroup->AddSubItem(new CMFCPropertyGridProperty(property_name, (_variant_t)value, remark));
    }
    return TRUE;
}
int CSpecialFFLVDlg::ShowBInfo()
{
    m_fflvbasicinfo.AddProperty(Headergroup);
    return TRUE;
}

void CSpecialFFLVDlg::SystemClear()
{
    Headergroup->RemoveAllOptions();

    Headergroup=new CMFCPropertyGridProperty(_T("Header"));
    m_fflvbasicinfo.RemoveAll();
    m_fflvtaglist.DeleteAllItems();
    tl_index=0;
}
//ListCtrl加颜色
void CSpecialFFLVDlg::OnCustomdrawMyList ( NMHDR* pNMHDR, LRESULT* pResult )
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

        CString strTemp = m_fflvtaglist.GetItemText(nItem,1);
        CString text_video,text_audio,text_script;
        text_video.LoadString(IDS_LIST_DATA_TYPE_VIDEO);
        text_audio.LoadString(IDS_LIST_DATA_TYPE_AUDIO);
        text_script.LoadString(IDS_LIST_DATA_TYPE_SCRIPT);

        if(strTemp.Compare(text_video)==0)
        {
            clrNewTextColor = RGB(0,0,0);		//Set the text
            clrNewBkColor = RGB(0,255,255);		//青色
        }
        else if(strTemp.Compare(text_audio)==0)
        {
            clrNewTextColor = RGB(0,0,0);		//text
            clrNewBkColor = RGB(255,255,0);		//黄色
        }
        else if(strTemp.Compare(text_script)==0)
        {
            clrNewTextColor = RGB(0,0,0);		//text
            clrNewBkColor = RGB(255,153,0);		//咖啡色
        }
        else
        {
            clrNewTextColor = RGB(0,0,0);		//text
            clrNewBkColor = RGB(255,255,255);			//白色
        }

        pLVCD->clrText = clrNewTextColor;
        pLVCD->clrTextBk = clrNewBkColor;


        // Tell Windows to paint the control itself.
        *pResult = CDRF_DODEFAULT;


    }
}

void CSpecialFFLVDlg::OnBnClickedFFlvAbout()
{
    CAboutDlg dlg;
    dlg.DoModal();
}


void CSpecialFFLVDlg::OnDropFiles(HDROP hDropInfo)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CDialogEx::OnDropFiles(hDropInfo);
    LPTSTR pFilePathName =(LPTSTR)malloc(MAX_URL_LENGTH);
    ::DragQueryFile(hDropInfo, 0, pFilePathName,MAX_URL_LENGTH);  // 获取拖放文件的完整文件名，最关键！
    m_fflvinputurl.SetWindowText(pFilePathName);
    ::DragFinish(hDropInfo);   // 注意这个不能少，它用于释放Windows 为处理文件拖放而分配的内存
    free(pFilePathName);
}


void CSpecialFFLVDlg::OnSelchangeFFlvLang()
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
    switch(m_fflvlang.GetCurSel())
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
    OnCancel();
}


//void CAboutDlg::OnDestroy()
//{
//	CDialogEx::OnDestroy();
//
//	// TODO: 在此处添加消息处理程序代码
//}


void CSpecialFFLVDlg::OnDestroy()
{

    CDialogEx::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
}
