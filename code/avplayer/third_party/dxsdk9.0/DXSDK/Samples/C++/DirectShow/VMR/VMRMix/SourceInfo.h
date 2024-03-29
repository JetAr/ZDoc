﻿//------------------------------------------------------------------------------
// File: SourceInfo.h
//
// Desc: DirectShow sample code
//       Header file and class description for CMediaList,
//       play-list of media files
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#if !defined(AFX_SOURCEINFO_H__707CCC42_D3CC_40F1_B722_4E3ED3D7EAFF__INCLUDED_)
#define AFX_SOURCEINFO_H__707CCC42_D3CC_40F1_B722_4E3ED3D7EAFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>

//////////////////////////////////////////////////////////////////////////////////////////
// Name: SourceInfo
// Purpose: this class describes properties of media file to be played by VMR
//////////////////////////////////////////////////////////////////////////////////////////
typedef struct SourceInfo
{
    SourceInfo::SourceInfo() :
        m_fAlpha(0.5f),
        m_dwSeekingFlags(NULL),
        m_fRate(1.0f),
        m_llDuration(NULL)

    {
        m_rD.top = m_rD.left = 0.f;
        m_rD.right = m_rD.bottom = 1.f;
        m_szPath[0] = 0;  // Null-terminate

#ifndef UNICODE
        MultiByteToWideChar(CP_ACP, 0, (const TCHAR*)m_szPath, -1, m_wszPath, MAX_PATH);
#else
        USES_CONVERSION;
        _tcsncpy(m_wszPath, T2W(m_szPath), MAX_PATH-1);
        m_wszPath[MAX_PATH-1] = 0;
#endif
    };

    SourceInfo::~SourceInfo()
    {
    };

    bool CopyTo( SourceInfo* pSI)
    {
        if( !pSI )
        {
            return false;
        }

        _tcsncpy( pSI->m_szPath, m_szPath, MAX_PATH);

#ifndef UNICODE
        MultiByteToWideChar(CP_ACP, 0, (const TCHAR*)m_szPath, -1, pSI->m_wszPath, MAX_PATH);
#else
        USES_CONVERSION;
        _tcsncpy(pSI->m_wszPath, T2W(m_szPath), MAX_PATH-1);
        m_wszPath[MAX_PATH-1] = 0;
#endif


        pSI->m_dwSeekingFlags = m_dwSeekingFlags;
        pSI->m_fAlpha = m_fAlpha;
        pSI->m_fRate = m_fRate;
        pSI->m_llDuration = m_llDuration;
        pSI->m_rD = m_rD;
        return true;
    };

    bool            m_bInUse;
    TCHAR           m_szPath[MAX_PATH];
    WCHAR           m_wszPath[MAX_PATH];
    DWORD           m_dwSeekingFlags;
    double          m_fAlpha;
    double          m_fRate;
    LONGLONG        m_llDuration;
    NORMALIZEDRECT  m_rD;

} SourceInfo;


//////////////////////////////////////////////////////////////////////////////////////////
// Name: CMediaList
// Purpose: List of SourceInfo objects; a storage of available media files
//          to be played by VMR
//////////////////////////////////////////////////////////////////////////////////////////
class CMediaList
{
public:
    CMediaList();
    virtual ~CMediaList();

    int Size()
    {
        return m_N;
    };

    SourceInfo * GetItem( int n)
    {
        return (n<0 || n>m_N-1) ? NULL : m_ppSI[n];
    };

    LONGLONG GetAvgDuration()
    {
        return (m_avgDuration);
    };

    const WCHAR * GetFileNameW( int n)
    {
        return (n>=0 && n<m_N) ? m_ppSI[n]->m_wszPath : NULL;
    };

    bool Add( SourceInfo * pSI);
    bool Initialize(TCHAR *szFolder);
    bool Clone(int n, CMediaList *pML, int nStart=0 );

    void Clean();
    void Shuffle();
    void AdjustDuration();
    void SetInitialParameters();

private:
    int           m_N;
    SourceInfo ** m_ppSI;
    LONGLONG      m_avgDuration;
};


#endif // !defined(AFX_SOURCEINFO_H__707CCC42_D3CC_40F1_B722_4E3ED3D7EAFF__INCLUDED_)

