//#define SaverAnim
#define PALIDX
/*

                          Sunclock for Windows

                    Astronomical Calculation Routines

*/

#include "sunclock.h"

#define TERMINC  100                  /* Circle segments for terminator */

#define PROJINT  (60 * 10)            /* Frequency of seasonal recalculation
                                         in seconds. */

int runmode = TRUE;                   /* Update time if true */
int animate = FALSE;                  /* Animate simulated time display ? */
int displaymode = 0;				  /* Display mode:  0 = Auto-sense
													    1 = Monochrome
													   16 = Colour vector
													  255 = Grey scale bitmap
													  256 = Colour bitmap */
int imageBitmap;					  // Index of chosen image bitmap
int bitmapNum;						  // Number of map bitmaps available
double faketime = 0.0;                /* Time increment for test mode */
int lincr = 0;                 		  /* Animation mode time increment */
int idir = 1;                  		  /* Simulated time increment/decrement */
int anirate = 1;					  /* Animation rate */
int tracking = FALSE;				  /* Tracking satellite ? */
int satx, saty;			  			  /* Satellite co-ordinates */
int showmoon = TRUE;				  /* Show Moon on open window map ? */
int cuckooClock = TRUE;				  /* Chirp the hours and half hours ? */
HANDLE earthImageDIB = NULL;		  /* Handle to master image DIB */

static HBITMAP blitMap = NULL;		  /* Bitmap for window repair */
int ImageWid;				  		  /* Width of master image */
int ImageHgt;						  /* Height of master image */
int xdots, ydots;			  		  /* Window size */
static int chunks = 1;		      // Number of vertical chunks in which to paint bitmap
static int onoon = -1;                /* Previous pixel longitude of noon */
static int sronoon = -1;			  // Screen resolution pixel longitude of noon
static int renoon = TRUE;             /* Recompute noon ? */
static short *wtab = NULL, *wtab1 = NULL, *wtabs;
static int wtabsize;                  /* Width table size */
static struct timeb tbtp;

static long cctime;                   /* Time to display (real or simulated) */
static long lctime = 0;               /* Last full calculation time */

double siteLat = 51.5;		 		  // Observing site latitude
double siteLon = 0;		 		  	  // Observing site longitude
char siteName[128] = "Greenwich";	  // Observing site name

HPALETTE imagePal = NULL;			  /* Custom colour palette */

static struct {
	WORD	palVersion;
	WORD	palNumEntries;
	PALETTEENTRY palPalEntry[256];
} logpal;

#ifdef ScreenSaver
extern int textHeight;				  // System font height
#else
struct satellite sat = {""};          /* Current satellite arguments */
static int satdisp = FALSE;           /* True if satellite to be shown */
#ifdef TRAILS
static int needtrail = TRUE;          /* Need satellite orbit trail ? */
#endif
static int tickoff = FALSE;           /* Satellite blinker */
#endif

static char last_icon_tag[20];        /* Old icon label */

static char dateSep[2], timeSep[2], amPM[2][5];
static int dateFormat, timeFormat;

TIME_ZONE_INFORMATION tzInfo;			// Time zone information
char *tzName[2] = { "", "" };			// Time zone names

#define DispMode (abs(displaymode))
#define ImageMode ((earthImageDIB != NULL) && (DispMode >= 255))

/*  Forward functions.  */

static void updimage(HWND hWnd, RECT cRect, HDC hDC, int istimer, int copyCat);

//  SET_TM_TIME  --  Set time from Windows system or local time

void set_tm_time(struct tm *t, BOOL islocal)
{
#define CtF(tf, sf) t->tf = s.sf

	SYSTEMTIME s;

	if (islocal) {
		GetLocalTime(&s);
	} else {
		GetSystemTime(&s);
	}
	CtF(tm_sec, wSecond);
	CtF(tm_min, wMinute);
	CtF(tm_hour, wHour);
	CtF(tm_mday, wDay);
	CtF(tm_mon, wMonth - 1);
	CtF(tm_year, wYear - 1900);
	CtF(tm_wday, wDayOfWeek);
	//  tm_yday  never used
	t->tm_isdst = GetTimeZoneInformation(&tzInfo) == TIME_ZONE_ID_DAYLIGHT;
}

/*  LOCALTIMEFORMAT  --  Retrieve localised date and time format from profile.  */

void localtimeformat(void)
{
	static char context[] = "intl";

	dateFormat = GetProfileInt(context, "iDate", 0);
	timeFormat = GetProfileInt(context, "iTime", 0);
	GetProfileString(context, "sDate", "/", dateSep, 2);
	GetProfileString(context, "sTime", ":", timeSep, 2);
	GetProfileString(context, "s1159", "AM", amPM[0], 5);
	GetProfileString(context, "s2359", "PM", amPM[1], 5);
}

/*  EDTIME  --  Edit time in localised format.  */

static void edtime(char *str, int hh, int mm, int ss)
{
	if (timeFormat == 1) {
		sprintf(str, Format(0), hh, timeSep, mm, timeSep, ss);
	} else {
		sprintf(str, Format(1), (hh % 12) ? (hh % 12) : 12,
			timeSep, mm, timeSep, ss, amPM[hh / 12]);
	}
}

/*  EDDATE  --  Edit date in localised format.  */

static void eddate(char *str, long yy, int mm, int dd, int wday)
{
	sprintf(str, Format(3), rstring(IDS_DAY_NAMES + wday),
		dateFormat == 1 ? (long) dd : (dateFormat == 2 ? yy : (long) mm),
		dateSep,
		dateFormat == 1 ? mm : (dateFormat == 2 ? mm : dd),
		dateSep,
		dateFormat == 1 ? yy : (dateFormat == 2 ? (long) dd : yy));
}

/*  EDATIME  --  Edit abbreviated time in localised format.  */

static void edatime(char *str, int hh, int mm, int ss)
{
	if (timeFormat == 1) {
		sprintf(str, Format(9), hh, timeSep, mm, timeSep, ss);
	} else {
		sprintf(str, Format(10), (hh % 12) ? (hh % 12) : 12,
			timeSep, mm, amPM[hh / 12]);
	}
}

/*  EDADATE  --  Edit abbreviated date in localised format.  */

static void edadate(char *str, int mm, int dd)
{
	sprintf(str, Format(11),
		dateFormat == 1 ? dd : mm,
		dateSep,
		dateFormat == 1 ? mm : dd);
}

/*  RSTRING  --  Retrieve a string from the resource file.  */

char *rstring(int resid)
{
#define maxCStrings 10              /* Maximum concurrently used strings */
    static char rstrings[maxCStrings][80];
    static int n = 0;
    int m = n;

    if (LoadString(hInst, resid, rstrings[m], 79) < 0) {
        strcpy(rstrings[m], "");
    }
    n = (n + 1) % maxCStrings;
    return rstrings[m];
}

/*  INVALIDATE_REFRESH  --  Invalidate the screen refresh bitmap kept
							to expedite damaged window repair.  */

void invalidate_refresh(void)
{
	if (blitMap != NULL) {
		DeleteObject(blitMap);
		blitMap = NULL;
	}
}

/*  RELEASE_BITMAPS  --  Release retained objects for display generation.  */

void release_bitmaps(void)
{
	invalidate_refresh();
	if (imagePal != NULL) {
		DeleteObject(imagePal);
		imagePal = NULL;
	}
	if (earthImageDIB != NULL) {
		FreeResource(earthImageDIB);
		earthImageDIB = NULL;
	}
}

#ifdef PALIDX

/*	PALMAPSTART  --  Create a replacement palette bitmap header.  */

static HGLOBAL pmBuf = NULL;

LPBITMAPINFOHEADER palMapStart(LPBITMAPINFOHEADER bh, int *bmode)
{
	WORD i, ncol, offbits;
	LPBITMAPINFOHEADER bm = NULL;
	unsigned short FAR *palidx;
    
    assert(pmBuf == NULL);
	ncol = (WORD) (bh->biClrUsed == 0 ? (1L << bh->biBitCount) : bh->biClrUsed);
	offbits = (WORD) (bh->biSize + ncol * sizeof(RGBQUAD));
	pmBuf = GlobalAlloc(GMEM_MOVEABLE, (DWORD) offbits);
	if (pmBuf != NULL) {
		bm = GlobalLock(pmBuf);
	}
	if (bm != NULL) {
		*bmode = DIB_PAL_COLORS;
		memcpy(bm, bh, offbits);
		bm->biClrUsed = bm->biClrImportant = 246;
		palidx = (unsigned short FAR *) ((LPBITMAPINFO) bm)->bmiColors; 
		for (i = 0; i < bm->biClrUsed; i++) {
			*palidx++ = (unsigned short) i;
		}
		return bm;
	}
	*bmode = DIB_RGB_COLORS;
	return bh;	
}

/*	PALMAPEND  --  Release palette map buffer, if any.  */

void palMapEnd(void)
{
	if (pmBuf != NULL) {
		GlobalUnlock(pmBuf);
		GlobalFree(pmBuf);
		pmBuf = NULL;
	}
} 
#endif

/*  NEEDIMAGEBITMAP  --  Make sure image bitmap and palette are loaded.  */

static void needImageBitmap(void)
{
	int i;
	LPBITMAPINFOHEADER bh;

	if (earthImageDIB == NULL) {
#ifdef ScreenSaver
		earthImageDIB = LoadResource(hInst,
			FindResource(hInst, "earth", RT_BITMAP));
#else
		if (imageBitmap >= bitmapNum) {
			imageBitmap = 0;
		}
		mapbitsGetBitMap(imageBitmap, &earthImageDIB); 
#endif			
	}

	bh = (LPBITMAPINFOHEADER) LockResource(earthImageDIB);
	ImageWid = (int) bh->biWidth;
	ImageHgt = (int) bh->biHeight;
    
    if (imagePal == NULL) {
	
		/* Extract the day and night colour palettes from the DIB file and
		   construct a memory logical palette embodying those colours.  Then
		   register the palette with the system to provide those colours on
		   the display board.  Note that if the user wants a grey scale map
		   window, we transform the colours in the selected bitmap's
		   palette into grey scale here. */
	
		logpal.palVersion = 0x300;
#ifdef PALIDX
		logpal.palNumEntries = 246;
#else	
		logpal.palNumEntries = 117 * 2;
#endif	
		for (i = 0; i < 117; i++) {
			RGBQUAD q = *((LPRGBQUAD) &((LPBITMAPINFO) bh)->bmiColors[i]);

			if (DispMode == 255) {
				double fgs;
	
				fgs = (q.rgbRed * 0.30) +
					  (q.rgbGreen * 0.59) +
					  (q.rgbBlue * 0.11);
				if (fgs > 255.0) {
					fgs = 255;
				}
				logpal.palPalEntry[i].peRed =
					logpal.palPalEntry[i].peGreen =
					logpal.palPalEntry[i].peBlue = (BYTE) fgs;
			} else {	
				logpal.palPalEntry[i].peRed = q.rgbRed;
				logpal.palPalEntry[i].peGreen = q.rgbGreen;
				logpal.palPalEntry[i].peBlue = q.rgbBlue;
			}
			logpal.palPalEntry[i].peFlags = PC_NOCOLLAPSE;
		}
		for (i = 0; i < 117; i++) {
			RGBQUAD q = *((LPRGBQUAD) &((LPBITMAPINFO) bh)->bmiColors[i + 128]);
	
#ifdef PALIDX
			if (DispMode == 255) {
				double fgs;
	
				fgs = (q.rgbRed * 0.30) +
					  (q.rgbGreen * 0.59) +
					  (q.rgbBlue * 0.11);
				if (fgs > 255.0) {
					fgs = 255;
				}
				logpal.palPalEntry[128 + i].peRed =
					logpal.palPalEntry[128 + i].peGreen =
					logpal.palPalEntry[128 + i].peBlue = (BYTE) fgs;
			} else {	
				logpal.palPalEntry[128 + i].peRed = q.rgbRed;
				logpal.palPalEntry[128 + i].peGreen = q.rgbGreen;
				logpal.palPalEntry[128 + i].peBlue = q.rgbBlue;
			}
			logpal.palPalEntry[128 + i].peFlags = PC_NOCOLLAPSE;
#else
			if (DispMode == 255) {
				double fgs;
	
				fgs = (q.rgbRed * 0.30) +
					  (q.rgbGreen * 0.59) +
					  (q.rgbBlue * 0.11);
				if (fgs > 255.0) {
					fgs = 255;
				}
				logpal.palPalEntry[117 + i].peRed =
					logpal.palPalEntry[117 + i].peGreen =
					logpal.palPalEntry[117 + i].peBlue = (BYTE) fgs;
			} else {	
				logpal.palPalEntry[117 + i].peRed = q.rgbRed;
				logpal.palPalEntry[117 + i].peGreen = q.rgbGreen;
				logpal.palPalEntry[117 + i].peBlue = q.rgbBlue;
			}
			logpal.palPalEntry[117 + i].peFlags = PC_NOCOLLAPSE;
#endif		
		}
	
		imagePal = CreatePalette((LPLOGPALETTE) &logpal);
	}
	UnlockResource(earthImageDIB);
}

/*  NEW_DISPLAY_MODE  --  Change display mode on the fly.  */

void new_display_mode(HWND hWnd, int mode)
{
	HDC hDC;

	release_bitmaps();
	displaymode = mode;
    hDC = GetDC(hWnd);
    ringgg(hWnd, hDC, TRUE, FALSE);
    ReleaseDC(hWnd, hDC);
#ifndef ScreenSaver
    satviewchanged();
#endif
}

/*  GO_ICONIC  --  Initialise when window is minimised to an icon. */

void go_iconic(HWND hWnd)
{
	RECT r;

	GetClientRect(hWnd, &r);
	SetWindowPos(hWnd, NULL, 0, 0, 2 * r.right, r.bottom, SWP_NOMOVE + SWP_NOZORDER);
    strcpy(last_icon_tag, "");              /* Force icon tag redisplay */
    SetWindowText(hWnd, last_icon_tag);		/* Initially clear icon title */
    release_bitmaps();						/* Prepare for display in icon */
}

/*  START_DRAWING  --  Set up to begin painting in client area.  */

static HPALETTE sdHpal;

static void start_drawing(HDC hDC)
{
#ifndef ScreenSaver
	if (imagePal != NULL) {
		sdHpal = SelectPalette(hDC, imagePal, FALSE);
		RealizePalette(hDC);
	}
	undrawsat(hWndMain);
	undrawmoon(hDC);
#endif
}

/*  END_DRAWING  --  End painting in client area.  */

static void end_drawing(HDC hDC) {
#ifndef ScreenSaver
	drawmoon(hDC);
	drawsat(hWndMain);
	if (imagePal != NULL) {
		SelectPalette(hDC, sdHpal, FALSE);
		RealizePalette(hDC);
	}
#endif
}

#ifdef ScreenSaver

/*  SSAVETIME  --  Edit and time for screen saver display.  */

static void ssavetime(HDC hDC, struct tm lt, RECT tr)
{
	if (!fChildPreview) {
		char tbuf[80];

		strcpy(tbuf, "   ");
		edtime(tbuf + strlen(tbuf), lt.tm_hour, lt.tm_min, lt.tm_sec);
		strcat(tbuf, " ");
		strcat(tbuf, tzName[!!lt.tm_isdst]);
		strcat(tbuf, " ");
		eddate(tbuf + strlen(tbuf), (long) (lt.tm_year + 1900), lt.tm_mon + 1, lt.tm_mday, lt.tm_wday);
		strcat(tbuf, "   ");
		tr.bottom = tr.top;
		tr.top = tr.top - textHeight;
		SetBkMode(hDC, OPAQUE);
		SetBkColor(hDC, RGB(0, 0, 0));
		SetTextColor(hDC, RGB(0, 255, 0));
		DrawText(hDC, tbuf, strlen(tbuf), &tr, DT_CENTER | DT_BOTTOM | DT_NOCLIP | DT_SINGLELINE);
	}
}
#endif

/*  RANDINT  --  Return a pseudorandom integer value between vmin and vmax
                 inclusive.  */

static int randInt(int vmin, int vmax)
{
	int range = vmax - vmin;
	int ptwo = 1;
	int v;

	while (range >= ptwo) {
		ptwo <<= 1;
	}
	ptwo--;

	do {
		v = rand() & ptwo;
	} while (v > range);

	return v + vmin;
}

/*  RINGGG  --  Update status on interval timer ticks and redraw
                window if needed.  */

static HDC hhdc;                            /* HDC hidden argument to drawvec */
static int lx, ly;							/* Last X and Y for optimisation */
static int yieldYet;						// Time to yield to other processes ?

#ifdef POLLY
extern POINT pLine[POLLY];					// Share vector list with SATVIEW.C
extern int pLinel;
#endif

#pragma check_stack (off)
static void PASCAL drawvec(int x1, int y1, int x2, int y2)
{
#ifdef POLLY
	if (pLinel >= POLLY || (x1 != lx || y1 != ly)) {
		if (pLinel > 0) {
			Polyline(hhdc, pLine, pLinel);
		}
		pLine[0].x = x1;
		pLine[0].y = y1;
		pLine[1].x = x2;
		pLine[1].y = y2;
		pLinel = 2;
	} else {
		pLine[pLinel].x = x2;
		pLine[pLinel++].y = y2;
	}
	lx = x2;
	ly = y2;
#else
	if (x1 != lx || y1 != ly) {
		lx = x1;
		ly = y1;
		MoveTo(hhdc, x1, y1);
	}
	if ((++yieldYet) > 1000) {
		MSG dmsg;

		PeekMessage(&dmsg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE);
		yieldYet = 0;
	}
	LineTo(hhdc, x2, y2);
#endif
}
#pragma check_stack ()

void ringgg(HWND hWnd, HDC hDC, int repaint, int copyCat)
{
	HINSTANCE hi;
    static RECT windex;

    hi = hInst;
#ifdef ScreenSaver
	{
		struct tm lt;
		int swid, sheight, xoff, yoff, dualscreen;
		static int lmin = -1;

		set_tm_time(&lt, TRUE);

#ifndef SaverAnim
		if (lt.tm_min == lmin) {
			ssavetime(hDC, lt, windex);
			return;
		}
#endif
		lmin = lt.tm_min;

	    GetClientRect(hWnd, &windex);
	    
	    /*	The following hack attempts to deduce whether we're running on a
			dual screen by looking at the aspect ratio of the screen dimensions
			we obtained from GetClientRect.  If the screen is more than twice
			as wide as it is high, we conclude it's a dual screen system and
			reduce the effective width by a half.  This keeps us from blowing
			up the screen saver image so large that it is highly probable to
			span the crack between the two screens.  */
	    dualscreen = windex.right > (windex.bottom * 2);
	    swid = dualscreen ? (windex.right / 4) : (windex.right / 2);
	    
		swid = min(swid, windex.bottom - (fChildPreview ? 0 : textHeight));
	    sheight = (swid / 2) + (fChildPreview ? 0 : textHeight);

		/* Determine the placement of the image on the screen.  If we have
		   guessed that this is a dual screen configuration, take care so
		   the image does not span the two screens (assuming they're the same
		   width. */

		xoff = randInt(0, (dualscreen ? (windex.right / 2) : windex.right) - (swid + 1));
		if (dualscreen && ((rand() & 0x400) != 0)) {
			xoff += windex.right / 2;
		}
		yoff = randInt(0, windex.bottom - (sheight + 1)) + (fChildPreview ? 0 : textHeight);

		windex.left += xoff;
		windex.right = windex.left + swid;
		windex.top += yoff;
		windex.bottom = windex.top + (swid / 2);
	}
#else
	GetClientRect(hWnd, &windex);
#endif

    if (repaint) {
        HDC hdcMem;
        HBITMAP hBitmap;

        InvalidateCalculation(moonpos);
        moonDisplayed = FALSE;
        satDisplayed = FALSE;

        if (blitMap) {
        	HANDLE obmap;

        	hdcMem = CreateCompatibleDC(hDC);
        	obmap = SelectObject(hdcMem, blitMap);
#ifdef ScreenSaver
			{	RECT wi;
				GetClientRect(hWnd, &wi);
				FillRect(hDC, &wi, (HBRUSH) GetStockObject(BLACK_BRUSH));
			}
#endif
	        BitBlt(hDC, windex.left, windex.top,
	        			windex.right - windex.left, windex.bottom - windex.top,
	                    hdcMem, 0, 0, SRCCOPY);
	        SelectObject(hdcMem, obmap);
            DeleteDC(hdcMem);
        } else {
			if (DispMode >= 255) {
				needImageBitmap();
			}
// #define CHECKREGISTRATION
#ifndef CHECKREGISTRATION
	           else
#endif
	        {
	        	{
	        		HPEN dpen, oldPen;
	        		HBRUSH dbrush;
	        		RECT screct;
	        		HBITMAP obmap;

	        		/* The bitmap image of the Earth cannot be displayed or isn't desired.
	        	  	   Generate a a vector image by scaling our master vector database
	        	   	   to a bitmap of the proper size. */

					invalidate_refresh();
		        	hBitmap = CreateCompatibleBitmap(hDC, windex.right - windex.left,
		        										  windex.bottom - windex.top);
	        		hdcMem = CreateCompatibleDC(hDC);
	            	obmap = SelectObject(hdcMem, hBitmap);
	            	blitMap = hBitmap;
	        		dpen = CreatePen(PS_SOLID, 1, DispMode == 1 ? GetSysColor(COLOR_WINDOW) :
	        						RGB(255, 255, 0));
	        		dbrush = CreateSolidBrush(DispMode == 1 ? GetSysColor(COLOR_WINDOWTEXT) : RGB(0, 0, 255));
	        		GetClientRect(hWnd, &screct);
#ifndef CHECKREGISTRATION
	        		FillRect(hdcMem, &screct, dbrush);
#endif
	        		DeleteObject(dbrush);
	        		oldPen = SelectObject(hdcMem, dpen);
	        		yieldYet = 0;
	    			hhdc = hdcMem;
	        		lx = ly = -1;						/* Reset optimisation */
#ifdef POLLY
					pLinel = 0;
#endif
	    			mapdraw(windex.right - windex.left, windex.bottom - windex.top, drawvec);
#ifdef POLLY
					if (pLinel > 0) {
						Polyline(hdcMem, pLine, pLinel);
					}
#endif
	    			SelectObject(hdcMem, oldPen);
	    			DeleteObject(dpen);
#ifdef ScreenSaver
					{	RECT wi;
	    				GetClientRect(hWnd, &wi);
        				FillRect(hDC, &wi, (HBRUSH) GetStockObject(BLACK_BRUSH));
        			}
#endif
	            	BitBlt(hDC, windex.left, windex.top, windex.right - windex.left,
	            			windex.bottom - windex.top, hdcMem, 0, 0, SRCCOPY);
	            	SelectObject(hdcMem, obmap);
	            	DeleteDC(hdcMem);
	    		}
	        }
#ifdef CHECKREGISTRATION
			{   time_t t;
				t = time(NULL);
				while ((time(NULL) - t) < 60) Yield();
			}
#endif
	    }
	    updimage(hWnd, windex, hDC, FALSE, copyCat);
    } else {
    	updimage(hWnd, windex, hDC, TRUE, copyCat);
    }
#ifdef ScreenSaver
	moonDisplayed = FALSE;
	if (showmoon) {
		updmoon(hWnd, hDC, windex, faketime);
	}
	/* Show and hide cursor to get around bug in Windows 3.1 that makes cursor
	   reappear after a StretchBlt. */
	ShowCursor(TRUE);
	ShowCursor(FALSE);
#else
    if (viewFromFrustrated) {
    	satviewchanged();
    }
#endif
}

/*  PROJILLUM  --  Project illuminated area on the map.  */

static void projillum(short *wtab, int xdots, int ydots, double dec)
{
    int i, ftf = TRUE, ilon, ilat, lilon, lilat, xt;
    double m, x, y, z, th, lon, lat, s, c;

    /* Clear unoccupied cells in width table */

    for (i = 0; i < ydots; i++) {
        wtab[i] = -1;
    }

    /* Build transformation for declination */

    s = sin(-dtr(dec));
    c = cos(-dtr(dec));

    /* Increment over a semicircle of illumination */

    for (th = -(PI / 2); th <= PI / 2 + 0.001; th += PI / TERMINC) {

        /* Transform the point through the declination rotation. */

        x = -s * sin(th);
        y = cos(th);
        z = c * sin(th);

        /* Transform the resulting co-ordinate through the
           map projection to obtain screen co-ordinates. */

        lon = (y == 0 && x == 0) ? 0.0 : rtd(atan2(y, x));
        lat = rtd(asin(z));

        ilat = (int) (ydots - (lat + 90) * (ydots / 180.0));
        ilon = (int) (lon * (xdots / 360.0));

        if (ftf) {

            /* First time.  Just save start co-ordinate. */

            lilon = ilon;
            lilat = ilat;
            ftf = FALSE;
        } else {

            /* Trace out the line and set the width table. */

            if (lilat == ilat) {
                wtab[(ydots - 1) - ilat] = 2 * (ilon == 0 ? 1 : ilon);
            } else {
                m = ((double) (ilon - lilon)) / (ilat - lilat);
                for (i = lilat; i != ilat; i += sgn(ilat - lilat)) {
                    xt = (int) (lilon + floor((m * (i - lilat)) + 0.5));
                    wtab[(ydots - 1) - i] = 2 * (xt == 0 ? 1 : xt);
                }
            }
            lilon = ilon;
            lilat = ilat;
        }
    }

    /* Now tweak the widths to generate full illumination for
       the correct pole. */

    if (dec < 0.0) {
        ilat = ydots - 1;
        lilat = -1;
    } else {
        ilat = 0;
        lilat = 1;
    }

    for (i = ilat; i != ydots / 2; i += lilat) {
        if (wtab[i] != -1) {
            while (TRUE) {
                wtab[i] = xdots;
                if (i == ilat) {
                    break;
                }
                i -= lilat;
            }
            break;
        }
    }
}

/*  XSPAN  --  Complement a span of pixels.  Called with line in which
               pixels are contained, leftmost pixel in the  line,  and
               the   number   of   pixels   to   complement.   Handles
               wrap-around at the right edge of the screen.  */

static void xspan(HDC hDC, RECT windex, int pline, int leftp, int npix)
{
    leftp = leftp % xdots;
    pline += windex.top;
    if ((leftp + npix) > xdots) {
		BitBlt(hDC, windex.left + leftp, pline, (xdots - leftp) - 1, 1, NULL, 0, 0, DSTINVERT);
		BitBlt(hDC, windex.left, pline, ((leftp + npix) - xdots), 1, NULL, 0, 0, DSTINVERT);
    } else {
		BitBlt(hDC, windex.left + leftp, pline, npix, 1, NULL, 0, 0, DSTINVERT);
    }
}

/*  MOVETERM  --  Update illuminated portion of the globe.  */

static void moveterm(HDC hDC, RECT windex,
					 void (*cspan)(HDC hWnd, RECT windex, int pline, int leftp, int npix),
					 short *wtab, int noon,
					 short *otab, int onoon,
					 int xdots, int ydots,
					 int copyCat)
{
    int i, j, ol, oh, nl, nh;

    if (ImageMode) {
    	HPALETTE opal;
		LPBITMAPINFOHEADER bh, bp;
		WORD ncol, offbits;
		int wid, hgt, bmode = DIB_RGB_COLORS;
		unsigned char *pa;

		bp = bh = (LPBITMAPINFOHEADER) LockResource(earthImageDIB);
		ncol = (WORD) (bh->biClrUsed == 0 ? (1L << bh->biBitCount) : bh->biClrUsed);
		offbits = (WORD) (bh->biSize + ncol * sizeof(RGBQUAD));

		wid = ImageWid;
		hgt = ImageHgt;

#define PixA(x, y) ((((unsigned char *) (bh)) + offbits + ((x) + (((DWORD) ((ImageHgt - 1) - (y))) * (((wid + (sizeof(LONG) - 1)) & (~(sizeof(LONG) - 1))))))))
#ifndef WINKY
    	for (i = 0; i < (int) hgt; i++) {
    		if (wtab[i] < 0) {
    			pa = PixA(0, i);
     			for (j = 0; j < wid; j++) {
					*pa++ &= 0x7F;
     			}
    		} else {

                /* If both the old and new spans were the entire
                   screen, they're equivalent. */

           		if ((otab[i] == wtab[i]) && (wtab[i] == wid)) {
                    continue;
               	}

                pa = PixA(0, i);
				for (j = 0; j < wid; j++) {
					*pa++ &= 0x7F;
				}

                nl =  ((noon - (wtab[i] / 2)) + wid) % wid;
                nh = (nl + wtab[i]) - 1;

                oh = (nh - nl) + 1;
    			if ((nl + oh) > wid) {
    				pa = PixA(nl, i);
					for (j = nl; j < wid; j++) {
						*pa++ |= 0x80;;
					}
					pa = PixA(0, i);
					for (j = 0; j < (((nl + oh) - wid) + 1); j++) {
						*pa++ |= 0x80;
					}
    			} else {
    				pa = PixA(nl, i);
					for (j = nl; j < ((nl + oh) + 1); j++) {
						*pa++ |= 0x80;
					}
    			}
    		}
    		otab[i] = wtab[i];
    	}
#endif // WINKY    	
#ifdef ScreenSaver
		{	RECT wi;
			GetClientRect(hMainWindow, &wi);
			FillRect(hDC, &wi, (HBRUSH) GetStockObject(BLACK_BRUSH));
		}
#endif
    	if (imagePal != NULL) {
    		opal = SelectPalette(hDC, imagePal, FALSE);
    		RealizePalette(hDC);
    	}
#ifdef PALIDX
		bp = palMapStart(bh, &bmode);
#endif    	
		if (!copyCat && (xdots - 1) == ImageWid && (ydots - 1) == ImageHgt) {
			SetDIBitsToDevice(hDC, windex.left, windex.top, xdots - 1, ydots - 1,
	        		0, 0, 0, ImageHgt,
	        		((LPSTR) bh) + offbits, (LPBITMAPINFO) bp,
					bmode);
		} else {
#ifdef OLDWAY
			int l;

			SetStretchBltMode(hDC, COLORONCOLOR);
			l = StretchDIBits(hDC, windex.left, windex.top, xdots, ydots,
	        					0, 0, ImageWid, ImageHgt,
	        					((LPSTR) bh) + offbits, (LPBITMAPINFO) bp,
								bmode, SRCCOPY);
			if (l == GDI_ERROR) {
			    StretchDIBits(hDC, windex.left, windex.top, xdots / 2, ydots,
	        					0, 0, ImageWid / 2, ImageHgt,
	        					((LPSTR) bh) + offbits, (LPBITMAPINFO) bp,
								bmode, SRCCOPY);
			    StretchDIBits(hDC, windex.left + xdots / 2, windex.top, xdots / 2, ydots,
	        					ImageWid / 2, 0, ImageWid / 2, ImageHgt,
	        					((LPSTR) bh) + offbits, (LPBITMAPINFO) bp,
								bmode, SRCCOPY);
			}
#else
    
			/*  Hey hey kids!  It's Crufty time again!  Even on a machine with
			    384 megabytes of RAM, pointy-headed Windows 98 can't
			    stretch a large (1400 by 700 pixel) bitmap into a
			    similarly large full-colour screen area.  So, we attempt
			    the stretch in one swell foop, and if it doesn't work, then
			    we try chopping the image into halves, quarters, eights,
			    etc. in the hope we'll be able to paint it in smaller
			    stripes.  */

			while (TRUE) {
			    int n, stat, swid, dwid, soff, doff;

			    SetStretchBltMode(hDC, COLORONCOLOR);
			    soff = doff = 0;
			    dwid = xdots / chunks;
			    swid = ImageWid / chunks;

			    for (n = 0; n < chunks; n++) {
				stat = StretchDIBits(hDC, windex.left + doff, windex.top, dwid, ydots,
	        					    soff, 0, swid, ImageHgt,
	        					    ((LPSTR) bh) + offbits, (LPBITMAPINFO) bp,
								    bmode, SRCCOPY);
				if (stat == GDI_ERROR) {
				    break;
				}
				soff += swid;
				doff += dwid;
			    }
			    if (stat != GDI_ERROR) {
				break;
			    }
			    if (((dwid / 2) < 1) || ((swid / 2) < 1)) {
				break;			// Bail out if the situation is hopeless
			    }
			    chunks *= 2;
			}
#endif
	    }
#ifdef PALIDX
		palMapEnd();
#endif    	
    	UnlockResource(earthImageDIB);
    	if (imagePal != NULL) {
    		SelectPalette(hDC, opal, FALSE);
    	}
    } else {

#ifndef WINKY
    	for (i = 0; i < (ydots - 1); i++) {

        	/* If line is off in new width table but is set in
           	the old table, clear it. */

        	if (wtab[i] < 0) {
            	if (otab[i] >= 0) {
                	(*cspan)(hDC, windex, i, ((onoon - (otab[i] / 2)) + xdots) % xdots,
                   	otab[i]);
            	}
        	} else {

            	/* Line is on in new width table.  If it was off in
               	the old width table, just draw it. */

            	if (otab[i] < 0) {
               	(*cspan)(hDC, windex, i, ((noon - (wtab[i] / 2)) + xdots) % xdots,
                  	wtab[i]);
            	} else {

                	/* If both the old and new spans were the entire
                   	   screen, they're equivalent. */

                	if ((otab[i] == wtab[i]) && (wtab[i] == xdots)) {
                    	continue;
                	}

                	/* The line was on in both the old and new width
                   	   tables.  We must adjust the difference in the
                   	   span.  */

                	ol =  ((onoon - (otab[i] / 2)) + xdots) % xdots;
                	oh = (ol + otab[i]) - 1;
                	nl =  ((noon - (wtab[i] / 2)) + xdots) % xdots;
                	nh = (nl + wtab[i]) - 1;

	                /* If spans are disjoint, erase old span and set
	                   new span. */

	                if (oh < nl || nh < ol) {
    	                (*cspan)(hDC, windex, i, ol, (oh - ol) + 1);
        	            (*cspan)(hDC, windex, i, nl, (nh - nl) + 1);
            	    } else {
	                    /* Clear portion(s) of old span that extend
	                       beyond end of new span. */
	                    if (ol < nl) {
    	                    (*cspan)(hDC, windex, i, ol, nl - ol);
        	                ol = nl;
            	        }
	                    if (oh > nh) {
	                        (*cspan)(hDC, windex, i, nh + 1, oh - nh);
                        	oh = nh;
                    	}
	                    /* Extend existing (possibly trimmed) span to
	                       correct new length. */
	                    if (nl < ol) {
	                        (*cspan)(hDC, windex, i, nl, ol - nl);
	                    }
	                    if (nh > oh) {
	                        (*cspan)(hDC, windex, i, oh + 1, nh - oh);
	                    }
	                }
    		    }
	        }
	        otab[i] = wtab[i];
    	}
#endif // WINKY    	
    }
}

#ifndef ScreenSaver

/*  UPDTIME  --  Update time based on current increment and mode.  */

static void updtime(void)
{
#define Day (24.0 * 60 * 60)
	int ydir;

    switch (lincr) {
        case 0:						/* Minute */
            faketime += (60 * idir) / Day;
            break;

        case 1:						/* Hour */
            faketime += (60 * 60 * idir) / Day;
            break;

        case 2:                   /* Day */
            faketime += idir;
            break;

        case 3:						/* Week */
            faketime += 7 * idir;
            break;

        case 4:						/* Month */
        	{
        		long yy;
    			int mm, dd, hh, mmm, ss;

            	jyear(faketime, &yy, &mm, &dd);
            	jhms(faketime, &hh, &mmm, &ss);
            	mm += idir;
            	if (mm < 1) {
            		mm = 12;
            		yy--;
            	} else if (mm > 12) {
            		mm = 1;
            		yy++;
            	}
            	faketime = ucttoj(yy, mm - 1, dd, hh, mmm, ss);
        	}
            break;

        case 5:						/* Year */
        	ydir = idir;
uyear:      {
				long yy;
    			int mm, dd, hh, mmm, ss;

            	jyear(faketime, &yy, &mm, &dd);
            	jhms(faketime, &hh, &mmm, &ss);
            	yy += ydir;
            	faketime = ucttoj(yy, mm - 1, dd, hh, mmm, ss);
        	}
            break;

        case 6:						/* Sidereal day */
            faketime += idir * 0.99726966;
            break;

        case 7:						/* Sidereal year */
            faketime += idir * 365.2564;
            break;

        case 8:						// Decade
        	ydir = idir * 10;
        	goto uyear;

        case 9:						// Century
        	ydir = idir * 100;
        	goto uyear;

        case 10:					// Millenium
        	ydir = idir * 1000;
        	goto uyear;
    }
}
#endif // ScreenSaver

/*  UPDIMAGE  --  Update current displayed image.  */

static void updimage(HWND hWnd, RECT cRect, HDC hDC, int istimer, int copyCat)
{
    int xl, srxl;
#ifndef ScreenSaver
    char tbuf[128];
#endif
    double jt, sunra, sundec, sunrv, sunlong, gt;
    struct tm ct, lt;
    static int lisec = 61;            /* Last iconic seconds */
    static double lfaketime = -1;	  /* Last fake time */

	xdots = (cRect.right - cRect.left) + 1;
	ydots = (cRect.bottom - cRect.top) + 1;

    /* If this is a full repaint of the window, force complete
       recalculation. */

    if (!istimer) {
    	int i;

    	/* Allocate (or reallocate, if already in existence) the projected
    	   illumination width tables. */

	    if (wtab != NULL) {
	        free(wtab);
	        free(wtab1);
	    }
	    wtabsize = ImageMode ? ImageHgt : ydots;
	    wtab = (short *) malloc((unsigned int) wtabsize * sizeof(short));
	    wtab1 = (short *) malloc((unsigned int) wtabsize * sizeof(short));
	    for (i = 0; i < wtabsize; i++) {
	        wtab[i] = wtab1[i] = -1;
	    }

        lctime = 0;
        onoon = -1;
        lisec = 61;
        lfaketime = -1;
    }

    if (animate) {
#ifdef ScreenSaver
		assert(FALSE);
#else
        updtime();
#endif
    }
	set_tm_time(&lt, TRUE);

#ifndef ScreenSaver

    /* Special  case  to  reduce overhead  whilst iconic: if we're
       only showing the icon, update the  display  only  once  per
       minute,  detected  by  the  fact  that  the current seconds
       reading is less than that of the  last  update.   The  icon
       shows  only  hours  and  minutes, and is sufficiently small
       that once-a-minute updates are plenty to keep  the  picture
       in sync.  Note that we aren't entitled to do this if the
	   Sky window exists and is not itself iconic. */

    if (IsIconic(hWnd) && runmode && (lt.tm_sec > lisec) &&
		((skyViewPanel == NULL) || IsIconic(skyViewPanel))) {
        return;
    }
#endif

	set_tm_time(&ct, FALSE);

    if (RealTime) {
    	jt = faketime = jtime(&ct);
    } else {
    	jt = faketime;
    }
#ifdef SaverAnim
	{
		static double yinker = 0.0;

		jt = faketime = faketime + yinker;
		yinker += 7.134;
	}
#endif

	/* On 32-bit Windows with the bottom bozo bar, there's no
	   small image when iconic.  Skip all the calculation and
	   image painting. */

	if (!IsIconic(hWnd)) {

		sunpos(jt, FALSE, &sunra, &sundec, &sunrv, &sunlong);
		gt = gmst(jt);

		/* Projecting the illumination curve  for the current seasonal
		   instant is costly.  If we're running in real time, only  do
		   it every PROJINT seconds.  */

		if ((!runmode && (lfaketime != faketime)) || animate || !istimer ||
			(cctime < lctime) || ((cctime - lctime) > PROJINT)) {
			projillum(wtab, ((!ImageMode) ? xdots : ImageWid),
        					((!ImageMode) ? ydots : ImageHgt), sundec);
			wtabs = wtab;
			wtab = wtab1;
			wtab1 = wtabs;
			lctime = cctime;
		}

		sunlong = fixangle(180.0 + (sunra - (gt * 15)));
		xl = (int) (sunlong * (((!ImageMode) ? xdots : ImageWid) / 360.0));
		srxl = (int) (sunlong * (xdots / 360.0));

#ifndef ScreenSaver

		/* If we're tracking a satellite, update its position. */

		if (tracking) {
			updsat(jt, FALSE, FALSE);
		}

		/* If the Sun and Moon information panel is displayed, update it. */

		if (sunMoonPanel != NULL) {
			upd_sun_moon(jt, RealTime, !istimer);
		}

		/* If the planet position panel is displayed, update it. */

		if (planetPanel != NULL) {
			updatePlanet(jt, RealTime);
		}

		/* Show the Moon on the map, if requested. */

		updmoon(hWnd, hDC, cRect, jt);

		/* Update the orrery, if needed. */

		updateOrrery(jt, FALSE);
#endif

		/* If the subsolar point has moved at least one pixel, update
		   the illuminated area on the screen.  */

		if ((!runmode && (lfaketime != faketime)) || animate ||
    		!istimer || renoon || (sronoon != srxl)) {
			start_drawing(hDC);
#ifndef SCREENCAP
			moveterm(hDC, cRect, xspan, wtab1, xl, wtab, onoon, xdots, ydots, copyCat);
#endif
			end_drawing(hDC);
			onoon = xl;
			sronoon = srxl;
			renoon = FALSE;
#ifndef ScreenSaver
			changesunview();
			/* The Moon doesn't move much faster than the Sun as seen from Earth.
			   Only update the view from the Moon when the subsolar position changes. */
			changemoonview();
#endif
		}
	}

#ifndef ScreenSaver

	/* Update the star map, if needed.  */

	if ((skyViewPanel != NULL) && !IsIconic(skyViewPanel)) {
		if (IsIconic(hWnd)) {
			updmoon(hWnd, hDC, cRect, jt);
		}
		updateSky(jt, FALSE);
	}
#endif

    lfaketime = faketime;

#ifdef ScreenSaver
	ssavetime(hDC, lt, cRect);
#else
    if (IsIconic(hWnd)) {

        /* Display time in icon label */

		char ldstr[24], ltstr[24];

        if (RealTime) {
			edadate(ldstr, lt.tm_mon + 1, lt.tm_mday);
			edatime(ltstr, lt.tm_hour, lt.tm_min, lt.tm_sec);
			V sprintf(tbuf, Format(7),
				ltstr, ldstr);
		} else {
			long yy;
    		int mm, dd, wday, hh, mmm, ss;

            jyear(faketime, &yy, &mm, &dd);
            jhms(faketime, &hh, &mmm, &ss);
            wday = ((int) fmod(faketime, 7)) + 1;
            if (wday == 7) {
            	wday = 0;
            }
			edadate(ldstr, mm, dd);
			edatime(ltstr, hh, mmm, ss);
            sprintf(tbuf, Format(8), ltstr, ldstr);
		}
		SetWindowText(hWnd, tbuf);

    } else {

        /* Display time in open window title */

		char ldstr[24], ltstr[24], udstr[24], utstr[24];

        if (RealTime) {
			eddate(ldstr, (long) (lt.tm_year + 1900), lt.tm_mon + 1, lt.tm_mday, lt.tm_wday);
			edtime(ltstr, lt.tm_hour, lt.tm_min, lt.tm_sec);
			eddate(udstr, (long) (ct.tm_year + 1900), ct.tm_mon + 1, ct.tm_mday, ct.tm_wday);
			edtime(utstr, ct.tm_hour, ct.tm_min, ct.tm_sec);
			V sprintf(tbuf, Format(4),
				ltstr, tzName[!!lt.tm_isdst], ldstr,
 				tracking ? sat.satname : "", utstr, udstr);
		} else {
			long yy;
    		int mm, dd, wday, hh, mmm, ss;

            jyear(faketime, &yy, &mm, &dd);
            jhms(faketime, &hh, &mmm, &ss);
            wday = ((int) fmod(faketime, 7)) + 1;
            if (wday == 7) {
            	wday = 0;
            }
			eddate(udstr, yy, mm, dd, wday);
			edtime(utstr, hh, mmm, ss);
            sprintf(tbuf, Format(5), tracking ? sat.satname : "", utstr, udstr);
		}
		SetWindowText(hWnd, tbuf);
    }
#endif
}


