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
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

#include <io.h>

#include <vector>
using std::vector;
//--------------------
/*
 *
 * 任务编辑对话框
 * 编辑还是添加？
 *
 */
typedef enum TaskDlgState
{
    TASK_DLG_ADD,
    TASK_DLG_EDIT
} TaskDlgState;


/*
 *
 * 系统状态
 *
 */
typedef enum Systemstate
{
    SYSTEM_PREPARE,
    SYSTEM_PROCESS,
    SYSTEM_PAUSE
} Systemstate;

/*
 *
 * 任务状态
 *
 */
typedef enum Taskstate
{
    TASK_PREPARE,
    TASK_PROCESS,
    TASK_FINISH
} Taskstate;

/*
 *
 * 转码模式
 *
 */
typedef enum Transcodemode
{
    MODE_INTERNAL,
    MODE_EXTERNAL
} Transcodemode;


/*
 *
 * 任务
 *
 */
typedef struct Task
{
    CString infile;
    long long insize;
    CString outfile;
    CString ifname;
    CString ofname;
    CString vcname;
    int vbitrate;
    double framerate;
    int reso_w;
    int reso_h;
    CString acname;
    int abitrate;
    int samplefmt;
    int channels;
    Taskstate taskstate;
    //是否包含视频流/音频流？
    int	video_stream;
    int	audio_stream;
} Task;

//--------------------





#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


