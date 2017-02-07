/*
Module : Panel.h
Purpose: Defines the header implementation for the CDisplay class
Created: CJP / 20-09-2001
History: CJP / 21-10-2001
Copyright (c) 2001 by C. Jiménez de Parga
All rights reserved.
*/

#ifndef PANEL_H
#define PANEL_H

class CDisplay
{
public:
    int m_x,m_y,m_h,m_w;
    CDC m_memDC;
protected:
    CBitmap m_bmp;
    CBrush  m_bkbrush;
public:
    CDisplay();
    ~CDisplay();
    void InitDisplay(CWnd *Wnd,int xp,int yp,int wp,int hp);
    void SetText(CWnd *Wnd,CString strText,int x,int y,COLORREF crColor);
    void EraseText(CWnd *Wnd,CString strText,int x,int y);
};

#endif