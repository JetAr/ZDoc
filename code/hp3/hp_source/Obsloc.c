/*

            Sunclock for Windows

    	Set Observer Location dialogue

*/

#include "sunclock.h"

static double currlat, currlon;				// Working latitude and longitude
static char currsite[sizeof siteName];      // Working site name
static FILE *fp;							// Site name database file
static int firstmod;						// First explicit entry in latitude/longitude box
static int chasetail = TRUE;				// Respond to changes in edit fields ?

/*  DEGTODMS  --  Convert fractional degrees to degrees, minutes, and seconds.  */

static void degToDMS(double degrees, double *deg, double *min, double *sec)
{
	*deg = (int) degrees;
	*min = (int) ((degrees - *deg) * 60);
	*sec = (degrees - (*deg + (*min / 60))) * 3600;
}

/*  FILLLATLON  --  Fill latitude and longitude edit fields from given values.  */

static void fillLatLon(HWND hDlg, double lat, double lon)
{
	double d, m, s;
	char tbuf[20];
	int sfm = firstmod;

	firstmod = chasetail = FALSE;
	degToDMS(abs(lat), &d, &m, &s);
#define Sf(value, field) sprintf(tbuf, "%.0f", value); SetDlgItemText(hDlg, field, (LPSTR) tbuf)
	Sf(d, IDM_OL_LATDEG);
	Sf(m, IDM_OL_LATMIN);
	Sf(s, IDM_OL_LATSEC);
	SendDlgItemMessage(hDlg, IDM_OL_NORS, CB_SETCURSEL, (lat >= 0) ? 0 : 1, 0L);
	degToDMS(abs(lon), &d, &m, &s);
	Sf(d, IDM_OL_LONDEG);
	Sf(m, IDM_OL_LONMIN);
	Sf(s, IDM_OL_LONSEC);
	SendDlgItemMessage(hDlg, IDM_OL_EORW, CB_SETCURSEL, (lon >= 0) ? 1 : 0, 0L);
	chasetail = TRUE;
	firstmod = sfm;
}

/*  UPDATELATLON  --  Update latitude and longitude from edit fields.  */

static void updateLatLon(HWND hDlg)
{
	char tbuf[80];

#define Df(field) (GetDlgItemText(hDlg, field, (LPSTR) tbuf, (sizeof tbuf) - 1), atof(tbuf))

	/* This excrescence of written-out code is thanks, as usual, to
	   the Monkey C (.NET) "optimiser", which screws up the following
	   expression, evaluting the dialogue fields in precisely ass-backward
	   order. */
//	currlat = (Df(IDM_OL_LATDEG) + (Df(IDM_OL_LATMIN) / 60.0) + (Df(IDM_OL_LATSEC) / 3600.0))
//				* ((SendDlgItemMessage(hDlg, IDM_OL_NORS, CB_GETCURSEL, 0, 0L) == 1) ? -1 : 1);
	currlat = Df(IDM_OL_LATDEG);
	currlat += Df(IDM_OL_LATMIN) / 60.0;
	currlat += Df(IDM_OL_LATSEC) / 3600.0;
	currlat *= (SendDlgItemMessage(hDlg, IDM_OL_NORS, CB_GETCURSEL, 0, 0L) == 1) ? -1 : 1;
//	currlon = (Df(IDM_OL_LONDEG) + (Df(IDM_OL_LONMIN) / 60.0) + (Df(IDM_OL_LONSEC) / 3600.0))
//				* ((SendDlgItemMessage(hDlg, IDM_OL_EORW, CB_GETCURSEL, 0, 0L) == 1) ? 1 : -1);
	currlon = Df(IDM_OL_LONDEG);
	currlon += Df(IDM_OL_LONMIN) / 60.0;
	currlon += Df(IDM_OL_LONSEC) / 3600.0;
	currlon *= (SendDlgItemMessage(hDlg, IDM_OL_EORW, CB_GETCURSEL, 0, 0L) == 1) ? 1 : -1;

	/* If the user enters a custom latitude and longitude, the site name in the
	   combo box is now invalid.  Clear it to encourage him to enter a site
	   name. */

	if (firstmod) {
		firstmod = FALSE;
		strcpy(currsite, "");
		SendDlgItemMessage(hDlg, IDM_OL_SITENAME, WM_SETTEXT, 0, (LPARAM) (LPSTR) currsite);
	}
}

/*  SITEPICKED  --  Fire up dialogue again after user picks site on map.  */

void sitePicked(WORD px, WORD py, WORD sx, WORD sy)
{
    DLGPROC lpfnMsgProc;
    int nRc;

	currlat = -(((double) py / sy) * 180 - 90);
	currlon = -(((double) px / sx) * 360 - 180);
	strcpy(currsite, "");
	lpfnMsgProc = (DLGPROC) MakeProcInstance((FARPROC) SetObserverLocation, hInst);
	nRc = DialogBox(hInst, MAKEINTRESOURCE(IDM_OBSLOC), hWndMain, lpfnMsgProc);
	FreeProcInstance((FARPROC) lpfnMsgProc);
	if (nRc) {
		planetSiteChanged();
		if (tracking) {
			// Must update satellite position as seen from new site
			updsat(faketime, FALSE, FALSE);
		} else {
        	satviewchanged();
        }
		updateSky(faketime, TRUE);
	}
}

/*  SETOBSERVERLOCATION  --  Main dialogue procedure.  */

DialogueProc(SetObserverLocation)
{
    char tbuf[80];

    switch (message) {
        case WM_INITDIALOG:
        	firstmod = FALSE;
        	if (sitePicking) {
        		sitePicking = FALSE;
        		mapCursor = LoadCursor(NULL, IDC_ARROW);
        	} else {
        		currlat = siteLat;
        		currlon = siteLon;
        		strcpy(currsite, siteName);
        	}
        	SendDlgItemMessage(hDlg, IDM_OL_NORS, CB_ADDSTRING, 0, (LPARAM) ((LPCSTR) rstring(IDS_NORTH)));
        	SendDlgItemMessage(hDlg, IDM_OL_NORS, CB_ADDSTRING, 0, (LPARAM) ((LPCSTR) rstring(IDS_SOUTH)));
        	SendDlgItemMessage(hDlg, IDM_OL_EORW, CB_ADDSTRING, 0, (LPARAM) ((LPCSTR) rstring(IDS_EAST)));
        	SendDlgItemMessage(hDlg, IDM_OL_EORW, CB_ADDSTRING, 0, (LPARAM) ((LPCSTR) rstring(IDS_WEST)));
            SendDlgItemMessage(hDlg, IDM_OL_SITENAME, CB_LIMITTEXT, 59, 0L);
            fp = fopen("sitename.txt", "r");
            if (fp != NULL) {
            	SendDlgItemMessage(hDlg, IDM_OL_SITENAME, WM_SETREDRAW, FALSE, 0L);
            	while (fgets(tbuf, sizeof tbuf, fp)) {
            		if (tbuf[0] != ';' && tbuf[0] != ' ') {
            			tbuf[strlen(tbuf) - 1] = 0;
                		SendDlgItemMessage(hDlg, IDM_OL_SITENAME, CB_ADDSTRING,
                    							 0, (LPARAM) (LPSTR) (tbuf + ((tbuf[17] == '+') ? 18 : 17)));
            		}
            	}
            }
            SendDlgItemMessage(hDlg, IDM_OL_SITENAME, WM_SETTEXT,
                   				   0, (LPARAM) (LPSTR) currsite);
            SendDlgItemMessage(hDlg, IDM_OL_SITENAME, WM_SETREDRAW, TRUE, 0L);
        	fillLatLon(hDlg, currlat, currlon);
        	firstmod = TRUE;					// Have first mod clear site name
            return TRUE;

        case WM_DESTROY:
        	if (fp != NULL) {
        		fclose(fp);
        	}
        	return TRUE;

        case WM_COMMAND:
            switch (WM_COMMAND_ID(wParam)) {
                case IDOK:
                	SendDlgItemMessage(hDlg, IDM_OL_SITENAME, WM_GETTEXT,
                							 	 sizeof(currsite) - 1, (LPARAM) (LPCSTR) currsite);
                	if (strlen(currsite) > 0) {
                		siteLat = currlat;
                		siteLon = currlon;
                		strcpy(siteName, currsite);
                    	EndDialog(hDlg, TRUE);
                    } else {
                    	MessageBeep(MB_ICONQUESTION);
                    	MessageBox(hDlg, rstring(IDS_NEED_SITENAME), NULL,
                    		MB_ICONQUESTION | MB_OK);
                    }
                    return TRUE;

                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return TRUE;

                case IDM_OL_LATDEG:
                case IDM_OL_LATMIN:
                case IDM_OL_LATSEC:
                case IDM_OL_LONDEG:
                case IDM_OL_LONMIN:
                case IDM_OL_LONSEC:
                    if (HIWORD(wParam) == EN_CHANGE && chasetail) {
                    	updateLatLon(hDlg);
                    }
                	return TRUE;

                case IDM_OL_NORS:
                case IDM_OL_EORW:
                    if (HIWORD(wParam) == CBN_SELCHANGE && chasetail) {
                    	updateLatLon(hDlg);
                    }
                 	return TRUE;

                case IDM_OL_HELP:
                	WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                				((DWORD) ((LPSTR) rstring(IDS_HELP_ON_OBSLOC))));
                	holped = TRUE;
                	return TRUE;

                case IDM_OL_SITENAME:
                	if (HIWORD(wParam) == CBN_SELCHANGE || HIWORD(wParam) == CBN_DBLCLK) {
                		if (fp != NULL) {
                			int item = (int) SendDlgItemMessage(hDlg, IDM_OL_SITENAME,
                							CB_GETCURSEL, 0, 0L);

                			rewind(fp);
            				while (fgets(tbuf, sizeof tbuf, fp)) {
            					if (tbuf[0] != ';' && tbuf[0] != ' ') {
            						if (item-- <= 0) {
										currlat = atol(tbuf) / 10000.0;
										currlon = atol(tbuf + 8) / 10000.0;
        								fillLatLon(hDlg, currlat, currlon);
        								break;
            						}
            					}
            				}
                	    }
                	}
                	return TRUE;

                case IDM_OL_CLOSECITY:
            		if (fp != NULL) {
            			int item = 0, bitem;
            			double cl, cx, cy, cz, dx, dy, dz, dd, ilat, ilon, acl, bd = 2;

                		cl = fixangle(currlon);
                		cx = cos(dtr(cl)) * abs(cos(dtr(currlat)));
                		cy = sin(dtr(cl)) * abs(cos(dtr(currlat)));
                		cz = sin(dtr(currlat));
            			rewind(fp);
        				while (fgets(tbuf, sizeof tbuf, fp)) {
        					if (tbuf[0] != ';' && tbuf[0] != ' ') {
								if (tbuf[17] == '+') {		// Only consider "major cities"
									ilat = dtr(atol(tbuf) / 10000.0);
									ilon = dtr(fixangle(atol(tbuf + 8) / 10000.0));
	       							acl = abs(cos(ilat));
	                				dx = cx - cos(ilon) * acl;
	                				dy = cy - sin(ilon) * acl;
	                				dz = cz - sin(ilat);
	                				if ((dd = (dx * dx + dy * dy + dz * dz)) < bd) {
	                					bitem = item;
	                					bd = dd;
	                				}
                				}
                				item++;
        					}
        				}
        				SendDlgItemMessage(hDlg, IDM_OL_SITENAME, CB_SETCURSEL, (WPARAM) bitem, 0L);
            	    }
                	return TRUE;

                case IDM_OL_PICK:
                	sitePicking = TRUE;
                	if (xhairCursor == NULL) {
                		xhairCursor = LoadCursor(hInst, "xhairs");
                	}
                	mapCursor = xhairCursor;					// Set cross-hair cursor
                	EndDialog(hDlg, FALSE);						// Return to map window for pick
                	return TRUE;

                default:
                    break;
            }
            break;
    }
    return FALSE;
}
