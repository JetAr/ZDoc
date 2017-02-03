/*

          Home Planet for Windows

        Sun and Moon information panel

*/

#include "sunclock.h"

static double nptime = 0.0;        /* Next new moon time */
int moonDisplayed = FALSE;         /* Is Moon on screen ? */
HBITMAP moonsbitmap;               /* Moon save background bitmap */
int moonx, moony;                  /* Moon position on screen */
double moonposLast = -1E10;        /* Moon last position calculation */
static double moonposInterval = CalculationInterval(60);
double moonphaseLast = -1E10;      /* Moon last phase calculation */
static double moonphaseInterval = CalculationInterval(300);
HICON micon = NULL;                /* Moon icon calculated for correct phase */

#define Plural(x, n) (x), (LPSTR) rstring(n + ((x) == 1 ? 0 : 1))
#define Set(field) SendDlgItemMessage(sunMoonPanel, (field), \
                            WM_SETTEXT, 0, (LONG) ((LPSTR) (tbuf)))

#ifndef ScreenSaver

/*  FMT_PHASE_TIME  --  Format  the  provided  julian  date  into  the
                        provided  buffer  in  UTC  format  for  screen
                        display  */

static void fmt_phase_time(double utime, char *buf)
{
    long yy;
    int mm, dd, hh, mmm, ss;

    jyear(utime, &yy, &mm, &dd);
    jhms(utime, &hh, &mmm, &ss);
    wsprintf((LPSTR) buf, (LPSTR) Format(22),
            hh, mmm, dd, (LPSTR) rstring(IDS_MONTH_NAMES + (mm - 1)), yy);
}
#endif

/*  UNDRAWMOON  --  Remove moon icon from screen and replace with background at the the
                    time it was drawn.  */

void undrawmoon(HDC hDC)
{
    if (moonDisplayed) {
        HDC hdcMem;
        HBITMAP obmap;

        hdcMem = CreateCompatibleDC(hDC);
        obmap = SelectObject(hdcMem, moonsbitmap);
        BitBlt(hDC, moonx, moony, 32, 32, hdcMem, 0, 0, SRCCOPY);
        SelectObject(hdcMem, obmap);
        DeleteDC(hdcMem);
        moonDisplayed = FALSE;
#ifndef ScreenSaver
        if (satDisplayed) {
            RECT rmoon, rsat, rint;

            SetRect(&rmoon, moonx, moony, moonx + 31, moony + 31);
            SetRect(&rsat, satx, saty, satx + 31, saty + 31);
            if (IntersectRect(&rint, &rmoon, &rsat)) {
                restoresat(hWndMain);
            }
        }
#endif
    }
}

/*  DRAWMOON  --  Draw moon icon on the screen at the position calculated for
                  moonx and moony.  */

void drawmoon(HDC hDC)
{
    if (showmoon && (micon != NULL)) {
#ifdef ScreenSaver
		if (fChildPreview) {
			return;					// No moon if showing preview		
		}
#else
        HDC hdcMem;
        HANDLE obmap;
        int restoreSat = FALSE;

        hdcMem = CreateCompatibleDC(hDC);
        obmap = SelectObject(hdcMem, moonsbitmap);

        /* This is a little messy, but attempts to do it "cleanly" with
           child windows ran into horrors of the Windows API the description of
           which this fragile hard disc is incapable of storing.  The little dance
           we're about to do is to prevent clashes between the images
           of the Moon and the satellite we're tracking (if any).  We
           need to make sure that the background image we save comes
           from the unpolluted screen.  So, if a satellite is displayed
           and its icon rectangle intersects that of the Moon, undraw it,
           grab our background image, then finally put it back on the
           screen. */

        if (satDisplayed) {
            RECT rmoon, rsat, rint;

            SetRect(&rmoon, moonx, moony, moonx + 31, moony + 31);
            SetRect(&rsat, satx, saty, satx + 31, saty + 31);
            if (IntersectRect(&rint, &rmoon, &rsat)) {
                undrawsat(hWndMain);
                restoreSat = TRUE;
            }
        }

        // Save bits underlying the moon icon
        BitBlt(hdcMem, 0, 0, 32, 32, hDC, moonx, moony, SRCCOPY);
        SelectObject(hdcMem, obmap);
        DeleteDC(hdcMem);
#endif
        DrawIcon(hDC, moonx, moony, micon);
        moonDisplayed = TRUE;
#ifndef ScreenSaver
        if (restoreSat) {
            /* Restore the satellite after painting the Moon so that the
               satellite appears to pass "in front of" the Moon. */
            restoresat(hWndMain);
        }
#endif
    }
}

#ifndef ScreenSaver

/*  RESTOREMOON  --  Repaint Moon after clash with a satellite icon.  */

void restoremoon(HDC hDC)
{
    if (showmoon && (micon != NULL)) {
        DrawIcon(hDC, moonx, moony, micon);
        moonDisplayed = TRUE;
    }
}


/*  HITMOON  --  Test if mouse hit the moon icon.  */

int hitmoon(int x, int y)
{
    if (moonDisplayed) {
        long mcx, mcy, mdsq;

        mcx = x - (moonx + 16);
        mcy = y - (moony + 16);
        mdsq = (mcx * mcx) + (mcy * mcy);
        return mdsq < (15 * 15);
    }
    return FALSE;
}
#endif

/*  ICONMOON  --  Construct icon for moon, given phase of moon.  */

static void iconmoon(HDC hDC, char *bmpname, int width, int CENTER,
                     int IRADIUS, int OFFSET,
                     int xpos, int ypos, double ph, int mm, int dd)
{
    int i, lx, rx, colour_mode;
    double cp, xscale, RADIUS = IRADIUS;

    HDC hMemoryDC;
    HBITMAP hColourmoon;
    unsigned char moon_template[] = {
        255,255,255,255,255,224,7,255,255,128,1,255,255,0,0,255,252,0,0,63,252,
        0,0,63,248,0,0,31,240,0,0,15,224,0,0,15,224,0,0,7,224,0,0,7,192,0,0,3,
        192,0,0,3,192,0,0,3,192,0,0,3,192,0,0,3,192,0,0,3,224,0,0,7,224,0,0,7,
        224,0,0,7,240,0,0,15,240,0,0,15,248,0,0,31,252,0,0,63,254,0,0,127,255,
        0,0,255,255,192,3,255,255,240,31,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255
    };

    /* If the moon is new, draw an empty black square. */

    colour_mode = width == 64;
    if ((colour_mode == TRUE) && ((ph < 0.01) || (ph > 0.99))) {
        BitBlt(hDC, xpos, ypos, 58, 58, NULL, 0, 0, BLACKNESS);
        return;
    }

    hColourmoon = LoadBitmap(hInst, bmpname);

    hMemoryDC = CreateCompatibleDC(hDC);
    SelectObject(hMemoryDC, hColourmoon);

    /* If it's July 20th (in local time if we're running in real time,
       otherwise based on UTC), display the  Apollo  11  Commemorative
       Red  Dot at Tranquility Base.  Otherwise, just show the regular
       mare floor.  */

    if (mm != 7 || dd != 20) {
        if (colour_mode) {
            SetPixel(hMemoryDC, 41, 29, GetPixel(hMemoryDC, 42, 29));
        } else {
            SetPixel(hMemoryDC, 20, 14, GetPixel(hMemoryDC, 21, 14));
        }
    }

    xscale = cos(2 * PI * ph);
    for (i = 0; i < IRADIUS; i++) {
        cp = RADIUS * cos(asin((double) i / RADIUS));
        if (ph < 0.5) {
            rx = (int) (CENTER + cp);
            lx = (int) (CENTER + xscale * cp);
        } else {
            lx = (int) (CENTER - cp);
            rx = (int) (CENTER - xscale * cp);
        }

        /* We  now know the left and right  endpoints of the scan line
           for this y  coordinate.   We  raster-op  the  corresponding
           scanlines  from  the  source  pixrect  to  the  destination
           pixrect, offsetting to properly place it in the pixrect and
           reflecting vertically. */

        if (lx > 0) {
            BitBlt(hMemoryDC, 0, OFFSET + i, lx, 1, NULL, 0, 0, BLACKNESS);
            if ((rx + 1) < width) {
                BitBlt(hMemoryDC, rx + 1, OFFSET + i, width - rx, 1, NULL, 0, 0, BLACKNESS);
            }
            if (i != 0) {
                BitBlt(hMemoryDC, 0, OFFSET - i, lx, 1, NULL, 0, 0, BLACKNESS);
                if ((rx + 1) < width) {
                    BitBlt(hMemoryDC, rx + 1, OFFSET - i, width - rx, 1, NULL, 0, 0, BLACKNESS);
                }
            }
        }
    }

    if (colour_mode) {
        BitBlt(hDC, xpos, ypos, 58, 58, hMemoryDC, 2, 0, SRCCOPY);
    } else {
        BITMAP cmb;
        LPSTR mbitmap;
        HGLOBAL gbm;

        GetObject(hColourmoon, sizeof cmb, &cmb);

        gbm = GlobalAlloc(GMEM_MOVEABLE, ((DWORD) cmb.bmWidthBytes) * cmb.bmHeight * cmb.bmPlanes);
        if (gbm != NULL) {
            mbitmap = GlobalLock(gbm);
            GetBitmapBits(hColourmoon, ((long) cmb.bmWidthBytes) * cmb.bmHeight * cmb.bmPlanes, mbitmap);
            if (micon != NULL) {
                DestroyIcon(micon);
                micon = NULL;
            }
            micon = CreateIcon(hInst, cmb.bmWidth, cmb.bmHeight, (BYTE) cmb.bmPlanes,
                                      (BYTE) cmb.bmBitsPixel, moon_template, mbitmap);
            GlobalUnlock(gbm);
            GlobalFree(gbm);
//          moonx = xpos - 16;
//          moony = ypos - 13;
        }
    }
    DeleteDC(hMemoryDC);
    DeleteObject(hColourmoon);
}

#ifndef ScreenSaver

void upd_sun_moon(double jd, int realtime, int repaint)
{
    int lunation, i, mm, dd, hh, mmm, ss, aom_d, aom_h, aom_m;
    long yy;
    double p, aom, cphase, cdist, cangdia, csund, csuang;
    double phasar[5];
    char tbuf[80];
    struct tm gm;
    HWND mimage;
    HDC hDC;

    p = phase(jd, &cphase, &aom, &cdist, &cangdia, &csund, &csuang);
    aom_d = (int) aom;
    aom_h = (int) (24 * (aom - floor(aom)));
    aom_m = (int) (1440 * (aom - floor(aom))) % 60;

    sprintf(tbuf, Format(12), jd /*JD + 0.5*/); /* Julian date */
    Set(SMI_JD);

    jyear(jd, &yy, &mm, &dd);
    jhms(jd, &hh, &mmm, &ss);

    if (!realtime) {  /* Ignore local time in test mode */
        strcpy(tbuf, "");
    } else {
		set_tm_time(&gm, TRUE);

        /* Local time */

        wsprintf((LPSTR) tbuf, (LPSTR) Format(14),
                gm.tm_hour, gm.tm_min, gm.tm_sec,
                gm.tm_mday,
                (LPSTR) rstring(IDS_MONTH_NAMES + gm.tm_mon), gm.tm_year + 1900);

        /* Dirty trick: jam the seconds from the local time snapshot into the
           value computed from the GMT to avoid "time parallax" in the information
           panel if the second happens to change between the time the GMT was computed
           and our time request above. */

        ss = gm.tm_sec;
    }
    Set(SMI_LT);
    ShowWindow(GetDlgItem(sunMoonPanel, SMI_LTL), realtime ? SW_SHOW : SW_HIDE);

    wsprintf((LPSTR) tbuf, (LPSTR) Format(13),
            hh, mmm, ss, dd,
            (LPSTR) rstring(IDS_MONTH_NAMES + (mm - 1)), yy);
    Set(SMI_UT);

    /* Information about the Moon */

    /* Age of moon */

    wsprintf((LPSTR) tbuf, (LPSTR) Format(15),
            Plural(aom_d, IDS_DAY), Plural(aom_h, IDS_HOUR),
            Plural(aom_m, IDS_MINUTE));
    Set(SMI_AOM);

    /* Moon phase */

    wsprintf((LPSTR) tbuf, (LPSTR) Format(16),
            (int) (cphase * 100));
    Set(SMI_PHASE);

    /* Moon distance */

    sprintf(tbuf, Format(17),
            (long) cdist, cdist / EarthRad);
    Set(SMI_MDIST);

    /* Moon subtends */

    sprintf(tbuf, Format(18), cangdia);
    Set(SMI_MSUBT);

    /* Information about the Sun */

    /* Sun's distance */

    sprintf(tbuf, Format(19),
            csund, csund / SunSMAX);
    Set(SMI_SDIST);

    /* Sun subtends */

    sprintf(tbuf, Format(20), csuang);
    Set(SMI_SSUBT);

    /* Calculate times of phases of this lunation.  This is
       sufficiently time-consuming that we only do it once a month. */

    if (repaint || (jd > nptime)) {
        phasehunt(jd + 0.5, phasar);
        lunation = (int) floor(((phasar[0] + 7) - LunatBase) / SynMonth) + 1;

        for (i = 0; i < 5; i++) {
            fmt_phase_time(phasar[i], tbuf);
            Set(SMI_PH1 + i);;
        }
        nptime = phasar[4];

        /* Edit lunation numbers into cells reserved for them. */

        wsprintf((LPSTR) tbuf, (LPSTR) Format(21), lunation);
        Set(SMI_LU1);
        wsprintf((LPSTR) tbuf, (LPSTR) Format(21), lunation + 1);
        Set(SMI_LU2);
    }

    mimage = GetDlgItem(sunMoonPanel, SMI_MOON);
    hDC = GetDC(mimage);
    iconmoon(hDC, "colourmoon", 64, 32, 27, 28,
             1, 1, p, mm, dd);
    ReleaseDC(mimage, hDC);
}

DialogueProc(SunMoonDlg)
{
    switch (message) {
        case WM_INITDIALOG:
            sunMoonPanel = hDlg;
            return TRUE;
            
        case WM_PAINT:
            upd_sun_moon(faketime, runmode && !animate, TRUE);
            break;

        case WM_CLOSE:
            DestroyWindow(hDlg);
            return TRUE;

		case WM_DESTROY:
            sunMoonPanel = NULL;
            return TRUE;

        case WM_COMMAND:
            switch (WM_COMMAND_ID(wParam)) {

                case IDOK:
                    SendMessage(hDlg, WM_CLOSE, 0, 0L);
                    return TRUE;

                case SMI_HELP:
                    WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                                ((DWORD) ((LPSTR) rstring(IDS_HELP_ON_SUNMOON))));
                    holped = TRUE;
                    return TRUE;


                default:
                    break;
            }
            break;

        default:
            break;

    }
    return FALSE;
}
#endif

/*  UPDMOON  --  Update display of the sublunar position on the Earth.  */

void updmoon(HWND hWnd, HDC hDC, RECT windex, double jd)
{
    static double p;
    double moonra, moondec, moonlong, moonlat, moonrv, gt,
           aom, cphase, cdist, cangdia, csund, csuang;

    if (showmoon) {
        if (NeedToCalculate(moonphase, jd)) {
            undrawmoon(hDC);
            InvalidateCalculation(moonpos);
            p = phase(jd, &cphase, &aom, &cdist, &cangdia, &csund, &csuang);
//{ char s[80]; sprintf(s, "P = %.12g  cphase = %.2f  aom = %.2f  Jd = %.12g\r\n", p, cphase, aom, jd); OutputDebugString(s); }            
            iconmoon(hDC, "moonicon", 32, /* X ctr */ 16, /* X radius */ 14,
                                          /* Y centre */ 14, 0, 0, p, 0, 0);
        }
        if (NeedToCalculate(moonpos, jd)) {
            int nmoonx = moonx, nmoony = moony;
            highmoon(jd, &moonra, &moondec, &moonrv);
            ecliptoeq(jd, moonra, moondec, &moonlong, &moonlat);
            gt = gmst(jd);
            moonlong = fixangle(180.0 + (moonlong - (gt * 15)));
            nmoony = windex.top + ((int) (ydots - (moonlat + 90) * (ydots / 180.0))) - 13;
            nmoonx = windex.left + ((int) ((moonlong) * (xdots / 360.0))) - 16;
            if (moonx != nmoonx || moony != nmoony) {
                undrawmoon(hDC);
                moonx = nmoonx;
                moony = nmoony;
            }
        }
        if (!moonDisplayed) {
            drawmoon(hDC);
        }
     }
}

