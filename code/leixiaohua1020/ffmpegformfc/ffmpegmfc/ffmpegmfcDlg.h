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



#include "TaskinfoDlg.h"
#include "SysinfoDlg.h"



// CffmpegmfcDlg 对话框
class CffmpegmfcDlg : public CDialogEx
{
// 构造
public:
    CffmpegmfcDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
    enum { IDD = IDD_FFMPEGMFC_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
    HICON m_hIcon;

    CWinThread *pThreadffmpeg;

    //初始化
    //Init Dialog
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:

    //Main Button
    //“开始”按钮
    //"Start" Button
    afx_msg void OnBnClickedStart();
    //“暂停”按钮
    //"Pause" Button
    afx_msg void OnBnClickedPause();
    //“停止”按钮
    //"Stop" Button
    afx_msg void OnBnClickedStop();
    //任务列表的“添加”按钮
    //TaskList's "Add" Button
    afx_msg void OnBnClickedTasklistAdd();
    //任务列表的“编辑”按钮
    //TaskList's "Add" Button
    afx_msg void OnBnClickedTasklistEdit();
    //任务列表的“删除”按钮
    //TaskList's "Delete" Button
    afx_msg void OnBnClickedTasklistDelete();
    //任务列表的“清空”按钮
    //TaskList's "Clear" Button
    afx_msg void OnBnClickedTasklistClear();
    //任务列表加颜色
    //TaskList's Color
    void OnCustomdrawMyList ( NMHDR* pNMHDR, LRESULT* pResult );
    //系统复位
    //Reset System
    void SystemClear();


    Systemstate state;
    //根据系统状态设置按钮
    //Set Buuton's state
    void SetBtn(Systemstate state);

    //TaskList
    vector<Task> tasklist;
    //添加一个任务
    //Add a task
    void TasklistAdd(Task task);
    //编辑一个任务
    //Edit a task
    void TasklistEdit(Task task,int taskid);
    //CmdStr
    //获取一个命令行参数
    //Get a command's parameter
    vector<CString> GetCmdargv(int taskid);
    //获取所有的命令行参数
    //Get all command's parameter
    void GetAllCmdargv();
    vector<vector<CString>> cmdargvlist;
    char **charlist;
    int charlist_size;
    //CString的Vector转换为 char **
    //Convert CString Vector to char **
    char ** CStringListtoCharList(vector<CString> cmdargv);
    //CString的Vector合并为一个CString
    //Convert CString Vector to a CString
    CString CStringListtoCString(vector<CString> cmdargv);
    //释放一个 char **
    //Free a char **
    void FreeCharList(char **charlist);

    //ProgressBar
    int current_process;
    int all_process;
    //设置进度的消息响应函数
    //Callback of Set Progress Message
    LRESULT OnSetProgress(WPARAM wParam, LPARAM lParam );
    //设置进度
    //Set Progress
    void SetProgress(int current,int all);
    CProgressCtrl m_progresscurrent;
    CProgressCtrl m_progressall;
    CEdit m_progressallp;
    CEdit m_progresscurrentp;

    //ListCtrl
    //更新列表
    //Refresh TaskList
    void RefreshList();
    //提取文件路径中的文件名
    //Get filename in an URL
    CString URLtoFilename(CString url);
    //字节单位的文件大小转换为KBMBGB
    //Convert bytes to KBMBGB
    CString bytetoKBMBGB(int filesize);
    CListCtrl m_tasklist;

    //Dialog
    TaskinfoDlg *taskinfodlg;
    SysinfoDlg sysinfodlg;

    //TaskList菜单
    CMenu tasklistmenu;
    CMenu *ptasklistsubmenu;
    //Menu
    //“关于”对话框
    //"About" Dialog
    afx_msg void OnAbout();
    //“系统信息”对话框
    //"System Information" Dialog
    afx_msg void OnSysinfo();
    //语言设置为中文
    //Set Language to Chinese
    afx_msg void OnLangCn();
    //语言设置为英文
    //Set Language to English
    afx_msg void OnLangEn();
    //双击任务列表（修改任务）
    //Double Click Tasklist
    afx_msg void OnDblclkTasklist(NMHDR *pNMHDR, LRESULT *pResult);
    //右键任务列表（弹出菜单）
    //Right Click Tasklist
    afx_msg void OnRclickTasklist(NMHDR *pNMHDR, LRESULT *pResult);
    //拖拽文件
    //Drop file
    afx_msg void OnDropFiles(HDROP hDropInfo);
    //网站
    //Website
    afx_msg void OnWebsite();
    //退出
    //Exit
    afx_msg void OnIdcancel();

    afx_msg void OnOuttranscode();
    afx_msg void OnIntranscode();
    //模式
    void LoadTranscodemode();
    Transcodemode transcodemode;
};
