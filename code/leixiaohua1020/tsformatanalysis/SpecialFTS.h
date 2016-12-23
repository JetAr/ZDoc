/*
 * MPEG2-TS格式分析器
 * MPEG2-TS Format Analysis
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 */
#pragma once

#ifndef __AFXWIN_H__
#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CSpecialFTSApp:
// 有关此类的实现，请参阅 SpecialFTS.cpp
//

class CSpecialFTSApp : public CWinApp
{
public:
    CSpecialFTSApp();

// 重写
public:
    virtual BOOL InitInstance();
    //加载语言配置
    void LoadLaguage();
// 实现

    DECLARE_MESSAGE_MAP()
};

extern CSpecialFTSApp theApp;
