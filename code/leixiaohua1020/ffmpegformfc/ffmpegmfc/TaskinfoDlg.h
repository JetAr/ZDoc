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
#pragma once


// TaskinfoDlg 对话框

// Task 信息的编辑
// Edit Task Information
class CffmpegmfcDlg;

class TaskinfoDlg : public CDialogEx
{
    DECLARE_DYNAMIC(TaskinfoDlg)

public:
    TaskinfoDlg(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~TaskinfoDlg();

// 对话框数据
    enum { IDD = IDD_TASKINFO_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    //对话框初始化
    //Init Dialog
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()
public:
    //CMFCPropertyGridCtrl
    CMFCPropertyGridCtrl m_taskinfoparam;
    CMFCPropertyGridProperty *format;
    CMFCPropertyGridProperty *format_if;
    CMFCPropertyGridProperty *format_of;
    CMFCPropertyGridProperty *video;
    CMFCPropertyGridProperty *video_bitrate;
    CMFCPropertyGridProperty *video_codec;
    CMFCPropertyGridProperty *video_framerate;
    CMFCPropertyGridProperty *video_reso;
    CMFCPropertyGridProperty *video_reso_w;
    CMFCPropertyGridProperty *video_reso_h;
    CMFCPropertyGridProperty *audio;
    CMFCPropertyGridProperty *audio_bitrate;
    CMFCPropertyGridProperty *audio_codec;
    CMFCPropertyGridProperty *audio_samplerate;
    CMFCPropertyGridProperty *audio_channels;


    //编辑的Task
    //Task
    TaskDlgState state;
    int tasklistid;
    Task task;
    //主对话框
    //Main Dialog
    CffmpegmfcDlg *maindlg;

    //封装格式，编解码器名称信息
    //Container formats, video codecs, audio codecs list
    vector<CString> iflist;
    vector<CString> oflist;
    vector<CString> vclist;
    vector<CString> aclist;

    //获取支持的封装格式等信息列表
    //Get support container format, video codec, audio codec etc.
    void GetNamelist();

    //设置Task
    //Set Task
    void SetTask();
    //加载Task
    //Load Task
    void LoadTask(TaskDlgState state_temp,int tasklistid_temp);
    //确定按钮
    //"OK" Button
    afx_msg void OnBnClickedOk();
    //取消按钮
    //"Cancel" Button
    afx_msg void OnBnClickedCancel();
    //拖拽文件
    //Drop file
    afx_msg void OnDropFiles(HDROP hDropInfo);
    //检查输入文件信息
    //Check input file information
    BOOL CheckInputMedia(CString url);

    CEdit m_codecachannels;
    CEdit m_codecaname;
    CEdit m_codecasamplerate;
    CEdit m_codecvframerate;
    CEdit m_codecvname;
    CEdit m_codecvresolution;
    CEdit m_formatbitrate;
    CEdit m_formatduration;
    CEdit m_formatinputformat;


    //Input/Output
    CEdit m_taskinfoinfile;
    CEdit m_taskinfooutfile;
    //“打开”文件对话框
    //"Open" Dialog
    afx_msg void OnBnClickedTaskinfoInfileFdlg();
    //“保存”文件对话框
    //"Save" Dialog
    afx_msg void OnBnClickedTaskinfoOutfileFdlg();
    //设置输入文件路径，并设置预设的输出文件路径
    //Set input and output URL
    void SetInputandOutput(CString inputurl);
    //视频流/音频流ID
    int	video_stream;
    int	audio_stream;

};
