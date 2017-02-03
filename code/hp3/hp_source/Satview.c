#define PALIDX								// *Must* be set the same as in SUNCALC.C
/*

			Sunclock for Windows

		View from Satellite information panel

*/

#include "Sunclock.h"
#include "Linalg.h"


static HDC hhdc;                            // HDC hidden argument to drawvec
static int lx, ly;							// Last X and Y for optimisation
static int yieldYet;						// Time to yield to other processes ?
static RECT screct;							// Display panel drawing area
static int ltop, lleft, lwid, lhgt;			// Logical drawing area upper corner and size
static int imagedia;						// Earth image diameter

static int izclip;							// Z clipping plane distance from centre of the Earth
static double sscale;						// Projected vector to screen scale factor
static double satla, satlo;					// Satellite latitude, longitude in degrees
static double satlacos, satlasin;			// Satellite latitude sine and cosine

static double lastslat, lastslon, lastsalt;	// Save last parameters for refresh
static double currslat, currslon, currsalt;	// Position of satellite being tracked
static long bslat, bslon, bsalt;			// View that saved bitmap represents

static DWORD lastDrawTime = 0;				// Ticks last update required to draw
static DWORD lastDrawEnd = 0;				// Time last update completed

static HBITMAP svbmap = NULL;				// Satellite view bitmap
static HGLOBAL svdib = NULL;				// Texture mapped DIB

int texturing = TRUE;						// Texture mapped image desired ?
int satvterm = TRUE;						// Show terminator in texture mapped image ?
int viewfrom = IDM_F_SATVIEW1;				// View from where ?
int viewFromFrustrated = FALSE;				// View from waiting for bitmap to be generated

/* These precalculated sine and cosine tables are *big* (24.5K) and eat a large
   portion of DGROUP.  If we get into a tight memory situation, they can be
   moved to global memory allocated dynamically when we need them. */

struct qTab {
	float qSin, qCos;
};
static struct qTab qLon[2048], qLat[1024];
static double qscale;

/*  IQTAB  --  Table of precalculated scaled sines and cosines for satellite
			   view rendering.  Saves hundreds of thousands of sine and cosine
			   calls which would be required for a straightforward projection of
			   the view from the satellite.  */

static void iqtab(void)
{
	int i;

	for (i = 0; i < 2048; i++) {
		double lon = (((i - 1024) * PI) / 1024) + dtr(satlo) - (PI / 2);

		qLon[i].qSin = (float) (sscale * sin(lon));
		qLon[i].qCos = (float) (sscale * cos(lon));
	}

	for (i = 0; i < 1024; i++) {
		double lat = (((i - 512) * (PI / 2)) / 512);

		qLat[i].qSin = (float) (sscale * sin(lat));
		qLat[i].qCos = (float) abs(cos(lat));
	}
}

/*  LLXFORM  --  Brute-force inlining of function that transforms the
				 Mercator-projection vectors to 3D vectors in the satellite's
				 viewpoint frame.  Note also the dirty trick where we don't
				 bother to calculate X and Y if Z is less than the clipping
				 plane distance.  */

#define llxform(lon, lat, x, y, z)	\
	acsflat = (double) qLat[lat].qCos; 												\
	fy = ((double) qLon[lon].qSin) * acsflat;										\
	fz = (double) qLat[lat].qSin;													\
	if ((z = (int) ((fy * -satlasin + fz * satlacos) * qscale)) >= izclip) {		\
		x = imagedia + (int) (((double) qLon[lon].qCos) * acsflat);         		\
		y = imagedia + (int) (fy * satlacos + fz * satlasin);               		\
	}

#ifdef POLLY
POINT pLine[POLLY];
int pLinel = 0;
#endif

#pragma check_stack (off)
static void PASCAL lldraw(int la1, int lo1, int la2, int lo2)
{
	int x1, y1, z1;
	static int x2, y2, z2;
	static int lla2 = -9999, llo2;
	double acsflat, fy, fz;

	if (lla2 == la1 && llo2 == lo1) {
		x1 = x2;
		y1 = y2;
		z1 = z2;
	} else {
		llxform(la1, lo1, x1, y1, z1);
	}
	llxform(la2, lo2, x2, y2, z2);
	lla2 = la2;
	llo2 = lo2;
	if (z1 >= izclip && z2 >= izclip) {
#ifdef POLLY
		if (pLinel >= POLLY || (x1 != lx || y1 != ly)) {
			if (pLinel > 0) {
				Polyline(hhdc, pLine, pLinel);
			}
			pLine[0].x = lleft + x1;
			pLine[0].y = ltop + y1;
			pLine[1].x = lleft + x2;
			pLine[1].y = ltop + y2;
			pLinel = 2;
		} else {
			pLine[pLinel].x = lleft + x2;
			pLine[pLinel++].y = ltop + y2;
		}
		lx = x2;
		ly = y2;
#else
		if (x1 != lx || y1 != ly) {
			lx = x1;
			ly = y1;
			MoveTo(hhdc, lleft + x1, ltop + y1);
		}
		LineTo(hhdc, lleft + x2, ltop + y2);
#endif
	}
}
#pragma check_stack ()

/*  TEXTUREMAPIMAGE  --  Create texture mapped image of globe.  */

static void textureMapImage(int isize, int tmwidth, int tmheight,
							int diwidth, int diheight, int showterm,
							double satlat, double satlon, double satalt)
{
	Vector satpos, v, n, u, up, ippo, raydir, iv;
	double acslat = abs(cos(satlat)), a, l, alpha, vheight, zclip,
		   vl, ui, vi, b, disc, t, tmwo2pi, tmhopi,
		   normTemp;
	int x, y, tmx, tmy, iso2, tmwo2, tmho2, tmhm1;
	LPBITMAPINFOHEADER bh, obmap;
	WORD ncol, offbits, rowlen, tmrowlen;
	unsigned char *opixel;
	unsigned char *tpixel;
	unsigned char termmask;

#define FastVecNorm(a)		assert(VecLen(a) > 0.0); normTemp = 1.0 / VecLen(a); VecScale(normTemp, a)

	termmask = (unsigned char) (showterm ? 0x0 : 0x80);
	bh = (LPBITMAPINFOHEADER) LockResource(earthImageDIB);
	ncol = (WORD) (bh->biClrUsed == 0 ? (1L << bh->biBitCount) : bh->biClrUsed);
	offbits = (WORD) (bh->biSize + ncol * sizeof(RGBQUAD));
	tpixel = ((unsigned char *) (bh)) + offbits;
	tmrowlen = ((tmwidth + (sizeof(LONG) - 1)) / sizeof(LONG)) * sizeof(LONG);
	tmhm1 = tmheight - 1;

	if (svdib != NULL) {
		GlobalFree(svdib);
		svdib = NULL;
	}
	rowlen = ((diwidth + (sizeof(LONG) - 1)) / sizeof(LONG)) * sizeof(LONG);
	svdib = GlobalAlloc(GHND, offbits + ((LONG) diheight) * rowlen);
	obmap = (LPBITMAPINFOHEADER) GlobalLock(svdib);
	memcpy(obmap, bh, offbits);
	obmap->biWidth = diwidth;
	obmap->biHeight = diheight;
	obmap->biSizeImage = ((DWORD) diheight) * rowlen;
	opixel = ((unsigned char *) (obmap)) + offbits;

#define PixA(x, y) ((tpixel + ((x) + (((DWORD) (tmhm1 - (y))) * (tmrowlen)))))
#define Opix(x, y) ((opixel + ((x) + (((DWORD) ((diheight - 1) - (y))) * (rowlen)))))

 	a = satalt + EarthRad;
 	assert(a > 0.0);
	MakeVector(a * cos(satlon) * acslat, a * sin(satlon) * acslat, -a * sin(satlat), satpos);
 	l = sqrt(a * a - EarthRad * EarthRad);
 	alpha = asin(EarthRad / a);
 	vheight = l * sin(alpha);
 	zclip = a - (l * cos(alpha));

 	VecCopy(satpos, v);
 	assert(VecLen(v) > 0.0);
 	vl = -1.0 / VecLen(v);
 	VecScale(vl, v);			// Invert and normalise at once

 	// n = Unit direction vector from eye position to North pole
 	MakeVector(0, 0, EarthRad, n);
 	VecSub(n, satpos, n);
 	FastVecNorm(n);

 	// u = v X n -- View right vector
 	VecCross(v, n, u);
 	FastVecNorm(u);

 	// up = u x v -- View up vector
 	VecCross(u, v, up);

 	VecCopy(satpos, ippo);
 	VecScale(zclip / a, ippo);

	// Precompute some values we'll need repeatedly inside the big loop

    iso2 = isize / 2;
    tmwo2 = tmwidth / 2;
    tmho2 = tmheight / 2;
    tmwo2pi = (tmwidth / 2) / PI;
    tmhopi = tmheight / PI;

 	for (y = 0; y < isize; y++) {
		unsigned char *opix = Opix(lleft, y);
		double viXupX, viXupY, viXupZ;
		double visq;

 		vi = -(((double) y) - iso2) / iso2;
 		visq = vi * vi;

 		viXupX = vi * up[X];
 		viXupY = vi * up[Y];
 		viXupZ = vi * up[Z];

 		for (x = 0; x < isize; x++) {
 			ui = (((double) x) - iso2) / iso2;

 			if (((ui * ui) + visq) <= 1.0) {

				raydir[X] = (ippo[X] - (ui * u[X] + viXupX) * vheight) - satpos[X];
				raydir[Y] = (ippo[Y] - (ui * u[Y] + viXupY) * vheight) - satpos[Y];
				raydir[Z] = (ippo[Z] - (ui * u[Z] + viXupZ) * vheight) - satpos[Z];
	 			FastVecNorm(raydir);

				iv[X] = -satpos[X];
				iv[Y] = -satpos[Y];
				iv[Z] = -satpos[Z];
	 			b = VecDot(iv, raydir);
	 			disc = (b * b - VecDot(iv, iv)) + EarthRad * EarthRad;
	 			if (disc >= 0) {			// If disc < 0, no intersection
	 				disc = sqrt(disc);
	 				t = ((b - disc) < 1e-6) ? b + disc : b - disc;

	 					tmx = (int) (tmwo2 - (tmwo2pi * atan2(t * raydir[Y] + satpos[Y], t * raydir[X] + satpos[X])));
	 					tmy = (int) (tmho2 + (tmhopi * asin((t * raydir[Z] + satpos[Z]) / EarthRad)));

						opix[x] = *(PixA(tmx, tmy)) | termmask;
	 			}
	 		}
 		}
 	}
	GlobalUnlock(svdib);
    UnlockResource(earthImageDIB);
}

/*  PAINTDIB  --  Paint the saved texture mapped DIB into the device surface.  */

static void paintDIB(HDC hDC)
{
	LPBITMAPINFOHEADER obmap, bp;
	WORD ncol, offbits;
	int bmode = DIB_RGB_COLORS;
	unsigned char *opixel;
	HPALETTE opal;

	bp = obmap = (LPBITMAPINFOHEADER) GlobalLock(svdib);
	ncol = (WORD) (obmap->biClrUsed == 0 ? (1L << obmap->biBitCount) : obmap->biClrUsed);
#ifdef PALIDX
	bp = palMapStart(obmap, &bmode);
#endif    	
	offbits = (WORD) (obmap->biSize + ncol * sizeof(RGBQUAD));
	opixel = ((unsigned char *) (obmap)) + offbits;
	opal = SelectPalette(hDC, imagePal, FALSE);
	RealizePalette(hDC);
	SetDIBitsToDevice(hDC, 1, 1, screct.right - 2, screct.bottom - 2,
    		1, 1, 0, screct.bottom - 1,
    		opixel, (LPBITMAPINFO) bp, bmode);
#ifdef PALIDX    		
    palMapEnd();
#endif    
	GlobalUnlock(svdib);
	SelectPalette(hDC, opal, FALSE);
}

/*  UPDSATVIEW  --  Update view from satellite.  */

static void updsatview(HWND hWnd, double satlat, double satlon, double satalt)
{
	HPEN dpen, oldPen;
	HBRUSH dbrush, obrush;
	HCURSOR oldcur;
	HWND panel;
	HDC hDC, hMemDC;
	HBITMAP obmap;
	double a, l, alpha, vheight, zclip;
	DWORD drawStart;							// Time this draw began
	int timeYet;								// Time to update ?
	static int textured = FALSE;				// Is saved bitmap texture mapped ?

#define Bres(x) ((long) ((x) * (1800 / PI)))	// Fixed point latitude and longitude for tests

	lastslat = satlat;							// Save last call arguments for repaint
	lastslon = satlon;                          // and for first display when we're activated
	lastsalt = satalt;

	/* To avoid hogging the machine, don't update any more frequently than
	   twice the time required to paint the screen.  */

	drawStart = GetTickCount();					// Current tick count
	timeYet = drawStart >= (lastDrawEnd + (2 * lastDrawTime));

	/* One more trick--if this is a pure repaint to repair screen damage
	   (e.g. the satellite hasn't changed), and we have a backing bitmap,
	   and it isn't time to recalculate, just repair with the bitmap.
	   Also, if the satellite position hasn't changed measurably, there's
	   no need to recalculate. */

	panel = GetDlgItem(hWnd, IDC_SV_IMAGE);
	if (((svbmap != NULL) || (svdib != NULL)) &&
						 	(!timeYet || ((bslat == Bres(satlat)) &&
										  (bslon == Bres(satlon)) &&
										  (bsalt == ((long) satalt))
						  			     )
						    )
	   ) {
		hDC = GetDC(panel);
		GetClientRect(panel, &screct);
		if (svdib != NULL) {
			paintDIB(hDC);
		} else {
			hMemDC = CreateCompatibleDC(hDC);
			obmap = SelectObject(hMemDC, svbmap);
			BitBlt(hDC, 1, 1, screct.right - 2, screct.bottom - 2,
					hMemDC, 1, 1, SRCCOPY);
			SelectObject(hMemDC, obmap);
			DeleteDC(hMemDC);
		}
		ReleaseDC(panel, hDC);
		return;
	}

    oldcur = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);
	a = satalt + EarthRad;						// Geocentric altitude of satellite
	l = sqrt(a * a - EarthRad * EarthRad);		// Distance to visible limb of Earth
	alpha = asin(EarthRad / a);					// Half-angle subtended by Earth from satellite
	vheight = l * sin(alpha);					// Diameter of visible disc from satellite, km
	zclip = a - (l * cos(alpha));				// Z-clipping plane distance from Earth's centre

	hDC = GetDC(panel);
	GetClientRect(panel, &screct);
	lwid = lhgt = (min(screct.right, screct.bottom) & -2) - 2;
	ltop = (screct.bottom - lhgt) / 2;
	lleft = (screct.right - lwid) / 2;
	if (svbmap == NULL) {
		svbmap = CreateCompatibleBitmap(hDC, screct.right, screct.bottom);
	}
	hMemDC = CreateCompatibleDC(hDC);
	obmap = SelectObject(hMemDC, svbmap);

	hhdc = hMemDC;
	sscale = (lwid / 2) / (vheight / EarthRad);
	izclip = (int) (zclip * (10000 / EarthRad));
	satlacos = cos(satlat + PI / 2);
	satlasin = sin(satlat + PI / 2);
	satla = rtd(satlat);
	satlo = rtd(satlon);

	BitBlt(hhdc, 1, 1, screct.right - 1, screct.bottom - 1, NULL, 0, 0, BLACKNESS);
	if ((viewfrom != IDM_F_SATVIEW1) || tracking) {
		char tbuf[80];
        double slo = satlo;

	    // Disable position displayed to show projection in progress
	    EnableWindow(GetDlgItem(hWnd, IDC_SV_POSITION), FALSE);

	    /* If the image was loaded already or we just succeeded in loading it, generate
	       the texture mapped image. */

		if (texturing && abs(displaymode) >= 255) {
			if (earthImageDIB != NULL && imagePal != NULL) {

				// Texture mapped image generation

// #define tmTiming
	 			textured = TRUE;
#ifdef tmTiming
				{
				DWORD tix = GetTickCount();
				char tstr[80];
#endif
				textureMapImage(lwid, ImageWid, ImageHgt, screct.right, screct.bottom, satvterm,
					satlat, satlon, satalt);
#ifdef tmTiming
				tix = GetTickCount() - tix;
				sprintf(tstr, "Time: %ld", tix);
				SetWindowText(hWnd, tstr);
				}
#endif
				viewFromFrustrated = FALSE;
			} else {
				viewFromFrustrated = TRUE;		// Indicate waiting for main window to repaint
			}
		} else {

			// Vector image generation

			textured = FALSE;
			dpen = CreatePen(PS_SOLID, 1, abs(displaymode) == 1 ? GetSysColor(COLOR_WINDOW) :
						RGB(255, 255, 0));
			dbrush = CreateSolidBrush(abs(displaymode) == 1 ? GetSysColor(COLOR_WINDOWTEXT) :
							RGB(0, 0, 255));
			obrush = SelectObject(hhdc, dbrush);
			Ellipse(hhdc, lleft, ltop, lleft + lwid, ltop + lhgt);
			SelectObject(hhdc, obrush);
			DeleteObject(dbrush);
			oldPen = SelectObject(hhdc, dpen);
			yieldYet = 0;
			lx = -32000;
			iqtab();
			qscale = 10000 / sscale;
			imagedia = lwid / 2;
#ifdef POLLY
			pLinel = 0;
#endif
			mapdraw(2048, 1024, lldraw);
#ifdef POLLY
			if (pLinel > 0) {
				Polyline(hhdc, pLine, pLinel);
			}
#endif
			SelectObject(hhdc, oldPen);
			DeleteObject(dpen);
			viewFromFrustrated = FALSE;
		}
		lastDrawEnd = GetTickCount();
		lastDrawTime = lastDrawEnd - drawStart;
		bslat = Bres(satlat);
		bslon = Bres(satlon);
		bsalt = (long) satalt;
		if (svdib != NULL) {
			paintDIB(hDC);
		} else {
			BitBlt(hDC, 1, 1, screct.right - 2, screct.bottom - 2, hMemDC, 1, 1, SRCCOPY);
		}

		// Display the view-from position below the image

        if (slo > 180) {
        	slo = slo - 360;
        }
	    sprintf(tbuf, Format(viewfrom == IDM_F_VFOBSLOC ? 47 : 25),
	    			abs(satla), satla >= 0 ? 'N' : 'S',
	    			abs(slo), slo > 0 ? 'W' : 'E', satalt);
	    SetDlgItemText(hWnd, IDC_SV_POSITION, (LPSTR) tbuf);
	    EnableWindow(GetDlgItem(hWnd, IDC_SV_POSITION), TRUE);
	} else {
		SetDlgItemText(hWnd, IDC_SV_POSITION, (LPSTR) "");
	}
	ReleaseDC(panel, hDC);
	SelectObject(hMemDC, obmap);
	DeleteDC(hMemDC);
	ShowCursor(FALSE);
	SetCursor(oldcur);
#undef Bres
}

/*  RELEASE_SATVIEW_BITMAPS  --  Release view from satellite backing bitmap.  */

void release_satview_bitmaps(void)
{
	if (svbmap != NULL) {
		DeleteObject(svbmap);
		svbmap = NULL;
	}
	if (svdib != NULL) {
		GlobalFree(svdib);
		svdib = NULL;
	}
}

/*  SATVIEWTITLE  --  Set title for view from window.  */

static void satviewtitle(HWND hDlg)
{
	char svtitle[80];

	switch (viewfrom) {
		case IDM_F_SATVIEW1:
               if (tracking) {
#ifndef tmTiming
				sprintf(svtitle, Format(23), sat.satname);
				SetWindowText(hDlg, svtitle);
#endif
			} else {
				SetWindowText(hDlg, Format(24));
			}
			break;

		case IDM_F_DAYVIEW:
			SetWindowText(hDlg, Format(26));
			break;

		case IDM_F_MOONVIEW:
			SetWindowText(hDlg, Format(28));
			break;

		case IDM_F_NIGHTVIEW:
			SetWindowText(hDlg, Format(27));
			break;
			
		case IDM_F_VFOBSLOC:
			sprintf(svtitle, Format(48), siteName);
			SetWindowText(hDlg, svtitle);		
			break;

	}
}

/*  SATVIEWCHANGED  --  Inform satellite view that satellite being tracked just changed.  */

void satviewchanged(void)
{
	if (satViewPanel != NULL) {
		release_satview_bitmaps();
		lastDrawTime = lastDrawEnd = 0;
		if (tracking) {
			satviewtitle(satViewPanel);
		} else {
			// If we've stopped tracking a satellite, close the view-from panel
			if (viewfrom == IDM_F_SATVIEW1) {
				SendMessage(satViewPanel, WM_CLOSE, 0, 0L);
			}
		}
	}
	skySatChanged();				// Let sky and telescope know satellite changed
}

/*  CHANGESATVIEW  --  Schedule repaint of satellite window when satellite moves.  */

void changesatview(double satlat, double satlon, double satalt)
{
	currslat = satlat;
	currslon = satlon;
	currsalt = satalt;
	if ((viewfrom == IDM_F_SATVIEW1) && (satViewPanel != NULL)) {
		InvalidateRgn(satViewPanel, NULL, FALSE);
	}
}

/*  CHANGEMOONVIEW  --  Schedule repaint of view from Moon.  */

void changemoonview(void)
{
	if ((viewfrom == IDM_F_MOONVIEW) && (satViewPanel != NULL)) {
		InvalidateRgn(satViewPanel, NULL, FALSE);
	}
}

/*  CHANGESUNVIEW  --  Schedule repaint of view from Sun.  */

void changesunview(void)
{
	if (((viewfrom == IDM_F_DAYVIEW) || (viewfrom == IDM_F_NIGHTVIEW) ||
		 (viewfrom == IDM_F_VFOBSLOC) || 
		 ((viewfrom == IDM_F_SATVIEW1) && texturing && satvterm))
			 && (satViewPanel != NULL)) {
		bsalt = 0;
		InvalidateRgn(satViewPanel, NULL, FALSE);
	}
}

/*  SATVIEWDLG  --  View from satellite dialogue procedure.  */

DialogueProc(SatViewDlg)
{
	RECT dw, cw;
	HWND cWnd;
	static RECT button1;
	static int buttonY, buttonDeltaX, buttonBarycentre, lastx, lasty, imagex, imagey,
			   minx, miny, dragging = FALSE;

	switch (message) {
		case WM_INITDIALOG:
			SendMessage(GetDlgItem(hDlg, IDC_SV_SHOWTERM), BM_SETCHECK, satvterm, 0L);
			GetClientRect(hDlg, &dw);
			lastx = dw.right;
			lasty = dw.bottom;
			GetWindowRect(hDlg, &cw);
			minx = cw.right - cw.left;
			miny = cw.bottom - cw.top;
			GetWindowRect(GetDlgItem(hDlg, IDOK), &button1);
			ScreenToClient(hDlg, (POINT FAR *) &(button1.left));
			buttonY = dw.bottom - button1.top;
			GetWindowRect(GetDlgItem(hDlg, IDC_SV_HELP), &cw);
			ScreenToClient(hDlg, (POINT FAR *) &(cw.left));
			ScreenToClient(hDlg, (POINT FAR *) &(cw.right));
			buttonDeltaX = cw.left - button1.left;
			buttonBarycentre = ((dw.right - button1.left) / 2) - button1.left;
			GetWindowRect(GetDlgItem(hDlg, IDC_SV_IMAGE), &cw);
			ScreenToClient(hDlg, (POINT FAR *) &(cw.left));
			ScreenToClient(hDlg, (POINT FAR *) &(cw.right));
			imagex = cw.right - cw.left;
			imagey = cw.bottom - cw.top;
   			return TRUE;

		case WM_CLOSE:
			DestroyWindow(hDlg);
			return TRUE;
			
		case WM_DESTROY:
			release_satview_bitmaps();
			satViewPanel = NULL;
			return TRUE;

        case WM_PALETTECHANGED:
        	// Force repaint if somebody changes the system palette
        	if (hDlg != ((HWND) wParam)) {
        		UpdateWindow(hDlg);	        		
        	}
			return TRUE;			

		case WM_GETMINMAXINFO:
			{	MINMAXINFO FAR *mmi = (MINMAXINFO FAR *) lParam;

				mmi->ptMinTrackSize.x = minx;
				mmi->ptMinTrackSize.y = miny;
			}
			break;

		case WM_SIZE:
			{
				int cwidth, cheight;

				cwidth = LOWORD(lParam);
				cheight = HIWORD(lParam);
#define floatChild(x)	cWnd = GetDlgItem(hDlg, x);								\
						GetWindowRect(cWnd, &cw);								\
						ScreenToClient(hDlg, (POINT FAR *) &(cw.left));			\
						cw.left += (cwidth - lastx) / 2;						\
						cw.top += (cheight - lasty);							\
						SetWindowPos(cWnd, NULL, cw.left, cw.top, 0, 0,			\
							SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER)

				floatChild(IDOK);
				floatChild(IDC_SV_HELP);
				floatChild(IDC_SV_SHOWTERM);
				floatChild(IDC_SV_POSITION);
				release_satview_bitmaps();
				cWnd = GetDlgItem(hDlg, IDC_SV_IMAGE);
				SetWindowPos(cWnd, NULL, 0, 0, imagex + (cwidth - lastx),
							imagey + (cheight - lasty),
							SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
				imagex += (cwidth - lastx);
				imagey += (cheight - lasty);
				lastx = cwidth;
				lasty = cheight;
			}
			return TRUE;
 
		case WM_ENTERSIZEMOVE:
			dragging = TRUE;
			break;

		case WM_EXITSIZEMOVE:
			dragging = FALSE;
			InvalidateRect(hDlg, NULL, FALSE);
			UpdateWindow(hDlg);
			break;

		case WM_PAINT:
			{	PAINTSTRUCT ps;

			    (void) BeginPaint(hDlg, &ps);
				if (dragging) {
					HDC hDC;
					RECT cr;
					HWND hWnd = GetDlgItem(hDlg, IDC_SV_IMAGE);

					hDC = GetDC(hWnd);
            		GetClientRect(hWnd, &cr);
					FillRect(hDC, &cr, (HBRUSH) GetStockObject(BLACK_BRUSH));
					ReleaseDC(hWnd, hDC);
				} else {
					satviewtitle(hDlg);
					ShowWindow(GetDlgItem(hDlg, IDC_SV_SHOWTERM), (abs(displaymode) >= 255) && texturing ?
						SW_SHOWNORMAL : SW_HIDE);

					if (viewfrom == IDM_F_SATVIEW1) {
						lastslat = currslat;
						lastslon = currslon;
						lastsalt = currsalt;
					} else {
						double ijdate, moonclong, moonclat, mooncrv, moonlong, moonra, moondec, igmst,
							   cphase, aom, cdist, cangdia, csund, csuang,
							   sunra, sundec, sunrv, sunlong, subslong;

							ijdate = faketime;
				    		igmst = gmst(ijdate);

							if (viewfrom == IDM_F_DAYVIEW || viewfrom == IDM_F_NIGHTVIEW) {
					    		sunpos(ijdate, TRUE, &sunra, &sundec, &sunrv, &sunlong);
					    		subslong = (igmst * 15) - sunra;
					    		if (viewfrom == IDM_F_NIGHTVIEW) {
					    			subslong = fixangle(subslong + 180);
					    			sundec = -sundec;
					    		}
					   			if (subslong > 180) {
					   				subslong = -(360 - subslong);
					   			} else if (subslong < -180) {
					   				subslong += 360;
					   			}
								lastslat = dtr(sundec);
								lastslon = dtr(subslong);
								lastsalt = sunrv * SunSMAX;
							} else if (viewfrom == IDM_F_VFOBSLOC) {
								lastslat = dtr(siteLat);
								lastslon = dtr(siteLon);
								lastsalt = SunSMAX;							
							} else {
								highmoon(ijdate, &moonclong, &moonclat, &mooncrv);
								ecliptoeq(ijdate, moonclong, moonclat, &moonra, &moondec);
					   			moonlong = (igmst * 15) - moonra;
					    		(void) phase(ijdate, &cphase, &aom, &cdist, &cangdia, &csund, &csuang);
					   			if (moonlong > 180) {
					   				moonlong = -(360 - moonlong);
					   			} else if (moonlong < -180) {
					   				moonlong += 360;
					   			}
					   			lastslat = dtr(moondec);
					   			lastslon = dtr(moonlong);
					   			lastsalt = mooncrv;
				   		}
					}
					updsatview(hDlg, lastslat, lastslon, lastsalt);
				}
				EndPaint(hDlg, &ps);
			}
			return TRUE;

        case WM_COMMAND:
            switch (WM_COMMAND_ID(wParam)) {

                case IDOK:
                    SendMessage(hDlg, WM_CLOSE, 0, 0L);
                    return TRUE;

                case IDC_SV_HELP:
                	WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                				((DWORD) ((LPSTR) rstring(IDS_HELP_ON_SATVIEW))));
                	holped = TRUE;
                	return TRUE;

                case IDC_SV_SHOWTERM:
                	SendMessage((HWND) LOWORD(lParam), BM_SETCHECK,
                		satvterm = (WORD) !SendMessage((HWND) LOWORD(lParam), BM_GETCHECK, 0, 0L), 0L);
                	release_satview_bitmaps();
                    InvalidateRgn(hDlg, NULL, FALSE);
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
