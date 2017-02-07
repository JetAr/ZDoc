/*
Module : Panel.cpp
Purpose: Defines the code implementation for the CDisplay class
Created: CJP / 20-09-2001
History: CJP / 21-10-2001
Copyright (c) 2001 by C. Jiménez de Parga
All rights reserved.
*/

#include "stdafx.h"
#include "panel.h"

// The constructor of the class
CDisplay::CDisplay()
{
}

// Initialize the display from a rectangle coordinates
// Asociate to it a memory bitmap
// Show the display in black background
void CDisplay::InitDisplay(CWnd *Wnd,int xp,int yp,int wp,int hp)
{
    m_x=xp;
    m_y=yp;
    m_h=hp;
    m_w=wp;
    CClientDC dc(Wnd);
    m_memDC.CreateCompatibleDC(&dc);
    m_bmp.CreateCompatibleBitmap(&dc,m_w,m_h);
    m_memDC.SelectObject(&m_bmp);
    m_bkbrush.CreateStockObject(BLACK_BRUSH);
    m_memDC.SelectObject(&m_bkbrush);
    m_memDC.PatBlt(0,0,m_w,m_h,PATCOPY);
    m_memDC.SetBkColor(RGB(0,0,0));
    m_memDC.SelectStockObject(ANSI_VAR_FONT);
}

// Member function to put text on the display
void CDisplay::SetText(CWnd *Wnd,CString strText,int x,int y,COLORREF crColor)
{
    CRect rect;
    rect.left=x;
    rect.top=y;
    m_memDC.SetTextColor(crColor);
    m_memDC.DrawText(strText,&rect,DT_CALCRECT|DT_SINGLELINE);
    m_memDC.DrawText(strText,&rect,DT_NOCLIP|DT_SINGLELINE);
    rect.left+=m_x;
    rect.top+=m_y;
    rect.bottom+=m_y;
    rect.right+=m_x;
    // invalidate only the affected area
    Wnd->InvalidateRect(&rect,FALSE);
}


// Member function to erase text on the display
void CDisplay::EraseText(CWnd *Wnd,CString strText,int x,int y)
{
    CRect rect;
    rect.left=x;
    rect.top=y;
    m_memDC.DrawText(strText,&rect,DT_CALCRECT|DT_SINGLELINE);
    m_memDC.FillSolidRect(&rect,RGB(0,0,0));
    rect.left+=m_x;
    rect.top+=m_y;
    rect.bottom+=m_y;
    rect.right+=m_x;
    // invalidate only the affected area
    Wnd->InvalidateRect(&rect);
}

// Destroy the GDI objects
CDisplay::~CDisplay()
{
    m_bmp.DeleteObject();
    m_bkbrush.DeleteObject();
}