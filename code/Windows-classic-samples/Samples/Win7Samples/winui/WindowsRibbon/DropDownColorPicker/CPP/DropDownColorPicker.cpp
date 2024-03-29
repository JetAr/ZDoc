﻿// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

// DropDownColorPicker.cpp : Defines the entry point for the application.
//

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files.
#include <windows.h>
#include <wchar.h>

#include "DropDownColorPicker.h"
#include "RibbonFramework.h"

#define MAX_LOADSTRING 100

// Global Variables.
WCHAR wszTitle[MAX_LOADSTRING];                  // The title bar text.
WCHAR wszWindowClass[MAX_LOADSTRING];            // the main window class name.
CRenderer g_renderer;                            // client renderer.

// Forward declarations of functions included in this code module.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        return FALSE;
    }

    // Initialize global strings.
    LoadString(hInstance, IDS_APP_TITLE, wszTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDS_APP_CLASS, wszWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    // Main message loop.
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();
    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = 0;    // Don't use CS_HREDRAW or CS_VREDRAW with a Ribbon
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RIBBONAPP));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = wszWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hWnd = CreateWindow(wszWindowClass, wszTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                        CW_USEDEFAULT, 0, 500, 650, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }
    // Initialize renderer.
    g_renderer.Initialize(hWnd);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND    - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY    - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    UINT ribbonHeight;

    switch (message)
    {
    case WM_SIZE:
        // Invalidate the client area below the ribbon
        if (SUCCEEDED(GetRibbonHeight(&ribbonHeight)))
        {
            RECT rect;
            GetClientRect(hWnd, &rect);
            rect.top = ribbonHeight;
            InvalidateRect(hWnd, &rect, TRUE);
        }
        break;
    case WM_CREATE:
        // Initialize the ribbon framework.
        if (!InitializeFramework(hWnd))
        {
            return -1;
        }
        break;
    case WM_PAINT:
        // Handles client area drawing.
        hdc = BeginPaint(hWnd, &ps);
        if (hdc && SUCCEEDED(GetRibbonHeight(&ribbonHeight)))
        {
            g_renderer.Draw(hdc, ribbonHeight);
        }
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        DestroyFramework();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
