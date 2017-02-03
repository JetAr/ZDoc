/*

            Home Planet for Windows

    Dialogue message processing functions

*/

#include "sunclock.h"

/*  ABOUT  --  Dialogue message procedure for the About dialogue. */

DialogueProc(About)
{
    switch (message) {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            if (WM_COMMAND_ID(wParam) == IDOK ||
				WM_COMMAND_ID(wParam) == IDCANCEL) {
                EndDialog(hDlg, TRUE);
                return TRUE;
            }
            break;
    }
    return FALSE;
}

/*  SETJDATE  --  Dialogue message procedure for the Set Julian Date
                  dialogue. */

DialogueProc(SetJdate)
{
	long yy;
    int mm, dd, hh, mmm, ss;
    static double jd;
    struct tm gm;
    char tbuf[80];

    switch (message) {
        case WM_INITDIALOG:
            jd = faketime;
            sprintf(tbuf, Format(2), jd/*JD + 0.5*/);
            SetDlgItemText(hDlg, IDC_J_JDATE, (LPSTR) tbuf);
            jyear(jd, &yy, &mm, &dd);
            jhms(jd, &hh, &mmm, &ss);
            sprintf(tbuf, Format(6),
                hh, mmm, ss, dd,
                rstring(IDS_MONTH_NAMES + (mm - 1)), yy);
            SetDlgItemText(hDlg, IDC_J_UTIME, (LPSTR) tbuf);
            return TRUE;

        case WM_COMMAND:
            switch (WM_COMMAND_ID(wParam)) {
                case IDOK:
                	if (jd < 0) {
                		MessageBeep(MB_ICONSTOP);
                	} else {
	                    EndDialog(hDlg, TRUE);
	                    faketime = jd;
                    }
                    return TRUE;

                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return TRUE;

                case IDC_J_NOW:
					set_tm_time(&gm, FALSE);
                    jd = jtime(&gm);
                    sprintf(tbuf, Format(2), jd /*JD + 0.5*/);
                    SetDlgItemText(hDlg, IDC_J_JDATE, (LPSTR) tbuf);
                    return TRUE;

                case IDC_J_HELP:
                	WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                				((DWORD) ((LPSTR) rstring(IDS_HELP_ON_JDATE))));
                	holped = TRUE;
                	return TRUE;

                case IDC_J_JDATE:
                    if (HIWORD(wParam) == EN_CHANGE) {
                        GetDlgItemText(hDlg, IDC_J_JDATE,
                            (LPSTR) tbuf, (sizeof tbuf) - 1);
                        if (sscanf(tbuf, "%lf", &jd) > 0 && (jd >= 0/*JD.5*/)) {
 //*JD                           jd -= 0.5;
                            jyear(jd, &yy, &mm, &dd);
                            jhms(jd, &hh, &mmm, &ss);
                            sprintf(tbuf, Format(6),
                                hh, mmm, ss, dd,
                                rstring(IDS_MONTH_NAMES + (mm - 1)), yy);
                            SetDlgItemText(hDlg, IDC_J_UTIME, (LPSTR) tbuf);
                        } else {
                            SetDlgItemText(hDlg, IDC_J_UTIME, (LPSTR) Format(44));
                            jd = -1;
                        }
                    }
                    return TRUE;

                default:
                    break;
            }
            break;
    }
    return FALSE;
}

/*  SET_UTIME_FROM_JD  --  Decompose a Julian date into UTC date and
                           time and fill the edit fields with the
                           components. */

static int invalidUT = FALSE;		// Is current UTC specification invalid ?

static void set_utime_from_jd(HWND hDlg, double jd)
{
	long yy;
    int mm, dd, hh, mmm, ss;
    char tbuf[80];

    jyear(jd, &yy, &mm, &dd);
    jhms(jd, &hh, &mmm, &ss);
    sprintf(tbuf, "%ld", yy);
    SetDlgItemText(hDlg, IDC_U_YEAR, (LPSTR) tbuf);
    sprintf(tbuf, "%d", dd);
    SetDlgItemText(hDlg, IDC_U_DAY, (LPSTR) tbuf);
    sprintf(tbuf, "%d", hh);
    SetDlgItemText(hDlg, IDC_U_HOUR, (LPSTR) tbuf);
    sprintf(tbuf, "%d", mmm);
    SetDlgItemText(hDlg, IDC_U_MINUTE, (LPSTR) tbuf);
    sprintf(tbuf, "%d", ss);
    SetDlgItemText(hDlg, IDC_U_SECOND, (LPSTR) tbuf);
    SendDlgItemMessage(hDlg, IDC_U_MONTH, CB_SETCURSEL, mm - 1, 0L);
    invalidUT = FALSE;
}

/*  CHANGE_UTC  --  Change the Julian date when one or more
                    components of the UTC change. */

#define NC	-32671					// No change code

static void change_utc(HWND hDlg, double *jd, long nyy, int nmm, int ndd,
                                              int nhh, int nmmm, int nss)
{
	long yy;
    int mm, dd, hh, mmm, ss;
    char tbuf[80];

    jyear(*jd, &yy, &mm, &dd);
    jhms(*jd, &hh, &mmm, &ss);

    mm--;
#define Update(x) if (n ## x != NC) { x = n ## x; }
    Update(yy);
    Update(mm);
    Update(dd);
    Update(hh);
    Update(mmm);
    Update(ss);
#undef Update

    if (yy < -4712 || dd < 0 || dd > 31 || hh < 0 || hh >= 24 ||
    	mmm < 0 || mmm >= 60 || ss < 0 || ss >= 60 ||
    	(yy == -4712 && hh < 12)) {
    	// Invalid or out of range universal time specified
    	invalidUT = TRUE;
    	SetDlgItemText(hDlg, IDC_U_JDATE, (LPSTR) Format(44));
    } else {
    	invalidUT = FALSE;
	    *jd = ucttoj(yy, mm, dd, hh, mmm, ss);
	    sprintf(tbuf, Format(2), (*jd) /*JD + 0.5*/);
	    SetDlgItemText(hDlg, IDC_U_JDATE, (LPSTR) tbuf);
	}
}

/*  SETUTIME  --  Dialogue message procedure for the Set Universal Time
                  dialogue.  */

DialogueProc(SetUtime)
{
    int i;
    static double jd;
    struct tm gm;
    BOOL ok;
    char tbuf[80];

    switch (message) {
        case WM_INITDIALOG:

            jd = faketime;
            SendDlgItemMessage(hDlg, IDC_U_MONTH, WM_SETREDRAW, FALSE, 0L);
            for (i = 0; i < 12; i++) {
                SendDlgItemMessage(hDlg, IDC_U_MONTH, CB_ADDSTRING,
                    0,
                    (LONG) (LPSTR) rstring(IDS_MONTH_NAMES + i));
            }
            SendDlgItemMessage(hDlg, IDC_U_MONTH, WM_SETREDRAW, TRUE, 0L);
            sprintf(tbuf, Format(2), jd /*JD + 0.5*/);
            SetDlgItemText(hDlg, IDC_U_JDATE, (LPSTR) tbuf);
            set_utime_from_jd(hDlg, jd);
            return TRUE;

        case WM_COMMAND:
            switch (WM_COMMAND_ID(wParam)) {
                case IDOK:
                	if (invalidUT) {
                		MessageBeep(MB_ICONSTOP);
                	} else {
	                    EndDialog(hDlg, TRUE);
	                    faketime = jd;
	                }
                    return TRUE;

                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return TRUE;

                case IDC_U_NOW:
					set_tm_time(&gm, FALSE);
                    jd = jtime(&gm);
                    set_utime_from_jd(hDlg, jd);
                    return TRUE;

                case IDC_U_HELP:
                	WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                				((DWORD) ((LPSTR) rstring(IDS_HELP_ON_UTC))));
                	holped = TRUE;
                	return TRUE;

                case IDC_U_YEAR:
                    if (HIWORD(wParam) == EN_CHANGE) {
                        GetDlgItemText(hDlg, IDC_U_YEAR,
                            (LPSTR) tbuf, (sizeof tbuf) - 1);
                        change_utc(hDlg, &jd, atol(tbuf), NC, NC, NC, NC, NC);
                    }
                    return TRUE;

                case IDC_U_MONTH:
                    if (HIWORD(wParam) == CBN_SELCHANGE) {
                        int v = (int) SendDlgItemMessage(hDlg, IDC_U_MONTH,
                            CB_GETCURSEL, 0, 0L);
                        if (v != CB_ERR) {
                            change_utc(hDlg, &jd, NC, v, NC, NC, NC, NC);
                        }
                    }
                    return TRUE;

                case IDC_U_DAY:
                    if (HIWORD(wParam) == EN_CHANGE) {
                        WORD v = GetDlgItemInt(hDlg, IDC_U_DAY,
                            &ok, FALSE);
                        if (ok) {
                            change_utc(hDlg, &jd, NC, NC, v, NC, NC, NC);
                        }
                    }
                    return TRUE;

                case IDC_U_HOUR:
                    if (HIWORD(wParam) == EN_CHANGE) {
                        WORD v = GetDlgItemInt(hDlg, IDC_U_HOUR,
                            &ok, FALSE);
                        if (ok) {
                            change_utc(hDlg, &jd, NC, NC, NC, v, NC, NC);
                        }
                    }
                    return TRUE;

                case IDC_U_MINUTE:
                    if (HIWORD(wParam) == EN_CHANGE) {
                        WORD v = GetDlgItemInt(hDlg, IDC_U_MINUTE,
                            &ok, FALSE);
                        if (ok) {
                            change_utc(hDlg, &jd, NC, NC, NC, NC, v, NC);
                        }
                    }
                    return TRUE;

                case IDC_U_SECOND:
                    if (HIWORD(wParam) == EN_CHANGE) {
                        WORD v = GetDlgItemInt(hDlg, IDC_U_SECOND,
                            &ok, FALSE);
                        if (ok) {
                            change_utc(hDlg, &jd, NC, NC, NC, NC, NC, v);
                        }
                    }
                    return TRUE;

                default:
                    break;
            }
            break;
    }
    return FALSE;
}

/*  SATICONDLG  --  Select satellite icon dialogue.  */

DialogueProc(SatIconDlg)
{
	static int iconsel;

    switch (message) {
        case WM_INITDIALOG:
        	iconsel = satIconIndex;
        	SendDlgItemMessage(hDlg, IDC_SI_BUTTONS + iconsel, BM_SETCHECK,
        		TRUE, 0L);
			return TRUE;

        case WM_COMMAND:
            switch (WM_COMMAND_ID(wParam)) {
                case IDOK:
                	satIconIndex = iconsel;
                	if (satIcon != NULL) {
                		DestroyIcon(satIcon);
                		satIcon = NULL;
                		undrawsat(hWndMain);
                		drawsat(hWndMain);
                	}
                	skySatChanged();
                    EndDialog(hDlg, TRUE);
                    return TRUE;

                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return TRUE;

                case IDC_SI_HELP:
                	WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                				((DWORD) ((LPSTR) rstring(IDS_HELP_ON_SATICON))));
                	holped = TRUE;
                	return TRUE;

                default:
                	if (wParam >= IDC_SI_ICONS) {
                		wParam -= (IDC_SI_ICONS - IDC_SI_BUTTONS);
                	}
                	if (wParam >= IDC_SI_BUTTONS && wParam < IDC_SI_ICONS) {
                		iconsel = wParam - IDC_SI_BUTTONS;
        				SendDlgItemMessage(hDlg, IDC_SI_BUTTONS + iconsel, BM_SETCHECK,
        				TRUE, 0L);
                	}
                    break;
            }
            break;
    }
    return FALSE;
}


/*  SATTRACKDLG  --  Satellite tracking control panel (modeless dialogue).  */

char satDBFile[128] = "";			// Satellite database file
static HWND satd = NULL;      		// Satellite information dialogue
char whichSat[128] = "";			// Satellite we're tracking
HBITMAP satsbitmap;					// Save image under satellite bitmap
HICON satIcon = NULL;				// Current satellite icon
int satIconIndex = 0;				// Selected satellite icon
int satDisplayed = FALSE;			// Is satellite visible on screen ?
double satCalcTime = 0.0;			// Time satellite position last calculated
double satLAT = 0.0, satLONG = 0.0;	// Subsatellite latitude and longitude
double satAZ = 0.0, satEL = -1.0;	// Satellite azimuth and elevation at observing site

static void setSatelliteDia(struct satellite *s)
{
	if (satd != NULL) {
		if (strlen(s->satname) == 0) {
#define SetSat(field, val) SendDlgItemMessage(satd, (field), \
							WM_SETTEXT, 0, (LONG) ((LPSTR) (val)))
#define ClearSat(field)	SetSat(field, "")
			ClearSat(ST_SAT);
			ClearSat(ST_LYEAR);
			ClearSat(ST_LNO);
			ClearSat(ST_PIECE);
			ClearSat(ST_INC);
			ClearSat(ST_ECCENT);
			ClearSat(ST_REVDAY);
			ClearSat(ST_LAT);
			ClearSat(ST_LON);
			ClearSat(ST_ALT);
			ClearSat(ST_ORBNO);
			ClearSat(ST_ECLIPSE);
		} else {
			char ebuf[60];

	    	SetSat(ST_SAT, s->satname);
	    	sprintf(ebuf, "%d", s->intdes.launchyear + 1900 +
				(s->intdes.launchyear < 57 ? 100 : 0));
	    	SetSat(ST_LYEAR, ebuf);
	   		sprintf(ebuf, "%d", s->intdes.launchno);
	    	SetSat(ST_LNO, ebuf);
	    	SetSat(ST_PIECE, s->intdes.pieceno);
	    	sprintf(ebuf, "%.2f", s->inclination);
	    	SetSat(ST_INC, ebuf);
	    	sprintf(ebuf, "%.2f", s->eccentricity);
	    	SetSat(ST_ECCENT, ebuf);
	    	sprintf(ebuf, "%.2f", s->meanmotion);
	    	SetSat(ST_REVDAY, ebuf);

			ClearSat(ST_LAT);
			ClearSat(ST_LON);
			ClearSat(ST_ALT);
			ClearSat(ST_ORBNO);
			ClearSat(ST_ECLIPSE);
		}
	}
}

/*  UNDRAWSAT  --  Remove satellite icon from the screen.  */

void undrawsat(HWND hWnd)
{
	if (satDisplayed) {
		HDC hDC, hdcMem;
		HBITMAP obmap;

        hDC = GetDC(hWnd);
        hdcMem = CreateCompatibleDC(hDC);
        obmap = SelectObject(hdcMem, satsbitmap);
	    BitBlt(hDC, satx, saty, 32, 32, hdcMem, 0, 0, SRCCOPY);
	    SelectObject(hdcMem, obmap);
        DeleteDC(hdcMem);
        if (moonDisplayed) {
        	RECT rmoon, rsat, rint;

        	SetRect(&rmoon, moonx, moony, moonx + 31, moony + 31);
        	SetRect(&rsat, satx, saty, satx + 31, saty + 31);
        	if (IntersectRect(&rint, &rmoon, &rsat)) {
        		restoremoon(hDC);
        	}
        }
        ReleaseDC(hWnd, hDC);
		satDisplayed = FALSE;
	}
}

/*  DRAWSAT  --  Draw satellite icon at given latitude and longitude. */

void drawsat(HWND hWnd)
{
	if (tracking) {
		HDC hDC, hdcMem;
		HANDLE obmap;
		int restoreMoon = FALSE;

		hDC = GetDC(hWnd);
        hdcMem = CreateCompatibleDC(hDC);
        obmap = SelectObject(hdcMem, satsbitmap);

        /* This is a little messy, but attempts to do it "cleanly" with
           child windows ran into horrors of the Windows API the description of
           which this fragile hard disc is incapable of storing.  The little dance
           we're about to do is to prevent clashes between the images
           of the Moon and the satellite we're tracking (if any).  We
           need to make sure that the background image we save comes
           from the unpolluted screen.  So, if a satellite is displayed
           and its icon rectangle intersects that of the Moon, undraw the Moon,
           grab our background image, then finally put it back on the
           screen. */

        if (moonDisplayed) {
        	RECT rmoon, rsat, rint;

        	SetRect(&rmoon, moonx, moony, moonx + 31, moony + 31);
        	SetRect(&rsat, satx, saty, satx + 31, saty + 31);
        	if (IntersectRect(&rint, &rmoon, &rsat)) {
        		undrawmoon(hDC);
        		restoreMoon = TRUE;
        	}
        }
        // Save image under satellite icon
	    BitBlt(hdcMem, 0, 0, 32, 32, hDC, satx, saty, SRCCOPY);
	    SelectObject(hdcMem, obmap);
        DeleteDC(hdcMem);

		if (satIcon == NULL) {
			static char siname[] = "sati?";

			siname[4] = (char) ('a' + satIconIndex);
			satIcon = LoadIcon(hInst, siname);
		}
        if (restoreMoon) {
        	/* Restore the Moon first so that the satellite consistently
        	   passes "in front" of the Moon. */
        	restoremoon(hDC);
        }
		DrawIcon(hDC, satx, saty, satIcon);
        ReleaseDC(hWnd, hDC);
        satDisplayed = TRUE;
	}
}

/*  RESTORESAT  --  Restore satellite icon after a clash with the Moon.  */

void restoresat(HWND hWnd)
{
	if (tracking && (satIcon != NULL)) {
		HDC hDC;

		hDC = GetDC(hWnd);
		DrawIcon(hDC, satx, saty, satIcon);
        ReleaseDC(hWnd, hDC);
        satDisplayed = TRUE;
	}
}

/*  HITSAT  --  Test if mouse hit the satellite icon.  */

int hitsat(int x, int y)
{
	return satDisplayed && (x >= satx) && (x < (satx + 32)) &&
						   (y >= saty) && (y < (saty + 32));
}

/*  UPDSAT  --  Update satellite position.  */

int updsat(double jd, int dotrail, int repaint)
{
    double now, smaxis, raanp, perigeep, reforbit, epday,
           avgmotion, currmotion, currorbit, meana, truea,
           ratx, raty, ratz, radius, satvx, satvy, satvz,
           ssplat, ssplong, sspheight, siteX, siteY, siteZ, siteVX, siteVY;
	mat3x3 siteMatrix;
    long iorbitnum;
    char ebuf[40];
    static int initialised = FALSE;
    int changed = FALSE;

	now = jd - 2415019.5;
    if (!initialised) {
        InitOrbitRoutines(now);
        initialised = TRUE;
    }
    epday = sat.epochday + GetDayNum(sat.epochyear, 1, 0);
    smaxis = 331.25 * exp(2.0 * log((24 * 60.0) / sat.meanmotion) / 3.0);
    GetPrecession(smaxis, sat.eccentricity, dtr(sat.inclination),
        &raanp, &perigeep);
    reforbit = dtr(sat.meananomaly) / (2 * PI) + sat.revno;

    while (TRUE) {
        avgmotion = sat.meanmotion + (now - epday) * (sat.meanmotiond1 / 2);
        currmotion = sat.meanmotion + (now - epday) * sat.meanmotiond1;
        smaxis = 331.25 * exp(2.0 * log((24 * 60.0) / currmotion) / 3.0);
        currorbit = reforbit + (now - epday) * avgmotion;
        iorbitnum = (long) currorbit;

        meana = (currorbit - iorbitnum) * (2 * PI);
        truea = Kepler(meana, sat.eccentricity);

        GetSatPosition(epday, dtr(sat.rascendnode), dtr(sat.argperigee),
            smaxis, dtr(sat.inclination), sat.eccentricity, raanp, perigeep,
            now, truea, &ratx, &raty, &ratz, &radius, &satvx, &satvy, &satvz);

        GetSubSatPoint(ratx, raty, ratz, now, &ssplat, &ssplong, &sspheight);

        /* If the object has re-entered, complain about bogus elements and
           terminate tracking. */

        if (sspheight <= 0.0) {
        	char errm[256];

        	sprintf(errm, rstring(IDS_ERR_SATELEM), whichSat);
			strcpy(whichSat, rstring(IDS_NONE));
			strcpy(sat.satname, "");
			tracking = FALSE;
			repaint = TRUE;
			changed = TRUE;
			sspheight = 1000;		// Allows blundering through rest of code
        	MessageBeep(MB_ICONHAND);
        	MessageBox(NULL, errm, rstring(IDS_APPNAME), MB_ICONHAND | MB_OK | MB_TASKMODAL);
        }

        satLAT = ssplat;
        satLONG = ssplong;
        satCalcTime = jd;				// Save calculation time for satellite

		GetSitPosition(dtr(siteLat), dtr(siteLon), 0.0, now, &siteX, &siteY, &siteZ,
					   &siteVX, &siteVY, siteMatrix);
		GetBearings(ratx, raty, ratz, siteX, siteY, siteZ, siteMatrix,
				    &satAZ, &satEL);
#ifdef SatChatty
{
	char p[80];
	sprintf(p, "Sat: Azimuth = %.3f Elevation = %.3f\n\r", rtd(satAZ), rtd(satEL));
	OutputDebugString(p);
}
#endif
        if (!dotrail) {
			int sx, sy;
			double lon, lat;

            lon = rtd(ssplong);
            lat = rtd(ssplat);
			if (lon > 180) {
				lon -= 360;
			} else if (lon < -180) {
        		lon += 360;
			}
			sy = ((int) (ydots - (lat + 90) * (ydots / 180.0))) - 16;
			sx = ((int) (xdots - (180 + lon) * (xdots / 360.0))) - 16;

			/* If the tracking control panel dialogue is visible, update
			   the current position fields in it. */

            if (satTrackDlg != NULL) {
            	if (tracking) {
	            	sprintf(ebuf, "%.2f %c", abs(rtd(ssplat)), ssplat < 0 ? 'S' : 'N');
	            	SetSat(ST_LAT, ebuf);
	            	sprintf(ebuf, "%.2f %c", rtd(ssplong) > 180 ?
	                                         360 - rtd(ssplong) : rtd(ssplong),
	                                     	 rtd(ssplong) > 180 ? 'E' : 'W');
	            	SetSat(ST_LON, ebuf);
	            	sprintf(ebuf, "%.2f km", sspheight);
	            	SetSat(ST_ALT, ebuf);
	            	sprintf(ebuf, "%.2f", currorbit);
	            	SetSat(ST_ORBNO, ebuf);
	            	SetSat(ST_ECLIPSE, Eclipsed(ratx, raty, ratz, radius, now) ? "Yes" : "No");
	            } else {
					ClearSat(ST_LAT);
					ClearSat(ST_LON);
					ClearSat(ST_ALT);
					ClearSat(ST_ORBNO);
					ClearSat(ST_ECLIPSE);
            	}
            }

			if (repaint || sx != satx || sy != saty) {
				undrawsat(hWndMain);
				satx = sx;
				saty = sy;
				drawsat(hWndMain);
				changesatview(ssplat, ssplong, sspheight);
				changed = TRUE;
			}
            break;
        }
    }
    return changed;
}

/*  LOADDIA  --  Initialise list box in dialogue when called back from INSAT().  */

static int loadDia(struct satellite *s)
{
	SendDlgItemMessage(satd, ST_LIST, LB_ADDSTRING, 0, (LONG) ((LPSTR) s->satname));
	return TRUE;
}

/*  LOADSAT  --  Set elements of satellite being tracked when called back from INSAT().  */

static int loadSat(struct satellite *s)
{
	if (strcmp(s->satname, whichSat) == 0) {
		sat = *s;
		return FALSE;
	}
	return TRUE;
}

/*  IGNORESAT  --  Do nothing at all when called back from INSAT().  Used for syntax
				   checking files dropped on us from the file manager.  */

static int ignoreSat(struct satellite *s)
{
	return TRUE;
}

void setSatFromName(void)
{
	FILE *fp;
	int err, linerr;
	struct satellite osat;

    osat = sat;
    if (strcmp(whichSat, rstring(IDS_NONE)) == 0) {
    	strcpy(sat.satname, "");
    } else {
		fp = fopen(satDBFile, "r");
		if (fp != NULL) {
			insat(fp, loadSat, &err, &linerr);
			fclose(fp);
		} else {
			strcpy(sat.satname, "");
		}
	}
	tracking = strlen(sat.satname) > 0;
	setSatelliteDia(&sat);
	if (memcmp(&osat, &sat, sizeof sat) != 0) {
		satviewchanged();
	}
}

/*  INITSATTRACKDLG  --  Initialise the satellite tracking dialogue with
						 the satellites from the current database.  */

static void initSatTrackDlg(HWND hDlg)
{
	FILE *fp;
	int err, linerr;

    SendDlgItemMessage(hDlg, ST_LIST, LB_RESETCONTENT, 0, 0L);
	SendDlgItemMessage(hDlg, ST_LIST, LB_ADDSTRING, 0,
		(LONG) ((LPSTR) rstring(IDS_NONE)));
	fp = fopen(satDBFile, "r");
	if (fp != NULL) {
		insat(fp, loadDia, &err, &linerr);
		fclose(fp);
	}
	if (err != 0) {
		char tbuf[80];

		sprintf(tbuf, "Error in satellite elements database on line %d.", linerr);
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(hDlg, tbuf,
						 "Invalid Satellite Database", MB_ICONEXCLAMATION | MB_OK);
	}
	setSatelliteDia(&sat);
	if (strlen(sat.satname) > 0) {
		SendDlgItemMessage(hDlg, ST_LIST, LB_SELECTSTRING, (WPARAM) -1,
				(LONG) ((LPSTR) sat.satname));
	}
}


DialogueProc(SattrackDlg)
{
	satd = hDlg;

	switch (message) {
		case WM_INITDIALOG:
			DragAcceptFiles(hDlg, TRUE);
			// Wheee!!!  Note fall-through

		case WM_ST_NEWDBFILE:
			initSatTrackDlg(hDlg);
			return TRUE;

		case WM_CLOSE:
			DestroyWindow(hDlg);
			return TRUE;

		case WM_DESTROY:
			DragAcceptFiles(hDlg, FALSE);
			satTrackDlg = NULL;
			return TRUE;

        case WM_COMMAND:
            switch (WM_COMMAND_ID(wParam)) {
            	case ST_C_TRACK:
            		wParam = MAKELONG(0, LBN_DBLCLK);
            		// Wheeee!!!  Note fall-through

            	case ST_LIST:
            		if (HIWORD(wParam) == LBN_DBLCLK) {
            			SendDlgItemMessage(hDlg, ST_LIST, LB_GETTEXT,
							(WORD) SendDlgItemMessage(hDlg, ST_LIST,
								LB_GETCURSEL, 0, 0L),
							(LONG) ((LPSTR) whichSat));
						setSatFromName();
						undrawsat(hWndMain);
						if (tracking) {
							updsat(faketime, FALSE, TRUE);
						}
            		}
            		break;

                case ST_C_HELP:
                	WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                				((DWORD) ((LPSTR) rstring(IDS_HELP_ON_SATRACK))));
                	holped = TRUE;
                	return TRUE;

                case IDOK:
                    SendMessage(hDlg, WM_CLOSE, 0, 0L);
                    return TRUE;

                default:
                	break;
            }
            break;

        case WM_DROPFILES:
        	{	char dbfile[sizeof satDBFile];
        		int nfiles;

        		nfiles = DragQueryFile((HDROP) wParam, 0xFFFF, (LPSTR) NULL, 0);
        		if (nfiles > 0) {
					FILE *fp;
					int err, linerr;

        			DragQueryFile((HDROP) wParam, 0, dbfile, sizeof dbfile);
        			DragFinish((HDROP) wParam);
					fp = fopen(dbfile, "r");
					if (fp != NULL) {
						insat(fp, ignoreSat, &err, &linerr);
						fclose(fp);
					}
					if (err == 0) {
						strcpy(satDBFile, dbfile);
						SendMessage(hDlg, WM_ST_NEWDBFILE, 0, 0L);
					} else {
						MessageBeep(MB_ICONEXCLAMATION);
						MessageBox(hDlg, "Dropped file is not a valid satellite elements database.",
										 "Invalid File", MB_ICONEXCLAMATION | MB_OK);
					}
        		}
        	}
        	break;

        default:
        	break;

	}
	return FALSE;
}
