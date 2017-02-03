/*

                          Home Planet for Windows
                              	 Release 3

    Originally designed and implemented by John Walker in December 1987
    for the Sun workstation.
    
    Converted to Microsoft Windows in 1992: Home Planet release 1
    
    Release 2:	November 1994.

	Release 3:	November 1996--converted to 32 bit Windows.

*/

#include "sunclock.h"
#ifdef MaskFloatingErrors
#include <float.h>
#endif

char szString[128];   					// Variable to load resource strings
char szAppName[20];   					// Class name for the window
HINSTANCE hInst;           				// Class instance pointer
HWND hWndMain;        					// Main window pointer
HWND satTrackDlg = NULL;				// Satellite tracking control panel
HWND sunMoonPanel = NULL;				// Sun and Moon information panel
HWND satViewPanel = NULL;				// Satellite view panel
HWND planetPanel = NULL;				// Planet position panel
HWND skyViewPanel = NULL;				// Sky view panel
HWND objectCatPanel = NULL;				// Object catalogue panel
DLGPROC objectCatProc = NULL;			// Object catalogue dialogue procedure
HWND orreryPanel = NULL;				// Orrery panel
int waveAudioAvailable;					// Is wave audio output available ?
int cuckooEnable = FALSE;				// Enable the cuckoo clock ?
int holped = FALSE;						// Did we invoke help ?
HCURSOR mapCursor;						// Map window current cursor
HCURSOR xhairCursor = NULL;				// Site picking cursor
int sitePicking = FALSE;				// Site picking in progress

/* Forward procedures */

static int nCwRegisterClasses(void);

static int irate[] = {250, 500, 1000};	// Animation rates

#define Timer ((WORD) (animate ? irate[anirate] : (IsIconic(hWnd) ? (ddeActive ? 20000 : 32000) : 1000)))

/*  UPDATE_WINDOW  --  Update the contents of the window.  If
                       REPAINT is nonzero, the entire contents
                       of the window is updated.  Otherwise,
                       only changed material is displayed. */

static void update_window(HWND hWnd, HDC hDC, int repaint)
{
    if (repaint && !IsIconic(hWnd)) {
        SetWindowText(hWndMain, rstring(IDS_APPNAME));
    }
    ringgg(hWnd, hDC, repaint, FALSE);
}

/*

    Windows 3.0 Main Program Body

    The  following  routine  is  the  Windows  Main Program.  The Main
    Program is executed when a program is selected  from  the  Windows
    Control  Panel or File Manager.  The WinMain routine registers and
    creates the program's main window and initializes global  objects.
    The   WinMain  routine  also  includes  the  applications  message
    dispatch loop.  Every window message destined for the main  window
    or  any  subordinate windows is obtained, possibly translated, and
    dispatched  to  a  window  or  dialog  processing  function.   The
    dispatch  loop  is  exited  when  a  WM_QUIT  message is obtained.
    Before exiting the WinMain  routine  should  destroy  any  objects
    created and free memory and other resources.

*/

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
 /*
    HINSTANCE hInstance;     handle for this instance
    HINSTANCE hPrevInstance; handle for possible previous instances
    LPSTR  lpszCmdLine;     long pointer to exec command line
    int    nCmdShow;        Show code for main window display
 */

    MSG msg;       			// MSG structure to store your messages
    int nRc;       			// Return value from Register Classes
    HACCEL hAccel;			// Keyboard accelerator table

    strcpy(szAppName, "HPLANET");
    hInst = hInstance;
    if (!hPrevInstance) {
        /* Register window classes if first instance of application */
        if ((nRc = nCwRegisterClasses()) == -1) {
            /* Registering one of the windows failed */
            LoadString(hInst, IDS_ERR_REGISTER_CLASS, szString, sizeof(szString));
            MessageBox(NULL, szString, NULL, MB_ICONEXCLAMATION);
            return nRc;
        }

        /* We only want to enable the cuckoo clock if this is the first instance
           launched.  Otherwise multiple instances will try to sound off at the
           same time and confuse everybody. */

        cuckooEnable = TRUE;
    }

    tzset();                /* Define time zone from TZ variable */

#ifdef MaskFloatingErrors
#define ALL (EM_INVALID|EM_DENORMAL|EM_ZERODIVIDE|EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT)
    _control87(ALL, MCW_EM);
#endif

    /* Create application's Main window */

    hWndMain = CreateWindow(
        szAppName,              // Window class name
        rstring(IDS_APPNAME),   // Window's title
        WS_CAPTION      |       // Title and Min/Max
        WS_SYSMENU      |       // Add system menu box
        WS_MAXIMIZEBOX	|		// Add maximise box
        WS_MINIMIZEBOX  |       // Add minimise box
        WS_THICKFRAME   |       // Thick frame: allow resize
        WS_CLIPCHILDREN |       // don't draw in child windows areas
        WS_OVERLAPPED,          // this is a conventional overlapped window
        CW_USEDEFAULT, 0,       // Use default X, Y
        620 + (GetSystemMetrics(SM_CXFRAME) * 2), // Initial window width
        310 + (GetSystemMetrics(SM_CYFRAME) * 2) +
        	   GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU), // Initial height
        NULL,                   // Parent window's handle
        NULL,                   // Default to Class Menu
        hInst,                  // Instance of window
        NULL);                  // Create struct for WM_CREATE

    if (hWndMain == NULL) {
        LoadString(hInst, IDS_ERR_CREATE_WINDOW, szString, sizeof(szString));
        MessageBox(NULL, szString, NULL, MB_ICONEXCLAMATION);
        return IDS_ERR_CREATE_WINDOW;
    }

#ifdef ALWAYS_START_ICONIC
    nCmdShow = SW_SHOWMINNOACTIVE;      // Force iconic display
#endif
    ShowWindow(hWndMain, nCmdShow);     // Display main window

    hAccel = LoadAccelerators(hInst, szAppName);
    
    //	Add map window options to map image popup menu
    
    {	int i;
    	HMENU hm = GetMenu(hWndMain); 
        
        bitmapNum = mapbitsCount();
    	for (i = 0; i < bitmapNum; i++) {
    		mapbitsGetDesc(i, szString, sizeof szString);
    		if (strlen(szString) > 0) {
    			InsertMenu(hm, IDM_F_IMAGE_POP,
    				MF_BYCOMMAND | MF_STRING,
    				IDM_F_IMAGE + i, szString);  
    		}
    	}
    	RemoveMenu(hm, IDM_F_IMAGE_POP, MF_BYCOMMAND);
    }

    ddeInit(hInst);

    while (GetMessage(&msg, NULL, 0, 0)) {
    	if (!TranslateAccelerator(hWndMain, hAccel, &msg)) {
    		if ((satTrackDlg == NULL || !IsDialogMessage(satTrackDlg, &msg))) {
        		TranslateMessage(&msg);
        		DispatchMessage(&msg);
        	}
        }
    }

    /* Do clean up before exiting from the application */

    ddeTerm();

    return msg.wParam;
}

/*

    Main Window Procedure

    This  procedure  provides  service routines for the Windows events
    (messages) that Windows sends to the window, as well as  the  user
    initiated  events  (messages)  that  are  generated  when the user
    selects  the  action  bar  and  pulldown  menu  controls  or   the
    corresponding keyboard accelerators.

    The  SWITCH  statement shown below distributes the window messages
    to the respective message service routines, which are set apart by
    the  CASE  statements.   The  window  procedures  must  provide an
    appropriate service routine for its end user  initiated  messages,
    as  well  as the general Windows messages (ie.  WM_CLOSE message).
    If a message is sent to this  procedure  for  which  there  is  no
    programmed  CASE clause (i.e., no service routine), the message is
    defaulted to the DefWindowProc function, where it  is  handled  by
    Windows.

    For  the  end-user initiated messages, this procedure is concerned
    principally with the WM_COMMAND message.  The menu control ID  (or
    the   corresponding   accelerator  ID)  is  communicated  to  this
    procedure in the first message  parameter  (wParam).   The  window
    procedure  provides  a  major  CASE  statement  for the WM_COMMAND
    message and a subordinate SWITCH statement to provide CASE clauses
    for  the  message  service  routines  for the various menu item's,
    identified by their ID values.

    The message service routines for the individual menu items are the
    main  work  points in the program.  These service routines contain
    the units of work performed when the end user select  one  of  the
    menu  controls.   The  required  application  response  to  a menu
    control is programmed in its associated CASE clause.  The  service
    routines  may  contain subroutine calls to separately compiled and
    libraried routines, in-line calls to subroutines to be embodied in
    this source code module, or program statements entered directly in
    the CASE clauses.  Program control is switched to the  appropriate
    service  routine  when  Windows  recognizes the end user event and
    sends a WM_COMMAND message to the window procedure.   The  service
    routine  provides the appropriate application-specific response to
    the end user initiated event, then breaks to return control to the
    WinMain()  routine which continues to service the message queue of
    the window(s).

*/

WindowProc(WndProc)
{
    HDC hDC;                			// handle for the display device
    PAINTSTRUCT ps;         			// holds PAINT information
    int nRc = 0;            			// return code
    static short cxClient, cyClient;    // Window size
    static int wasIconic = -2;
    static DLGPROC satDlgProc = NULL;	// Satellite panel procedure instance
    static DLGPROC sunDlgProc = NULL;	// Sun/Moon panel procedure instance
    static DLGPROC satvDlgProc = NULL;	// View from satellite procedure instance
    static DLGPROC planetProc = NULL;	// Planet dialogue procedure
    static DLGPROC orreryProc = NULL;	// Orrery dialogue procedure
    static int menuactive = FALSE;		// Is menu displayed ?
    static BOOL chasetail = TRUE;		// Prevent repaint looping
    static UINT ourHelpMessage;			// Common dialogue box error message ID
	static char stdname[128], dstname[128];	// Standard and Daylight time zone names

    switch (Message) {
        case WM_COMMAND:
         /* The Windows messages for  action  bar  and  pulldown  menu
            items are processed here.

            The  WM_COMMAND  message  contains  the  message ID in its
            first parameter (wParam).  This routine is  programmed  to
            SWITCH  on  the #define values generated by CASE:W for the
            menu items in the application's header (*.H) file.  The ID
            values   have   the  format,  IDM_itemname.   The  service
            routines for  the  various  menu  items  follow  the  CASE
            statements up to the generated BREAK statements.  */

         switch (WM_COMMAND_ID(wParam)) {

            case IDM_F_EXIT:
                PostMessage(hWnd, WM_CLOSE, 0, 0L);
                break;

            case IDM_F_SAVESETTINGS:
            	{
            		int sdisp = displaymode;
            		
            		if (displaymode < 0) {
            			displaymode = 0;
            		}
            		saveSettings(szAppName);
            		displaymode = sdisp;
            	}
            	break;

            case IDM_F_DEFSETTINGS:
            	defSettings(szAppName);
	            if (displaymode == 0) {
	            	hDC = GetDC(hWnd);
	            	displaymode = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	            	displaymode = -((displaymode == 1) ? 1 : ((displaymode < 8) ? 16 : 256));
	            	ReleaseDC(hWnd, hDC);
	            }
            	if (strlen(satDBFile) == 0) {
					strcpy(satDBFile, rstring(IDS_SAT_DEFDB));
				}
            	setSatFromName();
            	if (satIcon != NULL) {
            		DestroyIcon(satIcon);
            		satIcon = NULL;
            	}
            	new_display_mode(hWnd, displaymode);
            	break;

            case IDM_F_COPY:
                {
                    HBITMAP hBitmap;
                    hDC = GetDC(hWnd);
                    hBitmap = CreateCompatibleBitmap(hDC, cxClient, cyClient);
                    if (hBitmap) {
                    	HBITMAP obmap;
		                HDC hdcMem;

	                	hdcMem = CreateCompatibleDC(hDC);
                        obmap = SelectObject(hdcMem, hBitmap);
                        ringgg(hWnd, hdcMem, TRUE, TRUE);
                        SelectObject(hdcMem, obmap);
                        DeleteDC(hdcMem);
                        OpenClipboard(hWnd);
                        EmptyClipboard();
                        SetClipboardData(CF_BITMAP, hBitmap);
                        CloseClipboard();
                    }
                    ReleaseDC(hWnd, hDC);
                }
                break;

             case IDM_F_TESTMODE:
                KillTimer(hWnd, 1);
                animate = !animate;
                SetTimer(hWnd, 1, Timer, NULL);
                break;

             case IDM_F_STOP:
                runmode = FALSE;
                ModifyMenu(GetMenu(hWnd), IDM_F_STOP, MF_BYCOMMAND,
                                  IDM_F_RUN, (LPSTR) "&Run");
                break;

             case IDM_F_RUN:
                runmode = TRUE;
                ModifyMenu(GetMenu(hWnd), IDM_F_RUN, MF_BYCOMMAND,
                                  IDM_F_STOP, (LPSTR) "&Stop");
                break;

             case IDM_F_SHOWMOON:
             	hDC = GetDC(hWnd);
             	undrawmoon(hDC);
             	showmoon = !showmoon;
             	drawmoon(hDC);
             	ReleaseDC(hWnd, hDC);
             	break;

        	 case IDM_F_DISPMONO:
        	 	new_display_mode(hWnd, 1);
        	 	break;

        	 case IDM_F_DISPC16:
        	 	new_display_mode(hWnd, 16);
        	 	break;

        	 case IDM_F_DISPGREY:
        	 	new_display_mode(hWnd, 255);
        	 	break;

        	 case IDM_F_DISPC256:
        	 	new_display_mode(hWnd, 256);
        	 	break;

        	 case IDM_F_ANIMIN:
        	 case IDM_F_ANIHOUR:
        	 case IDM_F_ANIDAY:
        	 case IDM_F_ANIWEEK:
        	 case IDM_F_ANIMONTH:
        	 case IDM_F_ANIYEAR:
        	 case IDM_F_ANISDAY:
        	 case IDM_F_ANISYEAR:
        	 case IDM_F_ANIDECADE:
        	 case IDM_F_ANICENTURY:
        	 case IDM_F_ANIMILL:
        	 	lincr = wParam - IDM_F_ANIMIN;
        	 	break;

        	 case IDM_F_ANIFWD:
        	 	idir = 1;
        	 	break;

        	 case IDM_F_ANIBACK:
        	 	idir = -1;
        	 	break;

        	 case IDM_F_ANISFAST:
        	 	anirate = 0;
                KillTimer(hWnd, 1);
                SetTimer(hWnd, 1, Timer, NULL);
        	 	break;

        	 case IDM_F_ANISMED:
        	 	anirate = 1;
                KillTimer(hWnd, 1);
                SetTimer(hWnd, 1, Timer, NULL);
        	 	break;

        	 case IDM_F_ANISSLOW:
        	 	anirate = 2;
                KillTimer(hWnd, 1);
                SetTimer(hWnd, 1, Timer, NULL);
        	 	break;

             case IDM_F_ABOUT:
                {
                      DLGPROC lpfnMsgProc;

                      lpfnMsgProc = (DLGPROC) MakeProcInstance((FARPROC) About, hInst);
                      nRc = DialogBox(hInst, MAKEINTRESOURCE(IDC_ABOUT), hWnd, lpfnMsgProc);
                      FreeProcInstance((FARPROC) lpfnMsgProc);
                }

                break;

             case IDM_F_HELPCONT:
             	WinHelp(hWnd, rstring(IDS_HELPFILE), HELP_CONTENTS, 0L);
             	holped = TRUE;
             	break;

             case IDM_F_HELPSEARCH:
             	WinHelp(hWnd, rstring(IDS_HELPFILE), HELP_PARTIALKEY, ((DWORD) ((LPSTR) "")));
             	holped = TRUE;
             	break;

             case IDM_F_HELPHOW:
             	WinHelp(hWnd, rstring(IDS_HELPHELP), HELP_HELPONHELP, 0L);
             	holped = TRUE;
             	break;

             case IDM_F_SETJDATE:
                {
                      DLGPROC lpfnMsgProc;

                      lpfnMsgProc = (DLGPROC) MakeProcInstance((FARPROC) SetJdate, hInst);
                      nRc = DialogBox(hInst, MAKEINTRESOURCE(IDC_JDATE), hWnd, lpfnMsgProc);
                      FreeProcInstance((FARPROC) lpfnMsgProc);
                      if (nRc) {
                        runmode = FALSE;
                        ModifyMenu(GetMenu(hWnd), IDM_F_STOP, MF_BYCOMMAND,
                                  IDM_F_RUN, (LPSTR) "&Run");
                        hDC = GetDC(hWnd);
                        update_window(hWnd, hDC, TRUE);
                        ReleaseDC(hWnd, hDC);
                      }
                }
                break;

             case IDM_F_SETUTIME:
                {
                      DLGPROC lpfnMsgProc;

                      lpfnMsgProc = (DLGPROC) MakeProcInstance((FARPROC) SetUtime, hInst);
                      nRc = DialogBox(hInst, MAKEINTRESOURCE(IDC_SETUTIME), hWnd, lpfnMsgProc);
                      FreeProcInstance((FARPROC) lpfnMsgProc);
                      if (nRc) {
                        runmode = FALSE;
                        ModifyMenu(GetMenu(hWnd), IDM_F_STOP, MF_BYCOMMAND,
                                  IDM_F_RUN, (LPSTR) "&Run");
                        hDC = GetDC(hWnd);
                        update_window(hWnd, hDC, TRUE);
                        ReleaseDC(hWnd, hDC);
                      }
                }
                break;

             case IDM_F_SETOBSLOC:
                {
                      DLGPROC lpfnMsgProc;

                      lpfnMsgProc = (DLGPROC) MakeProcInstance((FARPROC) SetObserverLocation, hInst);
                      nRc = DialogBox(hInst, MAKEINTRESOURCE(IDM_OBSLOC), hWnd, lpfnMsgProc);
                      FreeProcInstance((FARPROC) lpfnMsgProc);
                      if (nRc) {
                      	planetSiteChanged();
						if (tracking) {
							// Must update satellite position as seen from new site
							updsat(faketime, FALSE, FALSE);
						}
				        updateSky(faketime, TRUE);
                      }
                }
                break;

             case IDM_F_CUCKOO:
             	cuckooClock = !cuckooClock;
             	break;

             case IDM_F_SATTRACK:
             	if (satTrackDlg == NULL) {
             		if (satDlgProc == NULL) {
             			satDlgProc = (DLGPROC) MakeProcInstance((FARPROC) SattrackDlg, hInst);
             		}
                	satTrackDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDC_SATTRACK), hWnd,
                		satDlgProc);
             	}
             	break;

             case IDM_F_SUNMOON:
             	if (sunMoonPanel == NULL) {
             		if (sunDlgProc == NULL) {
             			sunDlgProc = (DLGPROC) MakeProcInstance((FARPROC) SunMoonDlg, hInst);
             		}
                	sunMoonPanel = CreateDialog(hInst, MAKEINTRESOURCE(IDC_SUNMOON), hWnd,
                		sunDlgProc);
             	}
             	break;

             case IDM_F_PLANPAN:
             	if (planetPanel == NULL) {
             		if (planetProc == NULL) {
             			planetProc = (DLGPROC) MakeProcInstance((FARPROC) planetDlg, hInst);
             		}
                	planetPanel = CreateDialog(hInst, MAKEINTRESOURCE(IDM_PLANPOS), hWnd,
                		planetProc);
             	}
             	break;

             case IDM_F_ORRERY:
             	if (orreryPanel == NULL) {
             		if (orreryProc == NULL) {
             			orreryProc = (DLGPROC) MakeProcInstance((FARPROC) OrreryDlg, hInst);
             		}
                	orreryPanel = CreateDialog(hInst, MAKEINTRESOURCE(IDC_ORRERY), hWnd,
                		orreryProc);
             	}
             	break;

             case IDM_F_OBJCAT:
             	launchObjectCat();
             	break;

             case IDM_F_SKY:
             	launchSkyWindow();
             	break;

             case IDM_F_DAYVIEW:
             case IDM_F_MOONVIEW:
             case IDM_F_NIGHTVIEW:
             case IDM_F_SATVIEW1:
             case IDM_F_VFOBSLOC:
             	viewfrom = (int) wParam;
             	if (satViewPanel == NULL) {
             		if (satvDlgProc == NULL) {
             			satvDlgProc = (DLGPROC) MakeProcInstance((FARPROC) SatViewDlg, hInst);
             		}
                	satViewPanel = CreateDialog(hInst, MAKEINTRESOURCE(IDC_SATVIEW), hWnd,
                		satvDlgProc);
             	} else {
             		satviewchanged();
             		InvalidateRgn(satViewPanel, NULL, TRUE);
             	}
             	break;

             case IDM_F_TEXTUREVIEW:
             	texturing = TRUE;
             	if (satViewPanel != NULL) {
             		satviewchanged();
             		InvalidateRgn(satViewPanel, NULL, TRUE);
             	}
             	break;

             case IDM_F_VECTORVIEW:
             	texturing = FALSE;
             	if (satViewPanel != NULL) {
             		satviewchanged();
             		InvalidateRgn(satViewPanel, NULL, TRUE);
             	}
             	break;

             case IDM_F_SATDBASE:
             	{
             		OPENFILENAME ofn;
             		char *cp;

                    memset(&ofn, 0, sizeof(ofn));
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = hWnd;
					ofn.lpstrFilter = cp = rstring(IDS_SAT_FILTER);
					while (*cp != 0) {
						if (*cp == '|') {
							*cp = 0;
						}
						cp++;
					}
					ofn.lpstrCustomFilter = NULL;
					strcpy(szString, satDBFile);
					ofn.lpstrFile = (LPSTR) szString;
					ofn.nMaxFile = sizeof(szString);
					ofn.lpstrInitialDir = NULL;
					ofn.lpstrTitle = rstring(IDS_SAT_TITLE);
					ofn.Flags = OFN_FILEMUSTEXIST |
								OFN_HIDEREADONLY | OFN_SHOWHELP;
					if (GetOpenFileName((LPOPENFILENAME) &ofn)) {
						strcpy(satDBFile, szString);
						if (satTrackDlg != NULL) {
							SendMessage(satTrackDlg, WM_ST_NEWDBFILE, 0, 0L);
					    }
					}
             	}
             	break;

             case IDM_F_SATICON:
               {
                      DLGPROC lpfnMsgProc;

                      lpfnMsgProc = (DLGPROC) MakeProcInstance((FARPROC) SatIconDlg, hInst);
                      nRc = DialogBox(hInst, MAKEINTRESOURCE(IDC_SATICON), hWnd, lpfnMsgProc);
                      FreeProcInstance((FARPROC) lpfnMsgProc);
                }
             	break;

            default:
            
            	//	Selection of a new image from dynamically created image menu
            
        	 	if (wParam >= IDM_F_IMAGE && wParam <= IDM_F_IMAGE_POP) {
	        	 	imageBitmap = wParam - IDM_F_IMAGE;
	        	 	release_bitmaps();
	            	InvalidateRect(hWnd, NULL, TRUE);
	            	UpdateWindow(hWnd);
	             	if (satViewPanel != NULL) {
	             		satviewchanged();
	             		InvalidateRgn(satViewPanel, NULL, TRUE);
	             	}
        	 		break;
        	 	}
                return DefWindowProc(hWnd, Message, wParam, lParam);
            }
            break;

        case WM_CREATE:
         /* The  WM_CREATE  message is sent  once to a window when the
            window is created.   The  window  procedure  for  the  new
            window  receives this message after the window is created,
            but before the window becomes visible.

            Parameters:

               lParam  -  Points to a CREATESTRUCT structure with
                  the following form:

               typedef struct {
                  LPSTR     lpCreateParams;
                  HANDLE    hInst;
                  HANDLE    hMenu;
                  HWND      hwndParent;
                  int       cy;
                  int       cx;
                  int       y;
                  int       x;
                  LONG      style;
                  LPSTR     lpszName;
                  LPSTR     lpszClass;
                  DWORD     dwExStyle;
              }  CREATESTRUCT;  */

            mapCursor = LoadCursor(NULL, IDC_ARROW);
            ourHelpMessage = RegisterWindowMessage(HELPMSGSTRING);
			waveAudioAvailable = waveOutGetNumDevs() > 0;
            loadSettings(szAppName);
            if (strlen(satDBFile) == 0) {
				strcpy(satDBFile, rstring(IDS_SAT_DEFDB));
			}
            hDC = GetDC(hWnd);
            if (displaymode == 0) {
            	displaymode = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
            	displaymode = -((displaymode == 1) ? 1 : ((displaymode < 8) ? 16 : 256));
            }
			satsbitmap = CreateCompatibleBitmap(hDC, 32, 32);
			moonsbitmap = CreateCompatibleBitmap(hDC, 32, 32);
            ReleaseDC(hWnd, hDC);
			GetTimeZoneInformation(&tzInfo);
			wcstombs(stdname, tzInfo.StandardName, sizeof stdname);
			tzName[FALSE] = stdname;
			wcstombs(dstname, tzInfo.DaylightName, sizeof dstname);
			tzName[TRUE] = dstname;
			localtimeformat();
			setSatFromName();
            break;
            
        case WM_CLOSE:
            if (skyViewPanel != NULL) {
            	SendMessage(skyViewPanel, WM_CLOSE, 0, 0L);
            }
            DestroyWindow(hWnd);
			break;        

        case WM_DESTROY:
            KillTimer(hWnd, 1);
            release_bitmaps();
            release_satview_bitmaps();
            if (satDlgProc != NULL) {
            	FreeProcInstance((FARPROC) satDlgProc);
            }
            if (sunDlgProc != NULL) {
            	FreeProcInstance((FARPROC) sunDlgProc);
            }
            if (satvDlgProc != NULL) {
            	FreeProcInstance((FARPROC) satvDlgProc);
            }
            if (planetProc != NULL) {
            	FreeProcInstance((FARPROC) planetProc);
            }
            if (orreryProc != NULL) {
            	FreeProcInstance((FARPROC) orreryProc);
            }
            if (objectCatProc != NULL) {
            	FreeProcInstance((FARPROC) objectCatProc);
            }
            DeleteObject(moonsbitmap);
            DeleteObject(satsbitmap);
            if (satIcon != NULL) {
            	DestroyIcon(satIcon);
            }
            if (xhairCursor != NULL) {
            	DestroyCursor(xhairCursor);
            }
            if (holped) {
            	WinHelp(hWnd, rstring(IDS_HELPFILE), HELP_QUIT, 0L);
            }
            PostQuitMessage(0);
            break;

        case WM_MOUSEMOVE:
        	SetCursor(mapCursor);		// Update cursor position
        	break;

        case WM_LBUTTONDOWN:			// Left mouse button down
        	if (sitePicking) {
        		sitePicked(LOWORD(lParam), HIWORD(lParam), cxClient, cyClient);
        	}
        	break;

        case WM_LBUTTONDBLCLK:
        	{	int xpos, ypos;

        		/* Double click accelerator gimmicks:

        				Double clicking on the Moon displays the Sun/Moon information
        				panel.

        				Double clicking on the satellite displays the satellite tracking
        				panel.  If it's already displayed, the view from satellite panel
        				appears.  */

        		xpos = LOWORD(lParam);
        		ypos = HIWORD(lParam);
        		if (hitmoon(xpos, ypos) && (sunMoonPanel == NULL)) {
        			SendMessage(hWnd, WM_COMMAND, IDM_F_SUNMOON, 0L);
        		} else if (hitsat(xpos, ypos)) {
        			if (satTrackDlg == NULL) {
        				SendMessage(hWnd, WM_COMMAND, IDM_F_SATTRACK, 0L);
        			} else if (satViewPanel == NULL) {
        				SendMessage(hWnd, WM_COMMAND, IDM_F_SATVIEW1, 0L);
        			}
        		}
        	}
        	break;

        case WM_COMPACTING:
        	/* Good citizenship 101: when Windows is low on memory,
        	   free our screen refresh bitmap and opt instead to
        	   repaint the window from the vector list if the window
        	   needs repair. */
        	invalidate_refresh();
        	release_satview_bitmaps();
        	break;

        case WM_WININICHANGE:
        	/* Reset local time format in case that was the change just
        	   made to WIN.INI. */
        	localtimeformat();
        	break;
        	
        case WM_PALETTECHANGED:
        	if (chasetail) {
        		chasetail = FALSE;
	        	// Force repaint if somebody changes the system palette
	        	if (hWnd != ((HWND) wParam)) {
	        		InvalidateRgn(hWnd, NULL, TRUE);	        		
	        	}
	        	chasetail = TRUE;
	        }
			break;

        case WM_SIZE:
        	release_bitmaps();
        	if (wParam == SIZE_MINIMIZED) {
        		release_satview_bitmaps();
        	}
            /* wParam contains a code indicating the requested sizing */
            cxClient = LOWORD(lParam);
            cyClient = HIWORD(lParam);
            /* lParam contains the new height and width of the client area */
            break;

        case WM_MENUSELECT:
        	/* To avoid embarrassing screen artifacts, we must suppress timer-initiated
        	   screen updates while a menu (which has saved the underlying bitmap) is
        	   active on the screen. */
        	if (HIWORD(wParam) == 0xFFFF && lParam == 0) {
            	menuactive = FALSE;
            }
        	break;

        case WM_PAINT:
            memset(&ps, 0, sizeof(PAINTSTRUCT));

            /* Calling InvalidateRgn() at the start of the WM_PAINT message
               processing looks more than passing strange, but there's an
               excellent reason to do so.  In response to timer events,
               Home Planet updates its window at times when portions of it might
               be partially obscured by other windows.  When the obscuration
               is removed and a WM_PAINT is sent to repair the damage, it
               is essential that the drawing operations we do to reconstruct
               the window be performed everywhere, since the interim updates
               may have XORed regions of the screen which must now be
               rewritten from scratch. */

            InvalidateRgn(hWnd, NULL, TRUE);

            hDC = BeginPaint(hWnd, &ps);

            /* If iconic state of the window has  changed,  reset  the
               timer  to the update rate appropriate to the new window
               state.  */

            if (IsIconic(hWnd) != wasIconic) {
                if (wasIconic != -2) {
                    KillTimer(hWnd, 1);
                }
                wasIconic = IsIconic(hWnd);
                SetTimer(hWnd, 1, Timer, NULL);
                if (wasIconic) {
                    go_iconic(hWnd);
                }
            }

            /* Included in case the background is not a pure color  */
            SetBkMode(hDC, TRANSPARENT);

            /* Application should draw on the client window using  the
               GDI  graphics and text functions.  'ps' the PAINTSTRUCT
               returned by BeginPaint contains a rectangle to the area
               that must be repainted.  */

            ringgg(hWnd, hDC, TRUE, FALSE);
            /* Inform Windows painting is complete */
            EndPaint(hWnd, &ps);
            break;

        case WM_SYSCOLORCHANGE:
            /* Since the system window background and text colour settings
               are used for the open window display, force the window to
               be repainted when the system colours change. */
            InvalidateRect(hWnd, NULL, TRUE);
            new_display_mode(hWnd, displaymode);
            break;

        case WM_TIMER:
        	if (runmode && !menuactive) {
            	hDC = GetDC(hWnd);
            	update_window(hWnd, hDC, FALSE);
            	ReleaseDC(hWnd, hDC);
            }
            if (ddeActive) {
            	updateDDEInfo();
            }
            updateCuckoo();
            break;

        case WM_INITMENU:
        	menuactive = TRUE;
#define Chekov(menuitem, condition) \
			CheckMenuItem((HMENU) wParam, (menuitem), \
				(condition) ? MF_CHECKED : MF_UNCHECKED)
            if (GetMenu(hWnd) == ((HMENU) wParam)) {
            	Chekov(IDM_F_TESTMODE, animate);
            	Chekov(IDM_F_SHOWMOON, showmoon);
            	Chekov(IDM_F_CUCKOO, cuckooClock && waveAudioAvailable);
            	Chekov(IDM_F_DISPMONO, abs(displaymode) == 1);
            	Chekov(IDM_F_DISPC16, abs(displaymode) == 16);
            	Chekov(IDM_F_DISPGREY, abs(displaymode) == 255);
            	Chekov(IDM_F_DISPC256, abs(displaymode) == 256);
#define Cincr(x) Chekov((x), lincr == (x - IDM_F_ANIMIN))
        	 	Cincr(IDM_F_ANIMIN);
        	 	Cincr(IDM_F_ANIHOUR);
        	 	Cincr(IDM_F_ANIDAY);
        	 	Cincr(IDM_F_ANIWEEK);
        		Cincr(IDM_F_ANIMONTH);
        	 	Cincr(IDM_F_ANIYEAR);
        	 	Cincr(IDM_F_ANISDAY);
        	 	Cincr(IDM_F_ANISYEAR);
        	 	Cincr(IDM_F_ANIDECADE);
        	 	Cincr(IDM_F_ANICENTURY);
        	 	Cincr(IDM_F_ANISYEAR);
        	 	Cincr(IDM_F_ANIMILL);
        	 	Chekov(IDM_F_ANIFWD, idir > 0);
        	 	Chekov(IDM_F_ANIBACK, idir < 0);
            	Chekov(IDM_F_ANISFAST, anirate == 0);
            	Chekov(IDM_F_ANISMED, anirate == 1);
            	Chekov(IDM_F_ANISSLOW, anirate == 2);
#define Cvfrom(x) Chekov((x), (satViewPanel != NULL) && viewfrom == x)
				Chekov(IDM_F_SATVIEW1, (satViewPanel != NULL) && (viewfrom == IDM_F_SATVIEW1) && tracking);
				Cvfrom(IDM_F_DAYVIEW);
				Cvfrom(IDM_F_NIGHTVIEW);
				Cvfrom(IDM_F_MOONVIEW); 
				Cvfrom(IDM_F_VFOBSLOC);
				Chekov(IDM_F_TEXTUREVIEW, texturing && abs(displaymode) >= 256);
				Chekov(IDM_F_VECTORVIEW, !texturing || abs(displaymode) <= 255);
				EnableMenuItem((HMENU) wParam, IDM_F_SATVIEW1,
					tracking ? MF_ENABLED : (MF_DISABLED | MF_GRAYED));
				EnableMenuItem((HMENU) wParam, IDM_F_TEXTUREVIEW,
					abs(displaymode) >= 255 ? MF_ENABLED :
                						 (MF_DISABLED | MF_GRAYED));
                {	int i;
                
                	for (i = IDM_F_IMAGE; i < IDM_F_IMAGE + bitmapNum; i++) {
						EnableMenuItem((HMENU) wParam, i,
							abs(displaymode) >= 255 ? MF_ENABLED :
							(MF_DISABLED | MF_GRAYED));
						Chekov(i, (i - IDM_F_IMAGE) == imageBitmap);
					}
				}
                EnableMenuItem((HMENU) wParam, IDM_F_SATTRACK, satTrackDlg == NULL ?
                	MF_ENABLED : (MF_DISABLED | MF_GRAYED));
                EnableMenuItem((HMENU) wParam, IDM_F_SUNMOON, sunMoonPanel == NULL ?
                	MF_ENABLED : (MF_DISABLED | MF_GRAYED));
                EnableMenuItem((HMENU) wParam, IDM_F_PLANPAN, planetPanel == NULL ?
                	MF_ENABLED : (MF_DISABLED | MF_GRAYED));
                EnableMenuItem((HMENU) wParam, IDM_F_SKY, skyViewPanel == NULL ?
                	MF_ENABLED : (MF_DISABLED | MF_GRAYED));
                EnableMenuItem((HMENU) wParam, IDM_F_ORRERY, orreryPanel == NULL ?
                	MF_ENABLED : (MF_DISABLED | MF_GRAYED));
                EnableMenuItem((HMENU) wParam, IDM_F_OBJCAT, objectCatPanel == NULL ?
                	MF_ENABLED : (MF_DISABLED | MF_GRAYED));
                EnableMenuItem((HMENU) wParam, IDM_F_CUCKOO, waveAudioAvailable ?
                	MF_ENABLED : (MF_DISABLED | MF_GRAYED));
            }
            break;

        default:
        	if (Message == ourHelpMessage) {
                WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                			((DWORD) ((LPSTR) rstring(IDS_HELP_ON_SATDBASE))));
                holped = TRUE;
        		break;
        	}
            /* For  any  message  for  which  you  don't  specifically
               provide  a  service  routine,  you  should  return  the
               message  to Windows for default message processing.  */
            return DefWindowProc(hWnd, Message, wParam, lParam);
    }
    return 0L;
}

/*
    nCwRegisterClasses Function

    The following function  registers  all  the  classes  of  all  the
    windows associated with this application.  The function returns an
    error code if unsuccessful, otherwise it returns 0.
*/

static int nCwRegisterClasses(void)
{
    WNDCLASS wndclass;          /* struct to define a window class */
    memset(&wndclass, 0, sizeof(WNDCLASS));

    /* load WNDCLASS with window's characteristics */
    wndclass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW | CS_DBLCLKS;
    wndclass.lpfnWndProc = WndProc;
    /* Extra storage for Class and Window objects */
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInst;
    wndclass.hIcon = LoadIcon(hInst, "_SUNCLOCK");
    wndclass.hCursor = NULL;
    /* Create brush for erasing background */
    wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
    wndclass.lpszMenuName = szAppName;      // Menu Name is App Name
    wndclass.lpszClassName = szAppName;     // Class Name is App Name
    if (!RegisterClass(&wndclass))
        return -1;

    // Register sky view window class (even though we don't intially instantiate it)

    wndclass.hCursor = LoadCursor(hInst, "sky");
    wndclass.hIcon = LoadIcon(hInst, "sky");
    wndclass.hbrBackground = GetStockObject(GRAY_BRUSH);
    wndclass.lpfnWndProc = SkyWndProc;
    wndclass.lpszMenuName = "sky";			// Menu name
    wndclass.lpszClassName = "SkyWindow";	// Class name
    RegisterClass(&wndclass);

    return 0;
}
