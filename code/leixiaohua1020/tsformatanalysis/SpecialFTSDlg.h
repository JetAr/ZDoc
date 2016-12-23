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

#include "resource.h"
#include "stdafx.h"
#include "tsdemux.h"
#include "afxwin.h"
// CSpecialFTSDlg 对话框
class CSpecialFTSDlg : public CDialogEx
{
// 构造
public:
    CSpecialFTSDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
    enum { IDD = IDD_SPECIAL_F_TS };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedFTsInputurlOpen();
    void SystemClear();
    char fileurl[MAX_URL_LENGTH];
    CMFCEditBrowseCtrl m_ftsinputurl;
    CListCtrl m_ftspacketlist;
    //添加一条TS包记录(PID,类型（音频，视频 or PSI），PSI类型)
    //ListCtrl索引
    int tsl_index;
    int AppendTSLInfo(int pid,int type,int psi_type);
    CButton m_ftspacketlistmaxnum;
    void OnCustomdrawMyList ( NMHDR* pNMHDR, LRESULT* pResult );
    //PropertyList------------------
    CMFCPropertyGridProperty *PATgroup;
    CMFCPropertyGridProperty *PMTgroup;
    CMFCPropertyGridCtrl m_ftsbasicinfo;
    int AppendBInfo(char *dst_group,char *property_name,char *value,char *remark);
    int ShowBInfo();


    afx_msg void OnBnClickedFTsAbout();


    afx_msg void OnDropFiles(HDROP hDropInfo);

    //专门用于加载字符串
    CString text;
    CComboBox m_ftsdumpmode;

    afx_msg void OnBnClickedFTsOutput();
//	int m_ftsoutput;
    int m_ftsoutput;
    afx_msg void OnBnClickedFTsOutput2();
    CComboBox m_ftslang;
    afx_msg void OnSelchangeFTsLang();
};
