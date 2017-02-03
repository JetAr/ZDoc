/*

			Home Planet  --  The Screen Saver

*/

#include "sunclock.h"

#ifdef ScreenSaver

#define ID_TIMER 200

/* Function prototypes */

static void GetIniEntries(void);
static void GetIniSettings(void);

/* Global used by SCRNSAVE.LIB. Required for all screen savers. */

char szAppName[40];

/* Globals exported. */

char szName[]="Home Planet Screen Saver";
int satDisplayed = FALSE;
POINT pLine[POLLY];
int pLinel = 0;
int textHeight;							// System font total height

/* Externals defined in SCRNSAVE.LIB. Required for all screen savers. */

HINSTANCE hMainInstance;
HWND hMainWindow;
char szName[TITLEBARNAMELEN];
char szScreenSaver[22];
char szHelpFile[MAXFILELEN];
char szNoHelpMemory[BUFFLEN];
UINT MyHelpMessage;
HOOKPROC fpMessageFilter;

static WORD wElapse = 1000;					// speed parameter
static WORD wTimer;							// timer id
static char szShowMoon[] = "ShowMoon";		// Show Moon profile string key
static char stdname[128], dstname[128];		// Standard and Daylight time zone names

/* ScreenSaverProc - Main entry point for screen saver messages.
					 This function is required for all screen savers.

	Params:  Standard window message handler parameters.

	Return:  The return value depends on the message.

	Note that all messages go to the DefScreenSaverProc(), except
	for ones we process.
*/

WindowProc(ScreenSaverProc)
{
    RECT rc;
    HDC hDC;
    static int iticks;

    switch (Message) {
        case WM_CREATE:
        {
            TEXTMETRIC tm;

            /* Load the strings from the STRINGTABLE */

			hDC = GetDC(hWnd);
			GetTextMetrics(hDC, &tm);
			textHeight = tm.tmHeight + tm.tmExternalLeading;
            GetIniEntries();
            if (displaymode == 0) {
            	displaymode = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
            	displaymode = (displaymode == 1) ? 1 : ((displaymode < 8) ? 16 : 256);
            }

            GetIniSettings();
			GetTimeZoneInformation(&tzInfo);
			wcstombs(stdname, tzInfo.StandardName, sizeof stdname);
			tzName[FALSE] = stdname;
			wcstombs(dstname, tzInfo.DaylightName, sizeof dstname);
			tzName[TRUE] = dstname;
            localtimeformat();

            /* Create a timer to update the image */

            wTimer = SetTimer(hWnd, ID_TIMER, wElapse, NULL);
		    ReleaseDC(hWnd, hDC);
		    srand((unsigned) (GetTickCount() & 0xFFFF));
		    iticks = 1;
            break;
        }

        case WM_TIMER:
			hDC = GetDC(hWnd);
			if (iticks == 0) {
				ringgg(hWnd, hDC, TRUE, FALSE);
			} else {
				iticks--;
			}
		    ReleaseDC(hWnd, hDC);
            break;

        case WM_DESTROY:
            if (wTimer) {
            	KillTimer(hWnd, ID_TIMER);
            }
            release_bitmaps();
            break;

        case WM_ERASEBKGND:
            GetClientRect(hWnd, &rc);
            FillRect((HDC) wParam, &rc,
				(HBRUSH) GetStockObject(BLACK_BRUSH));
            return 1L;

        default:
            break;
	}

    return DefScreenSaverProc(hWnd, Message, wParam, lParam);
}

/* RegisterDialogClasses -- Entry point for registering window
							classes required by configuration dialog box.

	Params:  hInst -- Handle to instance

	Return:  None
*/
BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return TRUE;
}


/* ScreenSaverConfigureDialog -- Dialog box function for configuration dialog.

	Params:  hWnd -- Handle to window

	Return:  None
*/

DialogueProc(ScreenSaverConfigureDialog)
{
	static HWND hIDOK;

    switch (message) {
        case WM_INITDIALOG:
            GetIniEntries();
            GetIniSettings();
            SendDlgItemMessage(hDlg, ID_SHOWMOON, BM_SETCHECK, showmoon, 0);
            hIDOK = GetDlgItem(hDlg, IDOK);
            return TRUE;

        case WM_COMMAND:
            switch (WM_COMMAND_ID(wParam)) {
                case IDOK:
				
                    showmoon = IsDlgButtonChecked(hDlg, ID_SHOWMOON);
					{
						HKEY keyh;
						DWORD dispos;
						TCHAR keyname[80];

						strcpy(keyname, "Software\\Fourmilab.ch\\");
						strcat(keyname, szAppName);
						if (RegCreateKeyEx(HKEY_CURRENT_USER,
									keyname,					// Subkey name
									0,							// Reserved
									"",							// Bogus class string
									REG_OPTION_NON_VOLATILE,	// Options
									KEY_ALL_ACCESS,				// Access rights			
									NULL,						// Security attributes
									&keyh,						// Returned handle to the key
									&dispos) == ERROR_SUCCESS) { // Returned disposition
								RegSetValueEx(keyh, szShowMoon, 0, REG_DWORD, 
									(BYTE *) &showmoon, sizeof(showmoon));
								RegCloseKey(keyh);
						}
					}

                    EndDialog(hDlg, TRUE);
                    return TRUE;

                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return TRUE;

                case ID_HELP:
DoHelp:
                    MessageBox(hDlg, "Home Planet: screen saver edition.",
                        szName, MB_OK);
                    break;
            }
            break;

        default:
            if (message == MyHelpMessage)     // Context sensitive help msg.
                goto DoHelp;
    }
    return FALSE;
}

/* GetIniSettings -- Get initial bounce settings from the registry.  */

static void GetIniSettings(void)
{
	HKEY keyh;
	TCHAR keyname[80];
	
	strcpy(keyname, "Software\\Fourmilab.ch\\");
	strcat(keyname, szAppName);

	if (RegOpenKeyEx(HKEY_CURRENT_USER,
			keyname,				// Subkey name
			0,						// Reserved
			KEY_QUERY_VALUE,		// Security access mask
			&keyh) == ERROR_SUCCESS) {
		DWORD vsize = sizeof(showmoon);
		DWORD vtype =  REG_DWORD;

		RegQueryValueEx(keyh, szShowMoon, NULL, &vtype, 
			(BYTE *) &showmoon, &vsize);
		RegCloseKey(keyh);
	} else {
		showmoon = TRUE;
	}
}

static void GetIniEntries(void)
{
  // Load common strings from the string table
  LoadString(hMainInstance, idsAppName, szAppName, sizeof(szAppName));
  LoadString(hMainInstance, idsScreenSaver, szScreenSaver, sizeof(szScreenSaver));
  LoadString(hMainInstance, idsHelpFile, szHelpFile, MAXFILELEN);
  LoadString(hMainInstance, idsNoHelpMemory, szNoHelpMemory, BUFFLEN);
}
#endif
