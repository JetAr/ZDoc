/*
                                                                                      
		Sky Window procedure

		Since the telescope and horizon views have so much in common wit
		the sky window, their dialogue handlers are also in this file.

*/

#include "Sunclock.h"
#include "Project.h"

#define WM_U_UPDATESCOPE (WM_USER + 1)	// Update telescope to location picked in sky window

double skyLimitMag = 5.5;				// Limiting magnitude of sky map

int skyShowName = TRUE;					// Show star names in sky map ?
int telShowName = TRUE;					// Show star names in telescope ?
int horShowName = TRUE;					// Show star names at horizon ?
double skyNameMag = 2;					// Maximum magnitude to show names in sky
double telNameMag = 3.5;				// Maximum magnitude to show names in telescope
double horNameMag = 2;					// Maximum magnitude to show names at horizon
int skyShowBflam = TRUE;				// Show Bayer/Flamsteed numbers in sky ?
int telShowBflam = TRUE;				// Show Bayer/Flamsteed numbers in telescope ?
int horShowBflam = TRUE;				// Show Bayer/Flamsteed numbers at horizon ?
double skyBflamMag = 2.5;				// Maximum magnitude to show Bayer/Flamsteed in sky
double telBflamMag = 4;					// Maximum magnitude to show Bayer/Flamsteed in telescope
double horBflamMag = 2.5;				// Maximum magnitude to show Bayer/Flamsteed at horizon
int skyShowMag = FALSE;					// Show magnitudes in sky ?
int telShowMag = FALSE;					// Show magnitudes in telescope ?
int horShowMag = FALSE;					// Show magnitudes at horizon ?
double telShowMagMax = -1.5, telShowMagMin = 6; // Telescope show magnitude limits
int skyShowDeep = TRUE;					// Show deep sky objects in sky ?
int telShowDeep = TRUE;					// Show deep sky objects in telescope ?
int horShowDeep = TRUE;					// Show deep sky objects at horizon ?
double skyDeepMag = 2.5;				// Maximum magnitude to show deep sky objects in sky
double telDeepMag = 6.5;				// Maximum magnitude to show deep sky objects in telescope
double horDeepMag = 2.5;				// Maximum magnitude to show deep sky objects at horizon
int telSouthUp = FALSE;					// South up in telescope ?
int skyShowConstellations = TRUE;		// Show constellations
int skyShowConbounds = TRUE;			// Show constellation boundaries ?
int skyShowConnames = TRUE;				// Show constellation names ?
int skyAlignConnames = FALSE;			// Align constellation names with map boundary ?
int skyShowCoords = TRUE;				// Show coordinate grid ?
int skyShowPlanets = TRUE;				// Show solar system objects ?
int telShowConstellations = TRUE;		// Show constellations in telescope ?
int telShowConbounds = TRUE;			// Show constellation boundaries in telescope ?
static int telAlignConnames = FALSE;	// Align constellation names ? (Always FALSE for telescope)
int telShowConnames = TRUE;				// Show constellation names in telescope ?
int telShowCoords = TRUE;				// Show coordinate grid in telescope ?
int telShowPlanets = TRUE;				// Show solar system objects in telescope ?
int horShowConstellations = TRUE;		// Show constellations at horizon ?
int horShowConbounds = TRUE;			// Show constellation boundaries at horizon ?
static int horAlignConnames = FALSE;	// Align constellation names ? (Always FALSE for horizon)
int horShowConnames = TRUE;				// Show constellation names at horizon ?
int horShowCoords = TRUE;				// Show coordinate grid at horizon ?
int horShowPlanets = TRUE;				// Show solar system objects at horizon ?
int horShowTerrain = TRUE;				// Show terrain at horizon ?
int horShowScenery = TRUE;				// Show scenery at horizon ?
double horSceneryRoughness = 0.7;		// Fractal dimension of terrain at horizon
char userCat[64] = "-";					// User catalogue file name  

int precessionCalculation = PrecAuto;	// Precession calculation mode

#pragma pack(1)							// starMapData must be byte-packed
struct starMapData {
	unsigned short lon;
	short lat;
	unsigned short mag;
	unsigned char spectral;
};
#pragma pack()

static HWND scopeDlg = NULL;			// Telescope dialogue window
static HWND horizonDlg = NULL;			// Horizon dialogue window

static HINSTANCE starMapLib = 0;		// Star map DLL instance handle
static HGLOBAL starMapRes = NULL;       // Star map resource global block handle
static HRSRC starMapHandle = NULL;		// Star map resource handle
static HBITMAP smbitmap = NULL;			// Screen restore bitmap
static int bitmapValid = FALSE;			// Is bitmap current ?
static HBITMAP tvbitmap = NULL;			// Telescope restore bitmap
static int tvbitmapValid = FALSE;		// Is telescope bitmap current ?
static HPALETTE starPal = NULL;			// Star colour rendering palette 

static HBITMAP hobitmap = NULL;			// Horizon bitmap
static int hobitmapValid = FALSE;		// Is horizon bitmap current ?

#define Power1FOV	45.0				// Field of view for telescope power 1
int telActive = FALSE;					// Telescope active flag	
double telra = 0, teldec = 0, telalt = 0, telazi = 0;	// Telescope aim point 
static double telfov = Power1FOV, telpower = 1, telimag = 5.5;	// Telescope settings
static double horfov = 45, horazi = 0, horlimag = 5.5;	// Horizon settings
static double skyLham;					// Local hour angle offset at last sky map generation
static int Flip = 1;					// 1 if Northern hemisphere, -1 if Southern

static double skyLast = -1E10;
static double skyInterval = CalculationInterval(30);
static double skyCalcTime = CalculationInterval(30),
			  telCalcTime = CalculationInterval(30),
			  hoCalcTime  = CalculationInterval(30);

static int lastSatX = -2000, lastSatY;	// Last satellite position in sky

HICON planetIcons[12] = {NULL};			// Solar system object icon handles
static HICON starIcons[9] = {NULL};		// Star stamper icon handles
static HICON deepIcons[10] = {NULL};	// Deep sky object icons
static int pirefc = 0;                  // Planet icons reference count

static int clientOverheadX, clientOverheadY; /* Window to client overhead in
										   pixels for resizing */
static BOOL draggingSize = FALSE;		// Is user dragging window ?

int starCatalogue = 0;					// Star catalogue selection
int starQuality = 0;					// Star quality/time tradeoff
int showStarColours = TRUE;				// Show star colours ?

static char *starCatResName[] = {		// Star catalogue resource names (don't translate these)
	"Yale",                             // 0: Yale bright star catalogue
	"Sao"                               // 1: SAO star database
};

static DLGPROC editReturnProc = NULL,	// Special edit control procedures
			   editTabReturnProc = NULL;
static DLGPROC scopeDlgProc = NULL,		// Telescope dialogue procedure
    		   horizonDlgProc = NULL;	// Horizon dialogue procedure

#define BrightestMag -1
#define DimmestMag	18
#define BinsPerMag	2

static void updateTelescope(void), updateHorizon(void);

/*  LAUNCHSKYWINDOW  --  Activate the sky window, if it's not already open.  */

void launchSkyWindow(void)
{
 	if (skyViewPanel == NULL) {
		RECT r;
		DWORD skyStyle =
	        WS_CAPTION      |       // Title and Min/Max
	        WS_SYSMENU      |       // Add system menu box
	        WS_MAXIMIZEBOX	|		// Add maximise box
	        WS_MINIMIZEBOX  |       // Add minimise box
	        WS_THICKFRAME   |       // Thick frame: allow resize
	        WS_CLIPCHILDREN |       // don't draw in child windows areas
			WS_OVERLAPPED;          // this is a conventional overlapped window
			

		r.left = r.top = 0;
		r.right = r.bottom = 400;
		AdjustWindowRect(&r, skyStyle, TRUE);
		clientOverheadX = (r.right - r.left) - 400;
		clientOverheadY = (r.bottom - r.top) - 400;

	    skyViewPanel = CreateWindow(
	        "SkyWindow",            // Window class name
	        "",   					// Window's title
			skyStyle,				// Window style
	        CW_USEDEFAULT, 0,       // Use default X, Y
			r.right - r.left,		// Window width
			r.bottom - r.top,		// Window height
			NULL,                   // Parent window's handle
	        NULL,                   // Default to Class Menu
	        hInst,                  // Instance of window
	        NULL);                  // Create struct for WM_CREATE

	    ShowWindow(skyViewPanel, TRUE); /* Display main window */
 	}
}

/*  LOADPLANETICONS  --  Load the planet icons from the resource file.  */

void loadPlanetIcons(void)
{
	if (pirefc < 1) {
		int i;

		for (i = 0; i < ELEMENTS(planetIcons); i++) {
			planetIcons[i] = LoadIcon(hInst, MAKEINTRESOURCE(IDR_PLANETS + i));
		}
		for (i = 0; i < ELEMENTS(starIcons); i++) {
			starIcons[i] = LoadIcon(hInst, MAKEINTRESOURCE(IDR_STARS + i));
		}
		for (i = 0; i < ELEMENTS(deepIcons); i++) {
			deepIcons[i] = LoadIcon(hInst, MAKEINTRESOURCE(IDR_DEEP + i));
		}
	}
	pirefc++;
}
/*  UNLOADPLANETICONS  --  Release the planet icons when we're done with them.  */

void unloadPlanetIcons(void)
{
	pirefc--;
	if (pirefc < 1) {
		int i;

		for (i = 0; i < ELEMENTS(planetIcons); i++) {
			DestroyIcon(planetIcons[i]);
			planetIcons[i] = NULL;
		}
		for (i = 0; i < ELEMENTS(starIcons); i++) {
			DestroyIcon(starIcons[i]);
			starIcons[i] = NULL;
		}
		for (i = 0; i < ELEMENTS(deepIcons); i++) {
			DestroyIcon(deepIcons[i]);
			deepIcons[i] = NULL;
		}
	}
}

/* PAINTSKY  --  Paint the sky window.  */

#define	paintTEL	0 				// Telescope
#define paintSKY	1               // Sky
#define paintHOR	2               // Horizon

static HDC hhDC;					// Hidden HDC argument to D_movedraw
static int skyMode;					// Current window being plotted
static double lham;					// Local hour angle in radians

static void calcAltAz(double dec, double ra, double *alt, double *az)
{
	double lha = lham - dtr(ra);
	
	*az = fixangle(180 - rtd(atan2(sin(lha), cos(lha) * sin(dtr(siteLat)) -
							tan(dtr(dec)) * cos(dtr(siteLat)))));
	*alt = rtd(asin(sin(dtr(siteLat)) * sin(dtr(dec)) +
							cos(dtr(siteLat)) * cos(dtr(dec)) * cos(lha)));
}

static void altaz(double *dec, double *ra)
{
	if (skyMode == paintHOR) {
		double rra, rdec;
		
		calcAltAz(*dec, *ra, &rdec, &rra);
		*dec = rdec;
		*ra = rra;                                                
	}
}

void D_movedraw(int xloc1, int yloc1, int xloc2, int yloc2, int line_style)
{
	MoveTo(hhDC, xloc1, yloc1);
	LineTo(hhDC, xloc2, yloc2);
}

/*	Note: plotLine may be used only to plot segments of great circles.
    To plot other lines, call clipr_xform() and drawcurveline() yourself
    with FALSE great_circle arguments.  */

static void plotLine(double fdec, double fra, double tdec, double tra)
{
	int vx, vy, vx2, vy2;
	double cdec1, cra1, cdec2, cra2;
    
    altaz(&fdec, &fra);
    altaz(&tdec, &tra);
	if (clipr_xform(fdec, fra, tdec, tra, &vx, &vy, &vx2, &vy2, FALSE,
		&cdec1, &cra1, &cdec2, &cra2)) {
		drawcurveline(cdec1, cra1, cdec2, cra2, vx, vy, vx2, vy2, 0, TRUE, 0);
	}
}

static void paintSky(int skyTel, HDC hDC, RECT *br, double limag)
{
	int i, vx, vy, vis, vm, vx2, vy2, smhand = -1, pflip, precess;
	void far *smap;
	double igmst, ra, dec, mag;
	unsigned char smex[4];

#define ST(x, y, z)	((skyTel > 0) ? ((skyTel == paintHOR) ? (z) : (x)) : (y))
#define SorT(x) 	ST(sky##x, tel##x, hor##x)

    hhDC = hDC;
    skyMode = skyTel;
	if ((precessionCalculation == PrecAlways) ||
		((precessionCalculation == PrecAuto) && (abs(PrecEpoch - faketime) >
			(PrecYears * 365.25)))) {
		definePrecession(2000.0 + ((faketime - PrecEpoch) / 365.25));
		precess = TRUE;
	} else {
		precess = FALSE;
	}
#define Prd(x, y) if (precess) { precessObject(x, y, &x, &y); }

    if (skyTel == paintSKY) {
		igmst = gmst(faketime);
	    lham = skyLham = dtr((igmst * 15) - siteLon);
		skywin.width = br->right - br->left;
		skywin.height = br->bottom - br->top;
		skywin.x_offset = br->left;
		skywin.y_offset = br->top;
		skywin.proj_mode = STEREOGR;
		pflip = skywin.invert = Flip > 0;
		skywin.mirror = !skywin.invert;
		igmst = gmst(faketime);
		skywin.racen = fixangle((igmst * 15) - siteLon);
		skywin.dlcen = siteLat;
		skywin.scale = 90.0;
		initxform(&skywin);
	} else if (skyTel == paintTEL) {
		igmst = gmst(faketime);
	    lham = skyLham = dtr((igmst * 15) - siteLon);
		telwin.width = br->right - br->left;
		telwin.height = br->bottom - br->top;
		telwin.x_offset = br->left;
		telwin.y_offset = br->top;
		telwin.proj_mode = (telpower > 0.75) ? GNOMONIC : STEREOGR;
		pflip = telwin.invert = !telSouthUp;
		telwin.mirror = !telwin.invert;
		telwin.racen = telra;
		telwin.dlcen = teldec;
		telwin.scale = Power1FOV / telpower;
		initxform(&telwin);
	} else {
		igmst = gmst(faketime);
	    lham = skyLham = dtr((igmst * 15) - siteLon);
		horwin.width = br->right - br->left;
		horwin.height = br->bottom - br->top;
		horwin.x_offset = br->left;
		horwin.y_offset = br->top;
		horwin.proj_mode = GNOMONIC;
		pflip = horwin.invert = TRUE;
		horwin.mirror = FALSE; 
		horwin.racen = -horazi;
		horwin.dlcen = (horfov / 2 * max(horwin.width, horwin.height)) / horwin.width;
		horwin.scale = horfov;
		initxform(&horwin);
	}

	// Draw coordinate grid markers, if requested

	if (SorT(ShowCoords)) {
		int i, hi;
		HPEN cpen, open;
		double epsilon, esin, ecos, eqra, eqdec, eqlat, pera, pedec;
		HFONT ofont, cfont;
#define tickWid	0.75

        cfont = CreateFont(max(15, (br->right - br->left) / 40), 0, 0, 0, FW_NORMAL,
        			FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS,
        			NULL);
        ofont = SelectObject(hDC, cfont);
    	SetBkMode(hDC, TRANSPARENT);
    	SetTextAlign(hDC, TA_CENTER | TA_BASELINE | TA_NOUPDATECP);
        SetTextColor(hDC, RGB(0, 128, 128));

		open = SelectObject(hDC, (cpen = CreatePen(PS_SOLID, 0, RGB(0, 128, 128))));
		for (i = -1; i <= 1; i += 2) {
			// Equinoctual colures
			plotLine(88.0 * i, 0.0, 90.0 * i, 0.0);                    
			plotLine(88.0 * i, 180.0, 90.0 * i, 180.0);

			// Solstitial colures
			plotLine(88.0 * i, 90.0, 90.0 * i, 90.0);
			plotLine(88.0 * i, 270.0, 90.0 * i, 270.0);
		}
		
#define axform(lat, lon, x, y, in) { double la, lo; la = (lat); lo = (lon); altaz(&la, &lo); \
										xform(la, lo, x, y, in); } 			

		// Celestial equator
//		hi = skyTel == paintHOR ? 5 : 15;
hi = 15; 
		for (i = 0; i < 360; i += hi) {		// Draw the tick marks at hour angle intervals
			char hlab[4];
			int vx, vy, vis;

            plotLine(0.0, (double) i, 0.0, (double) (i + hi));
			plotLine(-tickWid, (double) i, tickWid, (double) i);
			if ((i % 15) == 0) {
				sprintf(hlab, Format(42), i / 15);
				axform(tickWid * (pflip ? 1 : -1), (double) i, &vx, &vy, &vis);
				if (vis) {
					TextOut(hDC, vx, vy, hlab, strlen(hlab));
				}
			}
		}

		SelectObject(hDC, open);
		DeleteObject(cpen);

		// Ecliptic

		open = SelectObject(hDC, (cpen = CreatePen(PS_SOLID, 0, RGB(128, 0, 0))));

		epsilon = dtr(obliqeq(faketime));	// Get current obliquity of ecliptic
		esin = sin(epsilon);
		ecos = cos(epsilon);

        SetTextColor(hDC, RGB(128, 0, 0));
		pera = pedec = 0.0;		// Dirty trick: ecliptic intersects equator at 0 longitude !
		for (i = 1; i <= 32; i++) {			// Draw the ecliptic itself
			eqlat = ((PI * 2) / 32.0) * i;
			eqra = fixangle(rtd(atan2(ecos * sin(eqlat), cos(eqlat))));
			eqdec = rtd(asin(esin * sin(eqlat)));
			plotLine(pedec, pera, eqdec, eqra);
			pera = eqra;
			pedec = eqdec;
		}

		for (i = 0; i < 360; i += 15) {		// Draw the tick marks at 15 degree intervals
			char hlab[6];
			int vx, vy, vis;

			eqlat = ((PI * 2) / 360.0) * i;
			pera = fixangle(rtd(atan2((ecos * sin(eqlat) -
					     	(tan(dtr(-tickWid)) * esin)), cos(eqlat))));
			pedec = rtd(asin((esin * sin(eqlat) * cos(dtr(-tickWid))) +
			     		(sin(dtr(-tickWid)) * ecos)));
			eqra = fixangle(rtd(atan2((ecos * sin(eqlat) -
					     	(tan(dtr(tickWid)) * esin)), cos(eqlat))));
			eqdec = rtd(asin((esin * sin(eqlat) * cos(dtr(tickWid))) +
			     		(sin(dtr(tickWid)) * ecos)));
			plotLine(pedec, pera, eqdec, eqra);
			sprintf(hlab, Format(43), i);
			axform(!pflip ? pedec : eqdec, !pflip ? pera : eqra, &vx, &vy, &vis);
			if (vis) {
				TextOut(hDC, vx, vy, hlab, strlen(hlab));
			}

		}
#undef tickWid

		SelectObject(hDC, open);
		DeleteObject(cpen);
		SelectObject(hDC, ofont);
		DeleteObject(cfont);
	}

	// Draw constellation names, if requested

	if (SorT(ShowConnames)) {
		FILE *fp = fopen(rstring(IDS_CNAMES_FILENAME), "r");

		if (fp != NULL) {
			char cl[80], cn[80];
			char *vfmt = Format(40);
			unsigned int tra;
			int tdec;
			HFONT ofont, cfont;

			if (!SorT(AlignConnames)) {
	            cfont = CreateFont(max(15, (br->right - br->left) / 36), 0, 0, 0, FW_NORMAL,
	            			FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
	            			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS,
	            			NULL);
	            ofont = SelectObject(hDC, cfont);
            }
        	SetBkMode(hDC, TRANSPARENT);
           	SetTextColor(hDC, RGB(255, 255, 0));
        	SetTextAlign(hDC, TA_CENTER | TA_BASELINE | TA_NOUPDATECP);
			while (fgets(cl, sizeof cl, fp) != NULL) {
				cl[strlen(cl) - 1] = EOS;

				if (strlen(cl) == 0 || isspace(cl[0]) || cl[0] == ';') {
					continue;
				}
				sscanf(cl, vfmt, &tra, &tdec, cn);
				if (strchr(cn, '_') != NULL) {
					*strchr(cn, '_') = ' ';
				}
	    		ra = tra / (24000.0 / 360.0);
	    		dec = tdec / (9000.0 / 90.0);
	    		Prd(ra, dec);
				axform(dec, ra, &vx, &vy, &vis);
				if (vis) {
					if (SorT(AlignConnames)) {
						int iangle;
						double eangle;
	
						vx2 = vx - (br->left + (br->right - br->left) / 2);
						vy2 = vy - (br->top + (br->bottom - br->top) / 2);
						if (vx2 == vy2) {
							eangle = 0;
						} else {
							eangle = rtd(atan2((double) vy2, (double) vx2));
							if (eangle >= 0) {
								eangle -= 90;
							} else {
								eangle += 90;
							}
						}
						eangle = - eangle;
						iangle = (int) (eangle * 10); 
			            cfont = CreateFont(max(15, (br->right - br->left) / 36), 0, iangle, iangle, FW_NORMAL,
			            			FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			            			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS,
			            			NULL);
			            ofont = SelectObject(hDC, cfont);
		            }
					TextOut(hDC, vx, vy, cn, strlen(cn));
					if (SorT(AlignConnames)) {
						SelectObject(hDC, ofont);
						DeleteObject(cfont);
					}
				}
			}
			fclose(fp);
			if (!SorT(AlignConnames)) {			
				SelectObject(hDC, ofont);
				DeleteObject(cfont);
			}
		}
	}

	// Draw constellation outlines, if requested

	if (SorT(ShowConstellations)) {
		FILE *fp = fopen(rstring(IDS_CONLINE_FILENAME), "r");

		if (fp != NULL) {
			char cl[80], cn[10];
			char *vfmt = Format(36);
			unsigned int fra, tra;
			int fdec, tdec;
			double ra2, dec2, cra1, cdec1, cra2, cdec2;
			HPEN cpen, open;

			open = SelectObject(hDC, (cpen = CreatePen(PS_SOLID, 0, RGB(128, 128, 128))));
			while (fgets(cl, sizeof cl, fp) != NULL) {
				cl[strlen(cl) - 1] = EOS;

				if (strlen(cl) == 0 || isspace(cl[0]) || cl[0] == ';') {
					continue;
				}
				sscanf(cl, vfmt, cn, &fra, &fdec, &tra, &tdec);
	    		ra = fra / (24000.0 / 360.0);
	    		dec = fdec / (9000.0 / 90.0);
	    		Prd(ra, dec);
	    		ra2 = tra / (24000.0 / 360.0);
	    		dec2 = tdec / (9000.0 / 90.0);
	    		Prd(ra2, dec2);
	    		altaz(&dec, &ra);
	    		altaz(&dec2, &ra2);
				if (clipr_xform(dec, ra, dec2, ra2, &vx, &vy, &vx2, &vy2, FALSE,
					&cdec1, &cra1, &cdec2, &cra2)) {
					MoveTo(hDC, vx, vy);
					LineTo(hDC, vx2, vy2);
				}
			}
			SelectObject(hDC, open);
			DeleteObject(cpen);
			fclose(fp);
		}
	}

	// Draw constellation boundaries, if requested

	if (SorT(ShowConbounds)) {
		FILE *fp = fopen(rstring(IDS_CONBOUND_FILENAME), "r");

		if (fp != NULL) {
			char cl[80];
			char *vfmt = Format(39);
			unsigned int fra, tra;
			int mdraw, fdec, tdec;
			double ra2, dec2, cra1, cdec1, cra2, cdec2;
			HPEN cpen, open;

			open = SelectObject(hDC, (cpen = CreatePen(PS_SOLID, 0, RGB(0, 128, 0))));
			while (fgets(cl, sizeof cl, fp) != NULL) {
				cl[strlen(cl) - 1] = EOS;

				if (strlen(cl) == 0 || isspace(cl[0]) || cl[0] == ';') {
					continue;
				}
				sscanf(cl, vfmt, &mdraw, &tra, &tdec);
				if (mdraw) {
		    		ra = fra / (24000.0 / 360.0);
		    		dec = fdec / (9000.0 / 90.0);
		    		Prd(ra, dec);

		    		ra2 = tra / (24000.0 / 360.0);
		    		dec2 = tdec / (9000.0 / 90.0);
		    		Prd(ra2, dec2);
		    		if (skyMode == paintHOR) {
		    			int n;
		    			double alt = dec, az = ra, alt2 = dec2, az2 = ra2;
		    			
			    		altaz(&alt, &az);
			    		altaz(&alt2, &az2);
						if (clipr_xform(alt, az, alt2, az2, &vx, &vy, &vx2, &vy2, FALSE,
							&cdec1, &cra1, &cdec2, &cra2)) {
							if (ra == ra2) {
								// No need to curve great circles
								MoveTo(hDC, vx, vy);
								LineTo(hDC, vx2, vy2);
							} else {
								// Make sure we go the short way around the circle
								if ((ra - ra2) > 180.0) {
									ra = ra - 360.0;
								} else if ((ra2 - ra) > 180.0) {
									ra2 = ra2 - 360.0;
								}
								for (n = 0; n < 10; n++) {
									alt = dec + n * ((dec2 - dec) / 10);
									az = ra + n * ((ra2 - ra) / 10);
									alt2 = dec + (n + 1) * ((dec2 - dec) / 10);
									az2 = ra + (n + 1) * ((ra2 - ra) / 10);
						    		altaz(&alt, &az);
						    		altaz(&alt2, &az2);
									if (clipr_xform(alt, az, alt2, az2, &vx, &vy, &vx2, &vy2, FALSE,
										&cdec1, &cra1, &cdec2, &cra2)) {								
										MoveTo(hDC, vx, vy);
										LineTo(hDC, vx2, vy2);
									}
								}
							}
						}
		    		} else {
						if (clipr_xform(dec, ra, dec2, ra2, &vx, &vy, &vx2, &vy2, FALSE,
							&cdec1, &cra1, &cdec2, &cra2)) {
							drawcurveline(cdec1, cra1, cdec2, cra2, vx, vy, vx2, vy2, 0, FALSE, 0);
						}
					}
				}
				fra = tra;
				fdec = tdec;
			}
			SelectObject(hDC, open);
			DeleteObject(cpen);
			fclose(fp);
		}
	}

	// Scan the star map and draw the stars
    
    if (starMapLib == NULL || starMapHandle == NULL || starMapRes == NULL) {
    	smap = NULL;
    	smhand = -1;
    } else {
		smap = LockResource(starMapRes);
	}
	if (smap != NULL || smhand != -1) {
		struct starMapData *smp = smap;
		int imag, erad;
		short ipmra, ipmdec;
		unsigned int ilimag;
		HFONT ofont, cfont, gfont;
		HPEN spen, open;
		HBRUSH sbrush, obrush;
		HGLOBAL scolourH = NULL;
		LPCSTR scolour = NULL;

        cfont = CreateFont(max(15, (br->right - br->left) / 40), 0, 0, 0, FW_NORMAL,
        			FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS,
        			NULL);
        gfont = CreateFont(max(15, (br->right - br->left) / 40), 0, 0, 0, FW_NORMAL,
        			FALSE, FALSE, FALSE, SYMBOL_CHARSET, OUT_DEFAULT_PRECIS,
        			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS,
        			rstring(IDS_GREEK_FONTNAME));
        ofont = SelectObject(hDC, cfont);
    	SetBkMode(hDC, TRANSPARENT);
    	SetTextAlign(hDC, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);
    	if (showStarColours && starQuality != 2) {
	    	scolourH = LoadResource(hInst, FindResource(hInst, "SColour", "StarCat"));
	    	if (scolourH != NULL) {
	    		scolour = LockResource(scolourH);
	    	}
		}
       	SetTextColor(hDC, RGB(255, 128, 128));

        ilimag = (unsigned int) ((limag + 1.5) * 100); // For quick tests against limiting magnitude

		while (TRUE) {
			char *sname;

          	memset(smex, 0, sizeof smex);
          	sname = ((char *) smp) + sizeof(struct starMapData);
          	if (smp->mag & 0x2000) {
          		memcpy(smex, sname, 3);
          		sname += 3;
          	}
          	if (smp->mag & 0x1000) {
          		memcpy(&ipmra, sname, sizeof ipmra);
          		memcpy(&ipmdec, sname + sizeof ipmra, sizeof ipmdec);
          		sname += sizeof ipmra + sizeof ipmdec;
          	} else {
          		ipmra = ipmdec = 0;
          	}
    		ra = smp->lon / (65536.0 / 360.0) +
    			(((faketime - PrecEpoch) / (JulianCentury / 100)) * (ipmra / (1000.0 * 60 * 60)));
			dec = smp->lat / (32767.0 / 90.0) +
				(((faketime - PrecEpoch) / (JulianCentury / 100)) * (ipmdec / (1000.0 * 60 * 60)));
    		Prd(ra, dec);

			axform(dec, ra, &vx, &vy, &vis);
			if (vis) {
				double mbase = 3.5;

	    		mag = ((smp->mag & 0xFFF) / 100.0) - 1.5;
				{
					int named = FALSE;

					/* If star is bright enough to meet the "show name" criterion
					   and it has a name, display it. */

					if ((smp->mag & 0x8000) && SorT(ShowName) && (mag < SorT(NameMag))) {
						SelectObject(hDC, cfont);
	       		     	TextOut(hDC, vx + 8, vy + 2, sname, lstrlen(sname));
	       		     	named = TRUE;
					}

					// See if the Bayer/Flamsteed code should be drawn
                    
					if ((smex[0] != 0) && SorT(ShowBflam) && (mag < SorT(BflamMag))) {
						SelectObject(hDC, smex[0] < 32 ? gfont : cfont);
						if (smex[0] < 32) {
							smex[0] += ('a' - 1);
						}
						TextOut(hDC, vx + (named ? -16 : 8), vy + 2,
							smex, strlen(smex));
					}
					
					// See if the magnitude should be plotted beneath the star
					
                    if (SorT(ShowMag) && (mag >= min(telShowMagMin, telShowMagMax)) &&
                    		(mag <= max(telShowMagMin, telShowMagMax))) {
                    	char maggit[20];
                    	int im = sgn(mag) * (int) ((abs(mag) + 0.05) * 10);
                    	UINT ota;
                    	
						SelectObject(hDC, cfont);
    					ota = SetTextAlign(hDC, TA_CENTER | TA_TOP | TA_NOUPDATECP);
       					SetTextColor(hDC, RGB(192, 192, 192));
                    	wsprintf(maggit, "%d", im);
                    	TextOut(hDC, vx, vy + 2, maggit, strlen(maggit));
	    				SetTextAlign(hDC, ota);
                    } 

					// Finally, paint the star according to its magnitude and the quality setting

					if (starQuality == 0) {
						mbase = 4;
						erad = 0;
					} else if (starQuality == 2) {
						imag = ((int) ELEMENTS(starIcons)) - ((int) (mag - BrightestMag));
						if (limag < (ELEMENTS(starIcons) - 2)) {
							imag -= (ELEMENTS(starIcons) - ((int) limag)) - 1;
						}
						imag = min(max(imag, 0), ELEMENTS(starIcons) - 1);
						DrawIcon(hDC, vx - 16, vy - 16, starIcons[imag]);
					} else {
	                	imag = ((int) (mag + 2.5)) - 1;
						erad = 6 - imag;

						if (erad < 1) {
							vm = (int) (255 - (mag - mbase) * (128 / (8 - mbase)));
							vm = min(255, max(vm, 32));
							if (scolour) {
								LPCSTR rgb = scolour + (3 * smp->spectral);

								SetPixel(hDC, vx, vy, PALETTERGB((vm * rgb[0]) / 255, (vm * rgb[1]) / 255, (vm * rgb[2]) / 255));
							} else {
								SetPixel(hDC, vx, vy, RGB(vm, vm, vm));
							}
						} else {
							if (scolour) {
								LPCSTR rgb = scolour + (3 * smp->spectral);

								sbrush = CreateSolidBrush(PALETTERGB(rgb[0], rgb[1], rgb[2]));
								obrush = SelectObject(hDC, sbrush);
								spen = CreatePen(PS_SOLID, 0, PALETTERGB(rgb[0], rgb[1], rgb[2]));
								open = SelectObject(hDC, spen);
							}
							Ellipse(hDC, vx - erad, vy - erad, vx + erad, vy + erad);
							if (scolour != NULL) {
								SelectObject(hDC, obrush);
								DeleteObject(sbrush);
								SelectObject(hDC, open);
								DeleteObject(spen);
							}
						}
					}
				}
			}

			/* Quit if we hit the end of file or exceed the limiting magnitude.
			   Note the assumption that the catalogue is sorted by magnitude! */

    		if ((smp->mag & 0x4000) || (((unsigned int) (smp->mag & 0xFFF)) > ilimag)) {
    			break;
    		}
    		if (smap != NULL) {
    			smp = (struct starMapData *) (sname + ((smp->mag & 0x8000) ? (strlen(sname) + 1) : 0));
    		}
		}

		SelectObject(hDC, ofont);
		DeleteObject(cfont);
		DeleteObject(gfont);

        if (smap != NULL) {
			UnlockResource(starMapRes);
		} else {
			_lclose(smhand);
		}
		if (scolourH != NULL) {
			if (scolour != NULL) {
				UnlockResource(scolourH);
			}
			FreeResource(scolourH);
		}
	}

	// Plot deep sky objects, if requested

	if (SorT(ShowDeep)) {
		FILE *fp = fopen(rstring(IDS_DEEP_FILENAME), "r");
		char *dcode = rstring(IDS_DEEP_CODES), *vfmt = Format(41);
		HFONT ofont, cfont;

        cfont = CreateFont(max(15, (br->right - br->left) / 40), 0, 0, 0, FW_NORMAL,
        			FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS,
        			NULL);
        ofont = SelectObject(hDC, cfont);
    	SetBkMode(hDC, TRANSPARENT);
       	SetTextColor(hDC, PALETTERGB(255, 128, 128));
    	SetTextAlign(hDC, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);

		if (fp != NULL) {
			char cl[132], cn[80], dc;
			double ra, dec, mag;

			while (fgets(cl, sizeof cl, fp) != NULL) {
				cl[strlen(cl) - 1] = EOS;

				if (strlen(cl) == 0 || isspace(cl[0]) || cl[0] == ';') {
					continue;
				}
				if (sscanf(cl, vfmt, &ra, &dc, &dec, &dc, &mag, &dc, cn) < 7) {
					continue;
				}
				if (mag > SorT(DeepMag)) {
					break;
				}
	    		Prd(ra, dec);
				axform(dec, ra, &vx, &vy, &vis);
				if (vis) {
					char *objclass, *popname;
					int i;
					HICON di = NULL;

					popname = strchr(cn, ',');
					if (popname != NULL) {
						*popname++ = EOS;
						if ((objclass = strchr(popname, ',')) != NULL) {
							*strchr(popname, ',') = EOS;
						}
						if (objclass != NULL) {
							objclass++;
							if (strchr(objclass, ',') != NULL) {
								*strchr(objclass, ',') = EOS;
							}
							for (i = 0; i < ELEMENTS(deepIcons); i++) {
								if (strncmp(objclass, dcode + (i * 2), 2) == 0) {
									di = deepIcons[i];
									break;
								}
							}
							if (di != NULL) {
								char *dname = strlen(popname) > 0 ? popname : cn;

								DrawIcon(hDC, vx - 16, vy - 16, di);
	       		     			TextOut(hDC, vx + 8, vy + 2, dname, lstrlen(dname));
							}
						}
					}

				}
			}
			SelectObject(hDC, ofont);
			DeleteObject(cfont);
			fclose(fp);
		}
	}
	
	//	If a user catalogue is selected, plot items in it.
	
	if (userCat[0] != '-') {
		FILE *fp = fopen(userCat, "r");
		char *dcode;
		HFONT ofont, cfont;
		HPEN open, cpen;
		static COLORREF stdColour[10] = {
			RGB(0, 0, 0),
			RGB(255, 0, 0),
			RGB(255, 255, 0),
			RGB(0, 255, 0),
			RGB(0, 255, 255),
			RGB(0, 0, 255),
			RGB(255, 0, 255),
			RGB(255, 255, 255),
			RGB(192, 192, 192),
			RGB(128, 1280, 128)
		};
        
        cpen = CreatePen(PS_SOLID, 0, stdColour[7]);
        open = SelectObject(hDC, cpen);
        cfont = CreateFont(max(15, (br->right - br->left) / 40), 0, 0, 0, FW_NORMAL,
        			FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS,
        			NULL);
        ofont = SelectObject(hDC, cfont);
    	SetBkMode(hDC, TRANSPARENT);
       	SetTextColor(hDC, PALETTERGB(255, 255, 255));
    	SetTextAlign(hDC, TA_LEFT | TA_TOP | TA_NOUPDATECP);

		if (fp != NULL) {
			char cl[350], cn[80];
			double ra, dec, cra1, cdec1, cra2, cdec2, fromRA, fromDec;
			int gotHeader = FALSE, rafield = -1, decfield = -1,
				typefield = -1, labelfield = -1, n = 0;

			while (fgets(cl, sizeof cl, fp) != NULL) {
				cl[strlen(cl) - 1] = EOS;
				if (strlen(cl) == 0 || cl[0] == ';') {
					continue;
				}
				
				
                CSVscanInit(cl);
                if (!gotHeader) {
                	n = 0;
                	while (CSVscanField(cn)) {
                		if (strcmp(cn, rstring(IDS_OC_RA)) == 0) {
                			rafield = n;
                		} else if (strcmp(cn, rstring(IDS_OC_DEC)) == 0) {
                			decfield = n; 
                		} else if (strcmp(cn, rstring(IDS_OC_TYPE)) == 0) {
                			typefield = n; 
                		} else if (strcmp(cn, rstring(IDS_OC_LABEL)) == 0) {
                			labelfield = n;
                		} 
                		n++;	
                	}
                	//	Ditch catalogue if required information isn't present.
                	if (rafield < 0 || decfield < 0 || (typefield < 0 && labelfield < 0)) {
                		break;
                	}
                	dcode = rstring(IDS_DEEP_CODES);
                	gotHeader = TRUE;
                } else {
                	char cra[20], cdec[20], ctype[20], clabel[80];
                
                	n = 0;
                	cra[0] = cdec[0] = ctype[0] = clabel[0] = EOS;
                	while (CSVscanField(cn)) {
                		if (n == rafield) {
                			strcpy(cra, cn);
                		} else if (n == decfield) {
                			strcpy(cdec, cn);
                		} else if (n == typefield) {
                			strcpy(ctype, cn);
                		} else if (n == labelfield) {
                			strcpy(clabel, cn);
                		}
                		n++;
                	}
                    
                    parseRaDec(cra, cdec, &ra, &dec);
		    		Prd(ra, dec);
                    if (strlen(ctype) > 0) {
						int i, f= FALSE;
						char *ct = ctype;
							
						while (strlen(ct) > 0) {
							//	Draw deep space icon if type matches
							for (i = 0; i < ELEMENTS(deepIcons); i++) {
								if (_strnicmp(ct, dcode + (i * 2), 2) == 0) {
									axform(dec, ra, &vx, &vy, &vis);
									if (vis) {
										DrawIcon(hDC, vx - 16, vy - 16, deepIcons[i]);
									}
									f = TRUE;
									break;
								}
							}
							if (!f) {
								char cm[3];
								
								memcpy(cm, ct, 2);
								cm[2] = EOS;
								if (isalpha(cm[0]) && islower(cm[0])) {
									cm[0] = toupper(cm[0]);
								}
								if (isalpha(cm[1]) && islower(cm[1])) {
									cm[1] = toupper(cm[1]);
								}
								
								//	Sn:	Star icon	
								if (cm[0] == 'S' && isdigit(cm[1])) {
									axform(dec, ra, &vx, &vy, &vis);
									if (vis) {
										DrawIcon(hDC, vx - 16, vy - 16, starIcons[cm[1] - '0']);
									}
								//	Pn:	Planet icon
								} else if (cm[0] == 'P' && isdigit(cm[1])) {
									axform(dec, ra, &vx, &vy, &vis);
									if (vis) {
										DrawIcon(hDC, vx - 16, vy - 16, planetIcons[cm[1] - '0']);
									}
								//	Bn:	Satellite (bird) icon
								} else if (cm[0] == 'B' && isdigit(cm[1])) {
									axform(dec, ra, &vx, &vy, &vis);
									if (vis) {
										static char siname[] = "sati?";
										HICON satic;
	
										siname[4] = (char) ('a' + cm[1] - '0');
										satic = LoadIcon(hInst, siname);
										if (satic != NULL) {
											DrawIcon(hDC, vx - 16, vy - 16, satic);
											DestroyIcon(satic);
										}
									}
								//	Hnnn: Scenery (horizon) icon
								} else if (cm[0] == 'H' && isdigit(cm[1])) {
									axform(dec, ra, &vx, &vy, &vis);
									if (vis) {
										WORD icn = atoi(ct + 1);
										HICON ic;
	                                    
	                                    getSceneryIcon(icn, &ic);
										if (ic != NULL) {
											DrawIcon(hDC, vx - 16, vy - 16, ic);
											DestroyIcon(ic);
										}
										while (isdigit(ct[2])) {
											ct++;
										}
									}
								//	Cn:	Set draw colour to n
								} else if (cm[0] == 'C' && isdigit(cm[1])) {
									HPEN npen;
										
									npen = CreatePen(PS_SOLID, 0, stdColour[cm[1] - '0']);
									SelectObject(hDC, npen);
									DeleteObject(cpen);
									cpen = npen;
								//	MV:	Move to given location
								} else if (strcmp(cm, "MV") == 0) {
									fromRA = ra;
									fromDec = dec;
								//	LT:	Straight line to given location
								} else if (strcmp(cm, "LT") == 0) {
									double ra1 = fromRA, dec1 = fromDec,
										   ra2 = ra, dec2 = dec;
	    							altaz(&dec1, &ra1);
	    							altaz(&dec2, &ra2);
									if (clipr_xform(dec1, ra1, dec2, ra2, &vx, &vy,
													&vx2, &vy2, FALSE,
													&cdec1, &cra1, &cdec2, &cra2)) {
										MoveTo(hDC, vx, vy);
										LineTo(hDC, vx2, vy2);
										LineTo(hDC, vx2, vy2);
									}
									fromRA = ra;
									fromDec = dec;
								//	CT: Curved line to given location
								} else if (strcmp(cm, "CT") == 0) {
					    			double fra = fixangle(fromRA), fdec = fromDec,
					    				   tra = fixangle(ra), tdec = dec, dra, ddec;
					    			int n, nsteps, hor = (skyMode == paintHOR), ns;
						    		
						    		ns = hor ? 2 : 30;	
					    			if (fra == tra  && fdec == tdec) {
					    				fra = 0;
					    				tra = 360;
					    			} else {
										if ((fra - tra) > 180.0) {
											fra = fra - 360.0;
										} else if ((tra - fra) > 180.0) {
											tra = tra - 360.0;
										}
									}
									nsteps = max(1, max((int) (abs(tra - fra) / ns),
												(int) (abs(tdec - fdec) / ns)));
									ddec = (tdec - fdec) / nsteps;
									dra = (tra - fra) / nsteps;
									for (n = 0; n < nsteps; n++) {
										double z1, a1, z2, a2;
										
										z1 = fdec + n * ddec;
										a1 = fra + n * dra;
										z2 = fdec + (n + 1) * ddec;
										a2 = fra + (n + 1) * dra;
										altaz(&z1, &a1);
										altaz(&z2, &a2); 
										if (clipr_xform(
												z1, a1, z2, a2,
												&vx, &vy, &vx2, &vy2, FALSE,
												&cdec1, &cra1, &cdec2, &cra2)) {
											if (!hor) {
												drawcurveline(cdec1, cra1, cdec2, cra2,
													vx, vy, vx2, vy2, 0, FALSE, 0);
											} else {
												MoveTo(hDC, vx, vy);
												LineTo(hDC, vx2, vy2);
											}
										}
									}
									fromRA = ra;
									fromDec = dec;
								} 
							}
							if (strlen(ct) >= 2 && ct[2] == ';') {
								ct += 3;
							} else {
								break;
							}
						}
					}
						
					if (strlen(clabel) > 0) {
						char *cl = clabel, *ct;
						UINT tam;
						WORD tw, th;
						int tvx, tvy;
						SIZE sz;

#define					ta_left		1
#define					ta_centre	2
#define					ta_right	4
#define					ta_top		8
#define					ta_middle	16
#define					ta_bottom	32
						
							
						// Re-project location as we may not have done it above	
						axform(dec, ra, &vx, &vy, &vis);
						if (vis) {							
							ct = cl;
							if (*cl == '\'') {
								ct++;
								while (TRUE) {
									tam = 0;
									cl++;
									while (*cl) {
										if (*cl == '\'') {
											if (cl[1] == '\'') {
												cl++;		// Skip forced quote
											} else {
												*cl++ = EOS;
												while (*cl) {
													switch (*cl) {
														case '<':
															tam |= ta_left;
															break;
														
														case '>':
															tam |= ta_right;
															break;
															
														case '|':
															tam |= ta_centre;
															break;
															
														case '^':
															tam |= ta_top;
															break;
															
														case '-':
															tam |= ta_middle;
															break;
															
														case '_':
															tam |= ta_bottom;
															break;
													}
													
													if (*cl >= '0' && *cl <= '9') {
														SetTextColor(hDC, stdColour[*cl - '0']);
													} else if (*cl == ';') {
														break;
													}
													cl++;
												}
												if (*cl == ';' || *cl == EOS) {
													break;
												}	
											}
										}
										cl++;
									}
									if ((tam & (ta_left | ta_right | ta_centre)) == 0) {
										tam |= ta_left;
									}						
									if ((tam & (ta_top | ta_middle | ta_bottom)) == 0) {
										tam |= ta_middle;
									} 									
									GetTextExtentPoint32(hDC, ct, strlen(ct), &sz);
									tw = (unsigned short) sz.cx;
									th = (unsigned short) sz.cy;
									if (tam & ta_right) {
										tvx = (vx - tw) - th / 2;
									} else if (tam & ta_centre) {
										tvx = vx - tw / 2;
									} else {
										tvx = vx + th / 2;
									}
									if (tam & ta_middle) {
										tvy = vy - th / 2;
									} else if (tam & ta_bottom) {
										tvy = vy;
									} else {
										tvy = vy - th;
									}
									TextOut(hDC, tvx, tvy, ct, strlen(ct));
									if (*cl == ';' && cl[1] == '\'') {
										ct = cl + 2;
										cl++;
										continue;
									}
									break;
								}
							} else {
								SIZE sz;

								// Default text location
								GetTextExtentPoint32(hDC, ct, strlen(ct), &sz);
								tw = (unsigned short) sz.cx;
								th = (unsigned short) sz.cy;
								vx += th / 2;
								vy -= th / 2;
								TextOut(hDC, vx, vy, clabel, lstrlen(clabel));
       		     			}
       		     		}
       		     	}
				}
			}
			SelectObject(hDC, ofont);
			DeleteObject(cfont);
			SelectObject(hDC, open);
			DeleteObject(cpen);
			fclose(fp);
#undef		ta_left
#undef		ta_centre
#undef		ta_right
#undef		ta_top
#undef		ta_middle
#undef		ta_bottom
		}
	}

	/*  Now paint the Sun, Moon, and planets on the map.  */

    if (SorT(ShowPlanets)) {
		for (i = 0; i <= (aTracked ? 10 : 9); i++) {
			if (((planet_info[i].alt > 0.0) || !skyTel) && ((i < 9) || (planet_info[i].hrv > 0))) {
	    		ra = dtr(planet_info[i].ra);
	    		dec = dtr(planet_info[i].dec);

	    		/* Note that since planetary positions are computed for the equinox
	    		   of the date, they are *not* precessed to the current equinox. */

				axform(rtd(dec), rtd(ra), &vx, &vy, &vis);
				if (!vis) {
					continue;
				}

				if (i == 3 && micon != NULL) {
					// Use the moon icon to show current phase
					DrawIcon(hDC, vx - 16, vy - 13, micon);
				} else {
					DrawIcon(hDC, vx - 16, vy - 16, PlanetIcon(i));
				}
			}
		}
	}

	/* Finally, if we're tracking a satellite and it's above the horizon, draw
	   the satellite icon at the correct point. */

	if (tracking && (satIcon != NULL)) {
		double saz, lha;

		/* Watch it!  Satellite tracking yields azimuths in navigator's terminology:
		   North = 0, East = 90, South = 180, West = 270.  We have to transform this
		   into astronomical South-based azimuth in the process of plotting. */

		saz = satAZ + PI;
		lha = atan2(sin(saz), cos(saz) * cos(dtr(siteLat)) + tan(satEL) * cos(dtr(siteLat)));
		dec = asin(sin(dtr(siteLat)) * sin(satEL) - cos(dtr(siteLat)) * cos(satEL) * cos(saz));
		ra = -(lha - (gmst(satCalcTime) * (PI / 12.0)) + dtr(siteLon));
		axform(rtd(dec), rtd(ra), &vx, &vy, &vis);
		if (vis) {
			DrawIcon(hDC, vx - 16, vy - 16, satIcon);
		}
	}
#undef Prd
}

/*  RELEASESKYBITMAPS  --  Release backing bitmaps for sky map.  */

static void releaseSkyBitmaps(void)
{
	if (smbitmap != NULL) {
		DeleteObject(smbitmap);
		smbitmap = NULL;
	}
}

/*  UPDATESKY  --  Update sky map if needed.  */

void updateSky(double jd, int force)
{
	if ((skyViewPanel != NULL) && (force || NeedToCalculate(sky, jd))) {
		if (force) {
			InvalidateCalculation(sky);
		}
		calcPlanet(jd);
		bitmapValid = FALSE;
		InvalidateRect(skyViewPanel, NULL, FALSE);
		updateTelescope();
		updateHorizon();
	}
}

/*  SKYSATCHANGED  --  Inform sky that satellite selection just changed.  */

void skySatChanged(void)
{
	lastSatX = -2000;
	if (skyViewPanel != NULL) {
		bitmapValid = FALSE;
		InvalidateRect(skyViewPanel, NULL, FALSE);
	}
	updateTelescope();
	updateHorizon();
}

/*  SKYWNDPROC  --  Window procedure for sky map.  */

WindowProc(SkyWndProc)
{
    HDC hDC, hMemDC;
    PAINTSTRUCT ps;         			// holds PAINT information
    static short cxClient, cyClient;    // Window size
    HBITMAP obmap;
    HBRUSH obrush, bbrush;
    HPEN open, bpen;
    HPALETTE opal, mopal;
    HRGN brgn;
    COLORREF skycolour;
    RECT cr;							// Entire client rectangle
    static RECT br;						// Sub-rectangle containing map (save for clicking)
    char tbuf[80];
    static DLGPROC scopeDlgProc = NULL,	// Telescope dialogue procedure
    			   horizonDlgProc = NULL; // Horizon dialogue procedure
	HGLOBAL scolourH = NULL;
	LPCSTR scolour;

    switch (Message) {
        case WM_COMMAND:
         switch (WM_COMMAND_ID(wParam)) {

            case IDM_F_EXIT:
                PostMessage(hWnd, WM_CLOSE, 0, 0L);
                break;

            case IDM_F_COPY:
            	if (smbitmap == NULL) {
            		InvalidateRect(hWnd, NULL, TRUE);
            		UpdateWindow(hWnd);
            	}
            	if (smbitmap != NULL) {
                    HBITMAP hBitmap, obmap, obmap1;

                    hDC = GetDC(hWnd);
                    hBitmap = CreateCompatibleBitmap(hDC, cxClient, cyClient);
                    if (hBitmap != NULL) {
	                    HDC hdcMem, hdcMem1;

	                    hdcMem = CreateCompatibleDC(hDC);
                    	hdcMem1 = CreateCompatibleDC(hDC);
                        obmap = SelectObject(hdcMem, hBitmap);
                        obmap1 = SelectObject(hdcMem1, smbitmap);
                        BitBlt(hdcMem, 0, 0, cxClient, cyClient,
                                hdcMem1, 0, 0, SRCCOPY);
                        OpenClipboard(hWnd);
                        EmptyClipboard();
                        SetClipboardData(CF_BITMAP, hBitmap);
                        CloseClipboard();
                        SelectObject(hdcMem, obmap);
                        SelectObject(hdcMem1, obmap1);
                        DeleteDC(hdcMem);
                        DeleteDC(hdcMem1);
                    }
                    ReleaseDC(hWnd, hDC);
                }
                break;

             case IDM_S_SCOPE:
             	if (scopeDlg == NULL) {
             		if (scopeDlgProc == NULL) {
             			scopeDlgProc = (DLGPROC) MakeProcInstance((FARPROC) TelescopeDlg, hInst);
             		}
                	scopeDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDC_TELESCOPE), hWnd,
                		scopeDlgProc);
             	}
             	break;

             case IDM_S_HORIZON:
             	if (horizonDlg == NULL) {
             		if (horizonDlgProc == NULL) {
             			horizonDlgProc = (DLGPROC) MakeProcInstance((FARPROC) HorizonDlg, hInst);
             		}
                	horizonDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDC_HORIZON), hWnd,
                		horizonDlgProc);
             	}
             	break;

             case IDM_F_OBJCAT:
             	launchObjectCat();
             	break;

             case IDM_S_CONSTELL:
             	skyShowConstellations = !skyShowConstellations;
updSky:             	releaseSkyBitmaps();
             	InvalidateRect(hWnd, NULL, FALSE);
             	break;

             case IDM_S_COORDS:
             	skyShowCoords = !skyShowCoords;
             	goto updSky;

             case IDM_S_PLANETS:
             	skyShowPlanets = !skyShowPlanets;
             	goto updSky;

             case IDM_S_CONBOUNDS:
             	skyShowConbounds = !skyShowConbounds;
             	goto updSky;

             case IDM_S_CONNAMES:
             	skyShowConnames = !skyShowConnames;
             	goto updSky;
             	
             case IDM_S_CONALIGN:
             	skyAlignConnames = !skyAlignConnames;
             	goto updSky;

             case IDM_S_STARNAMES:
             	skyShowName = !skyShowName;
             	goto updSky;

             case IDM_S_BFLAM:
             	skyShowBflam = !skyShowBflam;
             	goto updSky;

             case IDM_S_DEEPSKY:
             	skyShowDeep = !skyShowDeep;
             	goto updSky;

             case IDM_S_STARSLOW:
             case IDM_S_STARSMED:
             case IDM_S_STARSHIGH:
             	starQuality = ((int) wParam) - IDM_S_STARSLOW;
updSkyTel:      updateTelescope();
				updateHorizon();
             	goto updSky;

             case IDM_S_STARCOL:
             	showStarColours = !showStarColours;
             	goto updSkyTel;

             case IDM_S_YALE:
             case IDM_S_SAO:
             	starCatalogue = wParam - IDM_S_YALE;
             	if (starMapRes != NULL) {
        			FreeResource(starMapRes);
        		}
        		if (starMapLib != NULL) {
	            	starMapRes = LoadResource(starMapLib,
	            					starMapHandle = FindResource(starMapLib,
	            						starCatResName[starCatalogue], "STARMAP"));
					if (starMapRes == NULL && starCatalogue > 0) {
            			starMapRes = LoadResource(starMapLib,
            							starMapHandle = FindResource(starMapLib,
            								starCatResName[starCatalogue = 0], "STARMAP"));
					}
	            } else {
	            	starMapRes = NULL;
	            	starMapHandle = NULL;
	            }
             	goto updSkyTel;

             case IDM_S_PREC + PrecAuto:
             case IDM_S_PREC + PrecAlways:
             case IDM_S_PREC + PrecNever:
             	precessionCalculation = wParam - IDM_S_PREC;
             	goto updSkyTel;

             case IDM_HELP:
                WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                			((DWORD) ((LPSTR) rstring(IDS_HELP_ON_SKY))));
             	holped = TRUE;
             	break;

            default:
                return DefWindowProc(hWnd, Message, wParam, lParam);
            }
            break;

        case WM_CREATE:
            hDC = GetDC(hWnd);
            starMapLib = LoadLibrary(rstring(IDS_STARMAPLIB));
            if (((UINT) starMapLib) < 32) {
            	starMapLib = 0;
            	starMapRes = NULL;
            	starMapHandle = NULL;
            } else {
            	starMapRes = LoadResource(starMapLib,
            					starMapHandle = FindResource(starMapLib,
            						starCatResName[starCatalogue], "STARMAP"));
				if (starMapRes == NULL && starCatalogue > 0) {
            		starMapRes = LoadResource(starMapLib,
            						starMapHandle = FindResource(starMapLib,
            							starCatResName[starCatalogue = 0], "STARMAP"));
				}
            }
            loadPlanetIcons();
            ReleaseDC(hWnd, hDC);
            InvalidateCalculation(sky);
            calcPlanet(faketime);
            break;

        case WM_COMPACTING:
        	releaseSkyBitmaps();
        	relFindIndex();					// Rebuild finder index when we next need it
        	break;
        	
        case WM_DESTROY:
        	releaseSkyBitmaps();
        	relFindIndex();					// Object finder index no longer needed
        	if (starMapRes != NULL) {
        		FreeResource(starMapRes);
        	}
        	if (((UINT) starMapLib) > 32) {
        		FreeLibrary(starMapLib);
        	}
        	unloadPlanetIcons();
            if (scopeDlgProc != NULL) {
            	FreeProcInstance((FARPROC) scopeDlgProc);
            	scopeDlgProc = NULL;
            }
            if (horizonDlgProc != NULL) {
            	FreeProcInstance((FARPROC) horizonDlgProc);
            	horizonDlgProc = NULL;
            }
            if (editReturnProc != NULL) {
            	FreeProcInstance((FARPROC) editReturnProc);
            	editReturnProc = NULL;
            }
            if (editTabReturnProc != NULL) {
            	FreeProcInstance((FARPROC) editTabReturnProc);
            	editTabReturnProc = NULL;
            }
        	skyViewPanel = NULL;
        	if (starPal != NULL) {
        		DeleteObject(starPal);
        		starPal = NULL;
        	}
        	break;

		case WM_GETMINMAXINFO:
			{	MINMAXINFO FAR *mmi = (MINMAXINFO FAR *) lParam;
				RECT sr;
				int mins;
                
                GetWindowRect(GetDesktopWindow(), &sr);
                mins = min(sr.right, sr.bottom);
				mmi->ptMaxSize.x = mins;
				mmi->ptMaxSize.y = mins;
				mmi->ptMinTrackSize.x = mins / 4 + (GetSystemMetrics(SM_CXFRAME) * 2);
				mmi->ptMinTrackSize.y = mins / 4 + (GetSystemMetrics(SM_CYFRAME) * 2) +
										GetSystemMetrics(SM_CYCAPTION) +
										GetSystemMetrics(SM_CYMENU);
			}
			break;
        
        case WM_SIZE:
            cxClient = LOWORD(lParam);
            cyClient = HIWORD(lParam);
        	releaseSkyBitmaps();
            break;

		case WM_ENTERSIZEMOVE:
			draggingSize = TRUE;
			break;

		case WM_EXITSIZEMOVE:
			draggingSize = FALSE;
			InvalidateCalculation(sky);
			bitmapValid = FALSE;
			InvalidateRect(skyViewPanel, NULL, FALSE);
			break;

        case WM_PAINT:
        	sprintf(tbuf, Format(32), siteName);
        	SetWindowText(hWnd, tbuf);
            memset(&ps, 0, sizeof(PAINTSTRUCT));
            hDC = BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &cr);
            cr.right = cr.bottom = min(cr.right, cr.bottom);

			if (smbitmap == NULL) {
				smbitmap = CreateCompatibleBitmap(hDC, cr.right, cr.bottom);
				bitmapValid = FALSE;
			}
			if (smbitmap == NULL) {
				int tx, ty;
				SIZE sz;
				RECT rc;

	    		GetClientRect(hWnd, &rc);
	    		strcpy(tbuf, rstring(idsInsuffSkyMemory));
  				GetTextExtentPoint32(hDC, tbuf, lstrlen(tbuf), &sz);
  				SetBkMode(hDC, TRANSPARENT);
  				tx = (rc.right - sz.cx) / 2; 
  				ty = (rc.bottom - sz.cy) / 2;
	    		FillRect(hDC, &rc, (HBRUSH) GetStockObject(BLACK_BRUSH));
  				SetTextColor(hDC, RGB(255, 255, 255));
  				SetTextAlign(hDC, TA_NOUPDATECP);
  				TextOut(hDC, tx, ty, tbuf, strlen(tbuf));
			} else {
				hMemDC = CreateCompatibleDC(hDC);
	
				if (starPal == NULL) {
#define nSTD	84
#define sCOL	15
			    	scolourH = LoadResource(hInst, FindResource(hInst, "SColour", "StarCat"));
			    	if (scolourH != NULL) {
			    		scolour = LockResource(scolourH);
				    	if (scolour != NULL) {
				    		int i;
				    		LOGPALETTE *lp = (LOGPALETTE *) LocalAlloc(LPTR,
				    			sizeof(LOGPALETTE) + (nSTD + sCOL) * sizeof(PALETTEENTRY));
	
				    		if (lp != NULL) {
#define sC(n, r, g, b)		lp->palPalEntry[nSTD + n].peRed = r; \
								lp->palPalEntry[nSTD + n].peGreen = g; \
								lp->palPalEntry[nSTD + n].peBlue = b; \
								lp->palPalEntry[nSTD + n].peFlags = PC_NOCOLLAPSE
				    			lp->palNumEntries = nSTD + sCOL;
				    			lp->palVersion = 0x300;
				    			for (i = 0; i < nSTD; i++) {
				    				lp->palPalEntry[i].peRed = scolour[3 * i];
				    				lp->palPalEntry[i].peGreen = scolour[3 * i + 1];
				    				lp->palPalEntry[i].peBlue = scolour[3 * i + 2];
				    				lp->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
				    			}
				    			sC(0,   0,   0,   0);
				    			sC(1, 128, 128, 128);
				    			sC(2, 255, 255,   0);
				    			sC(3, 255, 128, 128);
				    			sC(4,   0, 128, 128);
				    			sC(5, 128,   0,   0);
				    			sC(6,   0, 128,   0);
				    			sC(7,   0,   0, 255);
				    			sC(8,   0,   0, 128);
				    			
				    			sC(9, 255,   0,   0);
				    			sC(10,255,   0, 255);
				    			sC(11,  0, 255,   0);
				    			sC(12,  0, 255, 255);
				    			sC(13,255, 255, 255);
				    			sC(14,192, 192, 192);
				    			starPal = CreatePalette(lp);
				    			LocalFree((HLOCAL) lp);
			    			}
			    			UnlockResource(scolourH);
			    		}
			    		FreeResource(scolourH);
			    	}
		    	}
#undef sC
#undef nSTD
		    	if (starPal != NULL) {
		    		opal = SelectPalette(hDC, starPal, FALSE);
		    		RealizePalette(hDC);
		    		mopal = SelectPalette(hMemDC, starPal, FALSE);
		    	}
				obmap = SelectObject(hMemDC, smbitmap);
	
	            if (!bitmapValid) {
				    HCURSOR ocurs;
				    TEXTMETRIC tm;
				    int charoff, brx, bry;
	                DWORD calcstart = GetTickCount();
	
	                Flip = (siteLat >= 0) ? 1 : -1;
	            	GetTextMetrics(hDC, &tm);
	                ocurs = SetCursor(LoadCursor(NULL, IDC_WAIT));
	                ShowCursor(TRUE);
	            	FillRect(hMemDC, &cr, GetStockObject(GRAY_BRUSH));
	            	br = cr;
	
	            	/* Label cardinal points on the map with text and shrink the logical
	            	   map so it doesn't overlap the labels. */
	
	            	charoff = (5 * max(tm.tmHeight + 1, tm.tmMaxCharWidth + 1)) / 4;
	            	br.left += charoff;
	            	br.right -= charoff;
	            	br.top += charoff;
	            	br.bottom -= charoff;
	
	            	SetBkMode(hMemDC, TRANSPARENT);
	            	SetTextColor(hMemDC, RGB(0, 255, 0));
	            	SetTextAlign(hMemDC, TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);
	            	TextOut(hMemDC, (br.left + br.right) / 2, br.top - 1,
	            		rstring(Flip > 0 ? IDS_NORTH : IDS_SOUTH), 1);
	            	SetTextAlign(hMemDC, TA_CENTER | TA_TOP | TA_NOUPDATECP);
	            	TextOut(hMemDC, (br.left + br.right) / 2, br.bottom + 1,
	            		rstring(Flip > 0 ? IDS_SOUTH : IDS_NORTH), 1);
	            	SetTextAlign(hMemDC, TA_CENTER | TA_BASELINE | TA_NOUPDATECP);
	            	TextOut(hMemDC, (cr.left + br.left) / 2,
	            					((br.top + br.bottom) + tm.tmHeight) / 2,
	            		rstring(Flip > 0 ? IDS_EAST : IDS_WEST), 1);
	            	SetTextAlign(hMemDC, TA_CENTER | TA_BASELINE | TA_NOUPDATECP);
	            	TextOut(hMemDC, (cr. right + br.right) / 2,
	            					((br.top + br.bottom) + tm.tmHeight) / 2,
	            		rstring(Flip > 0 ? IDS_WEST : IDS_EAST), 1);
	
	            	// Draw "compass rose" marks for NW, SW, NE, and SE
	
		            bpen = CreatePen(PS_SOLID, 0, RGB(0, 255, 0));
		            brx = (br.right - br.left) / 2;
		            bry = (br.bottom - br.top) / 2;
		            open = SelectObject(hMemDC, bpen);
		            MoveTo(hMemDC, br.left + (1 * brx) / 4,
		            			   br.top + (1 * bry) / 4);
		            LineTo(hMemDC, br.left + (7 * brx) / 4,
		            			   br.top + (7 * bry) / 4);
		            MoveTo(hMemDC, br.left + (1 * brx) / 4,
		            			   br.top + (7 * bry) / 4);
		            LineTo(hMemDC, br.left + (7 * brx) / 4,
		            			   br.top + (1 * bry) / 4);
		            SelectObject(hMemDC, open);
		            DeleteObject(bpen);
	
		            // Determine sky colour based on altitude of Sun
	
		            skycolour = (planet_info[0].alt < -18.0) ?
		            											RGB(0, 0, 0) :                 // Night
		            					      ((planet_info[0].alt > -1) ? RGB(0, 0, 255) :    // Day
		            					        ((planet_info[0].alt > -6) ? RGB(128, 0, 0) :  // Dawn/Sunset
		            					      		RGB(0, 0, 128)                             // Twilight
		            					        )
		            					      );
		            bbrush = CreateSolidBrush(skycolour);
		            bpen = CreatePen(PS_SOLID, 0, skycolour);
		            obrush = SelectObject(hMemDC, bbrush);
		            open = SelectObject(hMemDC, bpen);
	
		            Ellipse(hMemDC, br.left, br.top, br.right, br.bottom);
		            SelectObject(hMemDC, obrush);
		            SelectObject(hMemDC, open);
		            DeleteObject(bbrush);
		            DeleteObject(bpen);
	
		            obrush = SelectObject(hMemDC, GetStockObject(WHITE_BRUSH));
		            open = SelectObject(hMemDC, GetStockObject(WHITE_PEN));
// #define Timing
#ifdef Timing
					{
					DWORD tix = GetTickCount();
					char tstr[80];
#endif
		            /* To obtain precise clipping of objects on the star map at
		               the horizon, we create an elliptical (actually circular)
		               clipping region.  For large screens, this can take up a
		               lot of memory and, when memory is tight, may very well
		               fail.  That isn't the end of the world, however--all it
		               means is that legends may extend slightly outside the
		               circle of the map, so we soldier on without the clipping
		               region if we can't obtain it.  It is also possible that
		               if we successfully obtain the region, the attempt to select
		               it into the device context (which makes a copy of the
		               region) may fail.  Amazingly, or perhaps not to experienced
		               Windows hands, this corrupts the clipping region of the
		               HDC and causes nothing to be drawn.  We have to detect
		               this case and explicitly select a NULL clipping region to
		               avoid having a blank map drawn. */
		        
if (!draggingSize) {
		        	brgn = CreateEllipticRgn(br.left, br.top, br.right, br.bottom);        
		        	if (brgn != NULL) {
		        		int status = SelectClipRgn(hMemDC, brgn);
		
		        		DeleteObject(brgn);		// Select makes a copy, so we can delete now
						if (status == ERROR) {        		
							status = SelectClipRgn(hMemDC, NULL);
						}        		
		        	}
		            paintSky(paintSKY, hMemDC, &br, skyLimitMag);
	            	SelectClipRgn(hMemDC, NULL);
#ifdef Timing
					tix = GetTickCount() - tix;
					sprintf(tstr, "Time: %ld", tix);
					SetWindowText(hWnd, tstr);
#endif
		            bitmapValid = TRUE;
}
		            SelectObject(hMemDC, obrush);
		            SelectObject(hMemDC, open);
		            ShowCursor(FALSE);
		            SetCursor(ocurs);
		            skyLast = faketime;
		            skyCalcTime = CalculationInterval((GetTickCount() - calcstart) / 1000.0);
		            skyInterval = max(CalculationInterval(30),
						(skyCalcTime + ((scopeDlg != NULL) ? telCalcTime : 0.0)) * 10);
	            }
	
	            BitBlt(hDC, 0, 0, cr.right, cr.bottom, hMemDC, 0, 0, SRCCOPY);
	            if (starPal != NULL) {
	            	SelectPalette(hMemDC, mopal, FALSE);
	            	SelectPalette(hDC, opal, FALSE);
	            }
	            SelectObject(hMemDC, obmap);
	            DeleteDC(hMemDC);
	        }
            ValidateRgn(hWnd, NULL);

            /* Inform Windows painting is complete */
            EndPaint(hWnd, &ps);
            break;

        case WM_SYSCOLORCHANGE:
            /* Since the system window background and text colour settings
               are used for the open window display, force the window to
               be repainted when the system colours change. */
            InvalidateRect(hWnd, NULL, TRUE);
            break;

		case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
			{	POINT mp;

				mp.x = LOWORD(lParam);
				mp.y = HIWORD(lParam);
				if (mp.x >= br.left && mp.y >= br.top &&
					mp.x <= br.right && mp.y <= br.bottom) {
					double ra, dec;

					numwins = 1;
					mapwin[0] = &skywin;
					initxform(&skywin);
					if (invxform(mp.x, mp.y, &dec, &ra) >= 0) {
						teldec = dec;
						telra = ra;
						// If the telescope is active, aim it to the new position

						if (scopeDlg != NULL) {
							SendMessage(scopeDlg, WM_U_UPDATESCOPE, 0, 0L);
						} else {
        					SendMessage(hWnd, WM_COMMAND, IDM_S_SCOPE, 0L);
						}
					}
				}
			}
			return TRUE;

        case WM_INITMENU:
#define Chekov(menuitem, condition) \
			CheckMenuItem((HMENU) wParam, (menuitem), \
				(condition) ? MF_CHECKED : MF_UNCHECKED)

            EnableMenuItem((HMENU) wParam, IDM_S_SCOPE, scopeDlg == NULL ?
                	MF_ENABLED : (MF_DISABLED | MF_GRAYED));
            EnableMenuItem((HMENU) wParam, IDM_S_HORIZON, horizonDlg == NULL ?
                	MF_ENABLED : (MF_DISABLED | MF_GRAYED));
            EnableMenuItem((HMENU) wParam, IDM_F_OBJCAT, objectCatPanel == NULL ?
            	MF_ENABLED : (MF_DISABLED | MF_GRAYED));
            Chekov(IDM_S_CONSTELL, skyShowConstellations);
            Chekov(IDM_S_COORDS, skyShowCoords);
            Chekov(IDM_S_PLANETS, skyShowPlanets);
            Chekov(IDM_S_CONBOUNDS, skyShowConbounds);
            Chekov(IDM_S_CONNAMES, skyShowConnames);
            Chekov(IDM_S_CONALIGN, skyAlignConnames);
            Chekov(IDM_S_STARNAMES, skyShowName);
            Chekov(IDM_S_BFLAM, skyShowBflam);
            Chekov(IDM_S_DEEPSKY, skyShowDeep);
            Chekov(IDM_S_PREC + PrecAuto, precessionCalculation == PrecAuto);
            Chekov(IDM_S_PREC + PrecAlways, precessionCalculation == PrecAlways);
            Chekov(IDM_S_PREC + PrecNever, precessionCalculation == PrecNever);


			/*	Walk through the star map menu items and enable only
				those corresponding to resources actually present
				in the star map DLL.  This allows supplying a smaller
				"Lite" DLL without the larger catatalogues.  */

            {	int i, present;

            	for (i = IDM_S_YALE; i <= IDM_S_SAO; i++) {
					present = FALSE;

					if (starMapLib != NULL) {
	            		HGLOBAL s = LoadResource(starMapLib,
	            						FindResource(starMapLib,
	            							starCatResName[i - IDM_S_YALE], "STARMAP"));
						if (s != NULL) {
							FreeResource(s);
							present = TRUE;
						}
					}
            		EnableMenuItem((HMENU) wParam, i, present ?
            			MF_ENABLED : (MF_DISABLED | MF_GRAYED));
					if (starCatalogue == (i - IDM_S_YALE) && !present) {
						starCatalogue = 0;
					}
            	}
            }

            Chekov(IDM_S_YALE, starCatalogue == 0);
            Chekov(IDM_S_SAO, starCatalogue == 1);

            Chekov(IDM_S_STARSLOW, starQuality == 0);
            Chekov(IDM_S_STARSMED, starQuality == 1);
            Chekov(IDM_S_STARSHIGH, starQuality == 2);

            Chekov(IDM_S_STARCOL, showStarColours && (starQuality != 2));
            EnableMenuItem((HMENU) wParam, IDM_S_STARCOL, (starQuality != 2) ?
            				MF_ENABLED : (MF_DISABLED | MF_GRAYED));
            break;

        default:
#ifdef NEEDED
        	if (Message == ourHelpMessage) {
                WinHelp(hWnd, rstring(IDS_HELPFILE), HELP_KEY,
                			((DWORD) ((LPSTR) rstring(IDS_HELP_ON_SKY))));
                holped = TRUE;
        		break;
        	}
#endif
            /* For  any  message  for  which  you  don't  specifically
               provide  a  service  routine,  you  should  return  the
               message  to Windows for default message processing.  */
            return DefWindowProc(hWnd, Message, wParam, lParam);
    }
    return 0L;
}

/*  UPDATETELESCOPE  --  Force update of telescope's image.  */

static void updateTelescope(void)
{
	if (scopeDlg != NULL) {
		tvbitmapValid = FALSE;
		InvalidateRgn(scopeDlg, NULL, FALSE);
	}
}

/*  TELOPTSDLG  --  Telescope options dialogue procedure.  */

DialogueProc(TelOptsDlg)
{
	char tbuf[40];

#define Ifld(f, v) sprintf(tbuf, Format(33), v); SetDlgItemText(hDlg, f, tbuf)
#define Fval(f)	(GetDlgItemText(hDlg, f, tbuf, (sizeof tbuf) - 1), atof(tbuf))

    switch (message) {
        case WM_INITDIALOG:
#define Chk(f, v) SendDlgItemMessage(hDlg, f, BM_SETCHECK, v, 0L);
            Chk(IDC_TO_CONOUTL, telShowConstellations);
            Chk(IDC_TO_CONBOUND, telShowConbounds);
            Chk(IDC_TO_CONNAMES, telShowConnames);
            Chk(IDC_TO_SHOWNAME, telShowName);
            Chk(IDC_TO_SHOWBFLAM, telShowBflam);
            Chk(IDC_TO_SHOWMAG, telShowMag); 
            Chk(IDC_TO_SHOWDEEP, telShowDeep);
            Chk(IDC_TO_SOUTHUP, telSouthUp);
		    Chk(IDC_TO_COORDS, telShowCoords);
		    Chk(IDC_TO_SSOBJ, telShowPlanets);
    		Ifld(IDC_TO_NAMEMAGL, telNameMag);
    		Ifld(IDC_TO_BFLAMAG, telBflamMag);
    		Ifld(IDC_TO_MAXMAG, telShowMagMax);
    		Ifld(IDC_TO_MINMAG, telShowMagMin);
    		Ifld(IDC_TO_DEEPMAG, telDeepMag);
#undef Chk
			EnableWindow(skyViewPanel, FALSE);
			EnableWindow(hWndMain, FALSE);
            return TRUE;

		case WM_DESTROY:
			EnableWindow(skyViewPanel, TRUE);
			EnableWindow(hWndMain, TRUE);
			return TRUE;
            
        case WM_COMMAND:
        	switch (wParam) {
        		case IDOK:
#define Chk(f, v) v = (int) SendDlgItemMessage(hDlg, f, BM_GETCHECK, 0, 0L)
					Chk(IDC_TO_CONOUTL, telShowConstellations);
            		Chk(IDC_TO_CONBOUND, telShowConbounds);
            		Chk(IDC_TO_CONNAMES, telShowConnames);
            		Chk(IDC_TO_SHOWNAME, telShowName);
            		Chk(IDC_TO_SHOWBFLAM, telShowBflam);
            		Chk(IDC_TO_SHOWMAG, telShowMag); 
		            Chk(IDC_TO_SHOWDEEP, telShowDeep);
		            Chk(IDC_TO_SOUTHUP, telSouthUp);
		            Chk(IDC_TO_COORDS, telShowCoords);
		            Chk(IDC_TO_SSOBJ, telShowPlanets);
                    telNameMag = Fval(IDC_TO_NAMEMAGL);
                    telBflamMag = Fval(IDC_TO_BFLAMAG);
                    telShowMagMax = Fval(IDC_TO_MAXMAG);
                    telShowMagMin = Fval(IDC_TO_MINMAG);
                    telDeepMag = Fval(IDC_TO_DEEPMAG);
#undef Chk
        			EndDialog(hDlg, TRUE);
        			return TRUE;

            	case IDCANCEL:
                	EndDialog(hDlg, FALSE);
                	return TRUE;

                case IDC_TO_HELP:
                	WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                				((DWORD) ((LPSTR) rstring(IDS_HELP_ON_TELOPTS))));
                	holped = TRUE;
                	return TRUE;
            }
            break;
    }
    return FALSE;
}

/*  EDITRETURN  --  Edit control which notifies parent of return key.  */ 

static WNDPROC lpfnOldEdit = NULL;

LONG WINAPI
editReturn(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	if (message == WM_GETDLGCODE) {
		return DLGC_WANTALLKEYS;		// We want to see Tab, Return, etc.
	}

	if ((message == WM_KEYDOWN && wParam == VK_TAB) ||
		(message == WM_KEYDOWN && wParam == VK_RETURN)) {
		if ((lParam & (1 << 31)) == 0) {
			SendMessage(GetParent(hWnd), WM_COMMAND,
				MAKELONG(GetWindowLong(hWnd, GWL_ID), WM_EDITRETURN),
				(LPARAM) hWnd);
		}
		return 0L;
	} else {
		return CallWindowProc(lpfnOldEdit, hWnd, message, wParam, lParam);
	}
}

LONG WINAPI editTabReturn(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if ((message == WM_CHAR && wParam == VK_TAB) ||
		(message == WM_KEYDOWN && wParam == VK_RETURN)) {
		if ((lParam & (1 << 31)) == 0) {
			SendMessage(GetParent(hWnd), WM_COMMAND,
				MAKELONG(GetWindowLong(hWnd, GWL_ID),
					((wParam == VK_RETURN) ? WM_EDITRETURN : WM_EDITTAB)),
				(LPARAM) hWnd);
		}
		return 0L;
	} else {
		return CallWindowProc(lpfnOldEdit, hWnd, message, wParam, lParam);
	}
}

/*  TELESCOPEDLG  --  Telescope dialogue procedure.  */

DialogueProc(TelescopeDlg)
{
	RECT dw, cw;
	HWND cWnd;
	RECT hbar, vbar;
	static int hbaroff, vbaroff, shbar, svbar, yvbar, xhbar,
			   lastx, lasty, imagex, imagey,
			   minx, miny, chasetail, echanged, dragging = FALSE;
	char tbuf[40];

	switch (message) {
		case WM_INITDIALOG:
			chasetail = FALSE;
#define Ufld(v, f) v = Fval(f)
#define UtelRA() { double m = fixangle(telra) / 15; \
					sprintf(tbuf, Format(34), (int) m, ((int) (m * 60)) % 60, \
										fmod(m * 3600, 60.0)); \
					SetDlgItemText(hDlg, IDC_TE_RA, tbuf); }
					
			//	Subclass edit controls to permit automatic update when they change
			if (editReturnProc == NULL) {
				lpfnOldEdit = (WNDPROC) GetWindowLong(GetDlgItem(hDlg, IDC_TE_FOV), GWL_WNDPROC);
				editReturnProc = (DLGPROC) MakeProcInstance((FARPROC) editReturn, hInst);
			}
			if (editTabReturnProc == NULL) {
				editTabReturnProc = (DLGPROC) MakeProcInstance((FARPROC) editTabReturn, hInst);
			}
			SetWindowLong(GetDlgItem(hDlg, IDC_TE_RA), GWL_WNDPROC, (LONG) editTabReturnProc);
			SetWindowLong(GetDlgItem(hDlg, IDC_TE_DEC), GWL_WNDPROC, (LONG) editTabReturnProc);
			SetWindowLong(GetDlgItem(hDlg, IDC_TE_FOV), GWL_WNDPROC, (LONG) editReturnProc);
			SetWindowLong(GetDlgItem(hDlg, IDC_TE_POWER), GWL_WNDPROC, (LONG) editReturnProc);
			SetWindowLong(GetDlgItem(hDlg, IDC_TE_LIMAG), GWL_WNDPROC, (LONG) editReturnProc);
			
			UtelRA();
			Ifld(IDC_TE_DEC, teldec);
			Ifld(IDC_TE_POWER, telpower);
			telfov = Power1FOV / telpower;
			Ifld(IDC_TE_FOV, telfov);
			Ifld(IDC_TE_LIMAG, telimag);
		    SetScrollRange(GetDlgItem(hDlg, IDC_TE_VSCROLL), SB_CTL, -90, 90, FALSE);
		    SetScrollRange(GetDlgItem(hDlg, IDC_TE_HSCROLL), SB_CTL, 0, 360, FALSE);
			SetScrollPos(GetDlgItem(hDlg, IDC_TE_VSCROLL), SB_CTL, (int) teldec, TRUE);
			SetScrollPos(GetDlgItem(hDlg, IDC_TE_HSCROLL), SB_CTL, (int) telra, TRUE);
			chasetail = TRUE;
			GetWindowRect(hDlg, &dw);
			minx = dw.right - dw.left;
			miny = dw.bottom - dw.top;
			GetClientRect(hDlg, &dw);
			lastx = dw.right;
			lasty = dw.bottom;
			GetWindowRect(GetDlgItem(hDlg, IDC_TE_PANEL), &cw);
			GetWindowRect(GetDlgItem(hDlg, IDC_TE_HSCROLL), &hbar);
			GetWindowRect(GetDlgItem(hDlg, IDC_TE_VSCROLL), &vbar);
			svbar = vbar.right - vbar.left;
			shbar = hbar.bottom - hbar.top;
			ScreenToClient(hDlg, (POINT FAR *) &(hbar.left));
			ScreenToClient(hDlg, (POINT FAR *) &(vbar.left));
			hbaroff = dw.bottom - hbar.top;
			vbaroff = dw.right - vbar.left;
			ScreenToClient(hDlg, (POINT FAR *) &(cw.left));
			ScreenToClient(hDlg, (POINT FAR *) &(cw.right));
			imagex = cw.right - cw.left;
			imagey = cw.bottom - cw.top;
			yvbar = cw.top;
			xhbar = cw.left;
			telActive = TRUE;
			updateScopeDDE();
   			return TRUE;

		case WM_CLOSE:
			DestroyWindow(hDlg);
			return TRUE;

		case WM_DESTROY:
			if (tvbitmap != NULL) {
				DeleteObject(tvbitmap);
				tvbitmap = NULL;
			}
			scopeDlg = NULL;
			telActive = FALSE;
			updateScopeDDE();
			break;

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
						cw.top += (cheight - lasty);							\
						SetWindowPos(cWnd, NULL, cw.left, cw.top, 0, 0,			\
							SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER)

                floatChild(IDC_TE_OPTIONS);
				floatChild(IDC_TE_HELP);
				floatChild(IDCANCEL);
                if (tvbitmap != NULL) {
                	DeleteObject(tvbitmap);
                	tvbitmap = NULL;
                }
				// Resize the image area
				SetWindowPos(GetDlgItem(hDlg, IDC_TE_PANEL), NULL, 0, 0,
							imagex + (cwidth - lastx), imagey + (cheight - lasty),
							SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
				imagex += (cwidth - lastx);
				imagey += (cheight - lasty);
				/* ...then move the scroll bars to the edges of the dialogue and
				   resize them so they're as wide and high as the image area. */
				SetWindowPos(GetDlgItem(hDlg, IDC_TE_VSCROLL), NULL,
							cwidth - vbaroff, yvbar,
							svbar,
							imagey,
							SWP_NOACTIVATE | SWP_NOZORDER);
				SetWindowPos(GetDlgItem(hDlg, IDC_TE_HSCROLL), NULL,
							xhbar, cheight - hbaroff,
							imagex,
							shbar,
							SWP_NOACTIVATE | SWP_NOZORDER);
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
			    HDC hDC, hMemDC;
			    HWND hWnd = GetDlgItem(hDlg, IDC_TE_PANEL);
    			HBITMAP obmap;
    			HBRUSH obrush;
    			HPEN open;
    			HRGN brgn;
    			HPALETTE opal, mopal;
			    RECT cr;
			    HCURSOR ocurs;

                (void) BeginPaint(hDlg, &ps);
                ocurs = SetCursor(LoadCursor(NULL, IDC_WAIT));
                ShowCursor(TRUE);
                hDC = GetDC(hWnd);
            	GetClientRect(hWnd, &cr);
				if (dragging) {
					FillRect(hDC, &cr, (HBRUSH) GetStockObject(BLACK_BRUSH));
				} else {
					if (tvbitmap == NULL) {
						tvbitmap = CreateCompatibleBitmap(hDC, cr.right, cr.bottom);
						tvbitmapValid = FALSE;
					}
					if (tvbitmap == NULL) {
						int tx, ty;
						SIZE sz;
						
		    			strcpy(tbuf, rstring(idsInsuffTelMemory));
	  					GetTextExtentPoint32(hDC, tbuf, lstrlen(tbuf), &sz);
	  					SetBkMode(hDC, TRANSPARENT);
	  					tx = (cr.right - sz.cx) / 2; 
	  					ty = (cr.bottom - sz.cy) / 2;
		    			FillRect(hDC, &cr, (HBRUSH) GetStockObject(BLACK_BRUSH));
	  					SetTextColor(hDC, RGB(255, 255, 255));
	  					SetTextAlign(hDC, TA_NOUPDATECP);
	  					TextOut(hDC, tx, ty, tbuf, strlen(tbuf));
					} else {
						hMemDC = CreateCompatibleDC(hDC);
			    		if (starPal != NULL) {
			    			opal = SelectPalette(hDC, starPal, FALSE);
			    			RealizePalette(hDC);
			    			mopal = SelectPalette(hMemDC, starPal, FALSE);
			    		}
						obmap = SelectObject(hMemDC, tvbitmap);
		
						if (!tvbitmapValid) {
		            		DWORD calcstart = GetTickCount();
							
							calcAltAz(teldec, telra, &telalt, &telazi);
							updateScopeDDE();
		            		FillRect(hMemDC, &cr, GetStockObject(BLACK_BRUSH));
							obrush = SelectObject(hMemDC, GetStockObject(WHITE_BRUSH));
							open = SelectObject(hMemDC, GetStockObject(WHITE_PEN));
		            		brgn = CreateRectRgn(cr.left, cr.top, cr.right, cr.bottom);
		            		if (brgn != NULL) {
		            			SelectClipRgn(hMemDC, brgn);
		            		}
							paintSky(paintTEL, hMemDC, &cr, telimag);
		            		if (brgn != NULL) {
		            			SelectClipRgn(hMemDC, NULL);
		            			DeleteObject(brgn);
		            		}
							SelectObject(hMemDC, obrush);
							SelectObject(hMemDC, open);
							SetScrollPos(GetDlgItem(hDlg, IDC_TE_VSCROLL), SB_CTL,
			            		((int) teldec), TRUE);
							SetScrollPos(GetDlgItem(hDlg, IDC_TE_HSCROLL), SB_CTL,
			            		((int) telra), TRUE);
							tvbitmapValid = TRUE;
		            		telCalcTime = CalculationInterval((GetTickCount() - calcstart) / 1000.0);
		            		skyInterval = max(CalculationInterval(30), (skyCalcTime + telCalcTime) * 10);
						}
		
						BitBlt(hDC, 0, 0, cr.right, cr.bottom, hMemDC, 0, 0, SRCCOPY);
						if (starPal != NULL) {
		            		SelectPalette(hMemDC, mopal, FALSE);
		            		SelectPalette(hDC, opal, FALSE);
						}
						SelectObject(hMemDC, obmap);
	            		DeleteDC(hMemDC);
					}
				}
	            ReleaseDC(hWnd, hDC);
	            ValidateRgn(hWnd, NULL);
	            ShowCursor(FALSE);
	            SetCursor(ocurs);
				EndPaint(hDlg, &ps);
			}
			return TRUE;

		case WM_VSCROLL:
			switch (LOWORD(wParam)) {
				case SB_BOTTOM:
					teldec = 90;
					break;

				case SB_TOP:
					teldec = -90;
					break;

				case SB_THUMBPOSITION:
				case SB_THUMBTRACK:
					teldec = (short) HIWORD(wParam);
					break;

				case SB_LINEDOWN:
					teldec += telfov / 10;
					if (teldec > 90) {
						teldec = 90;
					}
					SetScrollPos((HWND) lParam, SB_CTL, (int) teldec, TRUE);
					break;

				case SB_LINEUP:
					teldec -= telfov / 10;
					if (teldec < -90) {
						teldec = -90;
					}
					SetScrollPos((HWND) lParam, SB_CTL, (int) teldec, TRUE);
					break;

				case SB_PAGEDOWN:
					teldec += telfov / 2;
					if (teldec > 90) {
						teldec = 90;
					}
					SetScrollPos((HWND) lParam, SB_CTL, (int) teldec, TRUE);
					break;

				case SB_PAGEUP:
					teldec -= telfov / 2;
					if (teldec < -90) {
						teldec = -90;
					}
					SetScrollPos((HWND) lParam, SB_CTL, (int) teldec, TRUE);
					break;

				case SB_ENDSCROLL:
					updateTelescope();
					break;
			}
			Ifld(IDC_TE_DEC, teldec);
			return 0L;

		case WM_HSCROLL:
			switch (LOWORD(wParam)) {
				case SB_LEFT:
					telra = 0;
					break;

				case SB_RIGHT:
					telra = 360;
					break;

				case SB_THUMBPOSITION:
				case SB_THUMBTRACK:
					telra = (short) HIWORD(wParam);
					break;

				case SB_LINELEFT:
					telra -= telfov / 10;
					telra = fixangle(telra);
					SetScrollPos((HWND) lParam, SB_CTL, ((int) telra), TRUE);
					break;

				case SB_LINERIGHT:
					telra += telfov / 10;
					telra = fixangle(telra);
					SetScrollPos((HWND) lParam, SB_CTL, ((int) telra), TRUE);
					break;

				case SB_PAGELEFT:
					telra -= telfov / 2;
					telra = fixangle(telra);
					SetScrollPos((HWND) lParam, SB_CTL, ((int) telra), TRUE);
					break;

				case SB_PAGERIGHT:
					telra += telfov / 2;
					telra = fixangle(telra);
					SetScrollPos((HWND) lParam, SB_CTL, ((int) telra), TRUE);
					break;

				case SB_ENDSCROLL:
					updateTelescope();
					break;
			}
			chasetail = FALSE;
			UtelRA();
			chasetail = TRUE;
			return 0L;

		case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
			{	POINT mp;

				mp.x = LOWORD(lParam);
				mp.y = HIWORD(lParam);
				MapWindowPoints(hDlg, GetDlgItem(hDlg, IDC_TE_PANEL), &mp, 1);
				{
					double ra, dec;
					numwins = 1;
					mapwin[0] = &telwin;
					if (invxform(mp.x, mp.y, &dec, &ra) >= 0) {
						teldec = dec;
						telra = ra;
						chasetail = FALSE;
						UtelRA();
						Ifld(IDC_TE_DEC, teldec);
						chasetail = TRUE;
						updateTelescope();
					}
				}
			}
			return TRUE;

		case WM_RBUTTONDOWN:
			{	POINT mp;
				double ra, dec;

				mp.x = LOWORD(lParam);
				mp.y = HIWORD(lParam);
				MapWindowPoints(hDlg, GetDlgItem(hDlg, IDC_TE_PANEL), &mp, 1);
				numwins = 1;
				mapwin[0] = &telwin;
				if (invxform(mp.x, mp.y, &dec, &ra) >= 0) {
					int bcat, bobj;
					double sep;

					if (findObject(ra, dec, wParam & (MK_CONTROL | MK_SHIFT),
								   &bcat, &bobj, &sep)) {
						pointObjectCat(bcat, bobj);
					}
				}
			}
			return TRUE;

		case WM_U_UPDATESCOPE:			// Update scope to new telra, teldec
            UtelRA();
        	chasetail = FALSE;
			Ifld(IDC_TE_DEC, teldec);
        	chasetail = TRUE;
        	updateTelescope();
			return TRUE;

        case WM_COMMAND:
            switch (WM_COMMAND_ID(wParam)) {

                case IDCANCEL:
                    SendMessage(hDlg, WM_CLOSE, 0, 0L);
                    return TRUE;

                case IDC_TE_HELP:
                	WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                				((DWORD) ((LPSTR) rstring(IDS_HELP_ON_SCOPE))));
                	holped = TRUE;
                	return TRUE;

                case IDC_TE_OPTIONS:
	               {
	                      DLGPROC lpfnMsgProc;

	                      lpfnMsgProc = (DLGPROC) MakeProcInstance((FARPROC) TelOptsDlg, hInst);
	                      if (DialogBox(hInst, MAKEINTRESOURCE(IDC_TELOPTS), hDlg, lpfnMsgProc)) {
	                      	updateTelescope();
	                      }
	                      FreeProcInstance((FARPROC) lpfnMsgProc);
	                }
                	return TRUE;

                case IDC_TE_RA:
                	if (chasetail) {
                		WORD mtype = HIWORD(wParam);
                		 
	                	if (mtype == EN_SETFOCUS) {
	                		echanged = FALSE;
	                	} else if (mtype == EN_CHANGE) {
	                		echanged = TRUE;
	                	} else if (((mtype == WM_EDITRETURN || mtype == WM_EDITTAB) ||
	                			   ((mtype == EN_KILLFOCUS) && echanged))) {
							GetDlgItemText(hDlg, IDC_TE_RA, tbuf, (sizeof tbuf) - 1);
							if (strchr(tbuf, 'h') != NULL ||
								strchr(tbuf, 'H') != NULL) {
								double hh = 0, mm = 0, ss = 0;
								char c1, c2;
	
								sscanf(tbuf, Format(35), &hh, &c1, &mm, &c2, &ss);
								telra = (hh + (mm / 60) + (ss / 3600)) * 15;
							} else {
								telra = atof(tbuf);
							}
							telra = fixangle(telra);
							if (mtype != WM_EDITTAB || (GetKeyState(VK_SHIFT) < 0)) {
								if (echanged) {
									updateTelescope();
								}
		                		echanged = FALSE;
								if (mtype == WM_EDITTAB) {
									SetFocus(GetDlgItem(hDlg, IDC_TE_LIMAG));
								}
							} else {
								int sec = echanged;
								chasetail = FALSE;
								SetFocus(GetDlgItem(hDlg, IDC_TE_DEC));
								chasetail = TRUE;
								echanged = sec;
							}
						}
					}
                	return TRUE;

                case IDC_TE_DEC:
                	if (chasetail) {
                		WORD mtype = HIWORD(wParam);
                		 
	                	if (mtype == EN_SETFOCUS) {
	                		echanged = FALSE;
	                	} else if (mtype == EN_CHANGE) {
	                		echanged = TRUE;
	                	} else if (((mtype == WM_EDITRETURN || mtype == WM_EDITTAB) ||
	                			   ((mtype == EN_KILLFOCUS) && echanged))) {
							GetDlgItemText(hDlg, IDC_TE_DEC, tbuf, (sizeof tbuf) - 1);
							if (strchr(tbuf, 'd') != NULL ||
								strchr(tbuf, 'D') != NULL ||
								strchr(tbuf, '°') != NULL) {
								double dd = 0, mm = 0, ss = 0;
								char c1, c2;
	
								sscanf(tbuf, Format(35), &dd, &c1, &mm, &c2, &ss);
								teldec = sgn(dd) * (abs(dd) + (mm / 60) + (ss / 3600));
							} else {
								teldec = atof(tbuf);
							}
							if (mtype != WM_EDITTAB || !(GetKeyState(VK_SHIFT) < 0)) {
								if (echanged) {
									updateTelescope();
								}
		                		echanged = FALSE;
								if (mtype == WM_EDITTAB) {
									SetFocus(GetDlgItem(hDlg, IDC_TE_POWER));
								}
							} else {
								int sec = echanged;
								chasetail = FALSE;
								SetFocus(GetDlgItem(hDlg, IDC_TE_RA));
								chasetail = TRUE;
								echanged = sec;
							}
						}
					}
                	return TRUE;

                case IDC_TE_POWER:
                	if (chasetail) {
	                	if (HIWORD(wParam) == EN_SETFOCUS) {
	                		echanged = FALSE;
	                	} else if (HIWORD(wParam) == EN_CHANGE) {
	                		echanged = TRUE;
	                	} else if (((HIWORD(wParam) == WM_EDITRETURN) ||
	                			   ((HIWORD(wParam) == EN_KILLFOCUS) && echanged))) {
	                		echanged = FALSE;
		                	Ufld(telpower, IDC_TE_POWER);
		                	if (telpower <= 0) {
		                		telpower = 1;
		                	}
	                		chasetail = FALSE;
		                	telfov = Power1FOV / telpower;
							Ifld(IDC_TE_FOV, telfov);
	                		chasetail = TRUE;
							updateTelescope();
	                	}
                	}
                	return TRUE;

                case IDC_TE_FOV:
                	if (chasetail) {
	                	if (HIWORD(wParam) == EN_SETFOCUS) {
	                		echanged = FALSE;
	                	} else if (HIWORD(wParam) == EN_CHANGE) {
	                		echanged = TRUE;
	                	} else if (((HIWORD(wParam) == WM_EDITRETURN) ||
	                			   ((HIWORD(wParam) == EN_KILLFOCUS) && echanged))) {
	                		echanged = FALSE;
		                	Ufld(telfov, IDC_TE_FOV);
		                	if (telfov <= 0) {
		                		telfov = 180;
		                	}
		            		chasetail = FALSE;
		                	telpower = Power1FOV / telfov;
							Ifld(IDC_TE_POWER, telpower);
		            		chasetail = TRUE;
		            		updateTelescope();
		            	}
	            	}
                	return TRUE;

                case IDC_TE_LIMAG:
                	if (chasetail) {
	                	if (HIWORD(wParam) == EN_SETFOCUS) {
	                		echanged = FALSE;
	                	} else if (HIWORD(wParam) == EN_CHANGE) {
	                		echanged = TRUE;
	                	} else if (((HIWORD(wParam) == WM_EDITRETURN) ||
	                			   ((HIWORD(wParam) == EN_KILLFOCUS) && echanged))) {
	                		echanged = FALSE;
		                	Ufld(telimag, IDC_TE_LIMAG);
		                	updateTelescope();
		                }
	                }
                	return TRUE;

               default:
                	break;
            }
            break;

        default:
        	break;

	}
	return FALSE;
#undef Ufld	
}

/*  UPDATEHORIZON  --  Force update of horizon image.  */

static void updateHorizon(void)
{
	if (horizonDlg != NULL) {
		hobitmapValid = FALSE;
		InvalidateRgn(horizonDlg, NULL, FALSE);
	}
}

/*  HOROPTSDLG  --  Horizon options dialogue procedure.  */

DialogueProc(HorOptsDlg)
{
	char tbuf[40];

#define Ifld(f, v) sprintf(tbuf, Format(33), v); SetDlgItemText(hDlg, f, tbuf)
#define Fval(f)	(GetDlgItemText(hDlg, f, tbuf, (sizeof tbuf) - 1), atof(tbuf))

    switch (message) {
        case WM_INITDIALOG:
#define Chk(f, v) SendDlgItemMessage(hDlg, f, BM_SETCHECK, v, 0L);
            Chk(IDC_TO_CONOUTL, horShowConstellations);
            Chk(IDC_TO_CONBOUND, horShowConbounds);
            Chk(IDC_TO_CONNAMES, horShowConnames);
            Chk(IDC_TO_SHOWNAME, horShowName);
            Chk(IDC_TO_SHOWBFLAM, horShowBflam);
            Chk(IDC_TO_SHOWDEEP, horShowDeep);
		    Chk(IDC_TO_COORDS, horShowCoords);
		    Chk(IDC_TO_SSOBJ, horShowPlanets);
		    Chk(IDC_HO_TERRHOR, horShowTerrain);
		    Chk(IDC_HO_SCENERY, horShowScenery); 
    		Ifld(IDC_TO_NAMEMAGL, horNameMag);
    		Ifld(IDC_TO_BFLAMAG, horBflamMag);
    		Ifld(IDC_TO_DEEPMAG, horDeepMag);
    		Ifld(IDC_HO_TERROUGH, horSceneryRoughness);
#undef Chk
			EnableWindow(skyViewPanel, FALSE);
			EnableWindow(hWndMain, FALSE);
            return TRUE;
            
		case WM_DESTROY:
			EnableWindow(skyViewPanel, TRUE);
			EnableWindow(hWndMain, TRUE);
			return TRUE;

        case WM_COMMAND:
        	switch (WM_COMMAND_ID(wParam)) {
        		case IDOK:
#define Chk(f, v) v = (int) SendDlgItemMessage(hDlg, f, BM_GETCHECK, 0, 0L)
					Chk(IDC_TO_CONOUTL, horShowConstellations);
            		Chk(IDC_TO_CONBOUND, horShowConbounds);
            		Chk(IDC_TO_CONNAMES, horShowConnames);
            		Chk(IDC_TO_SHOWNAME, horShowName);
            		Chk(IDC_TO_SHOWBFLAM, horShowBflam);
		            Chk(IDC_TO_SHOWDEEP, horShowDeep);
		            Chk(IDC_TO_COORDS, horShowCoords);
		            Chk(IDC_TO_SSOBJ, horShowPlanets);
		    		Chk(IDC_HO_TERRHOR, horShowTerrain);
		    		Chk(IDC_HO_SCENERY, horShowScenery); 
                    horNameMag = Fval(IDC_TO_NAMEMAGL);
                    horBflamMag = Fval(IDC_TO_BFLAMAG);
                    horDeepMag = Fval(IDC_TO_DEEPMAG);
                    horSceneryRoughness = Fval(IDC_HO_TERROUGH);
                    horSceneryRoughness = max(0, min(horSceneryRoughness, 1.75));
#undef Chk
        			EndDialog(hDlg, TRUE);
        			return TRUE;

            	case IDCANCEL:
                	EndDialog(hDlg, FALSE);
                	return TRUE;

                case IDC_TO_HELP:
                	WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                				((DWORD) ((LPSTR) rstring(IDS_HELP_ON_HOROPTS))));
                	holped = TRUE;
                	return TRUE;
            }
            break;
    }
    return FALSE;
}

/*  HORIZONDLG  --  Horizon dialogue procedure.  */

DialogueProc(HorizonDlg)
{
	RECT dw, cw;
	HWND cWnd;
	RECT hbar;
	static int hbaroff, shbar, xhbar,
			   lastx, lasty, imagex, imagey,
			   minx, miny, chasetail, i, dragging = FALSE;
	char tbuf[80];

	switch (message) {
		case WM_INITDIALOG:
			chasetail = FALSE;
#define Ufld(v, f) v = Fval(f)
#define UhorAzi() { sprintf(tbuf, Format(45), horazi); \
					SetDlgItemText(hDlg, IDC_HO_DEGREES, tbuf); \
					i = (int) (horazi * 10); \
					SendDlgItemMessage(hDlg, IDC_HO_CARDINAL, CB_SETCURSEL, \
						(((i % 225) == 0) ? (i / 225) : -1), 0L); }

			UhorAzi();
			if (editReturnProc == NULL) {
				lpfnOldEdit = (WNDPROC) GetWindowLong(GetDlgItem(hDlg, IDC_HO_FOV), GWL_WNDPROC);
				editReturnProc = (DLGPROC) MakeProcInstance((FARPROC) editReturn, hInst);
			}
			SetWindowLong(GetDlgItem(hDlg, IDC_HO_FOV), GWL_WNDPROC, (LONG) editReturnProc);
			SetWindowLong(GetDlgItem(hDlg, IDC_HO_DEGREES), GWL_WNDPROC, (LONG) editReturnProc);
			SetWindowLong(GetDlgItem(hDlg, IDC_HO_LIMAG), GWL_WNDPROC, (LONG) editReturnProc);
			Ifld(IDC_HO_FOV, horfov);
			Ifld(IDC_HO_LIMAG, horlimag);
		    SetScrollRange(GetDlgItem(hDlg, IDC_HO_HSCROLL), SB_CTL, 0, 360, FALSE);
			SetScrollPos(GetDlgItem(hDlg, IDC_HO_HSCROLL), SB_CTL, (int) horazi, TRUE);
			for (i = 0; i < 16; i++) {
				SendDlgItemMessage(hDlg, IDC_HO_CARDINAL, CB_ADDSTRING,
					0, (LPARAM) ((LPCSTR) rstring(IDS_CARDINAL_POINTS + i)));
			}
			SendDlgItemMessage(hDlg, IDC_HO_CARDINAL, CB_SETCURSEL, 0, 0L);
			chasetail = TRUE;
			GetWindowRect(hDlg, &dw);
			minx = dw.right - dw.left;
			miny = dw.bottom - dw.top;
			GetClientRect(hDlg, &dw);
			lastx = dw.right;
			lasty = dw.bottom;
			GetWindowRect(GetDlgItem(hDlg, IDC_HO_PANEL), &cw);
			GetWindowRect(GetDlgItem(hDlg, IDC_HO_HSCROLL), &hbar);
			shbar = hbar.bottom - hbar.top;
			ScreenToClient(hDlg, (POINT FAR *) &(hbar.left));
			hbaroff = dw.bottom - hbar.top;
			ScreenToClient(hDlg, (POINT FAR *) &(cw.left));
			ScreenToClient(hDlg, (POINT FAR *) &(cw.right));
			imagex = cw.right - cw.left;
			imagey = cw.bottom - cw.top;
			xhbar = cw.left;
			terrainSeeded = FALSE;
   			return TRUE;

		case WM_CLOSE:
			DestroyWindow(hDlg);
			return TRUE;

		case WM_DESTROY:
			if (hobitmap != NULL) {
				DeleteObject(hobitmap);
				hobitmap = NULL;
			}
			horizonDlg = NULL;
			break;

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
						cw.top += (cheight - lasty);							\
						SetWindowPos(cWnd, NULL, cw.left, cw.top, 0, 0,			\
							SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER)

                floatChild(IDC_HO_OPTIONS);
				floatChild(IDC_HO_HELP);
				floatChild(IDCANCEL);
                if (hobitmap != NULL) {
                	DeleteObject(hobitmap);
                	hobitmap = NULL;
                }
				// Resize the image area
				SetWindowPos(GetDlgItem(hDlg, IDC_HO_PANEL), NULL, 0, 0,
							imagex + (cwidth - lastx), imagey + (cheight - lasty),
							SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
				imagex += (cwidth - lastx);
				imagey += (cheight - lasty);
				/* ...then move the scroll bar to the edge of the dialogue and
				   resize it so it's as wide as the image area. */
				SetWindowPos(GetDlgItem(hDlg, IDC_HO_HSCROLL), NULL,
							xhbar, cheight - hbaroff,
							imagex,
							shbar,
							SWP_NOACTIVATE | SWP_NOZORDER);
				lastx = cwidth;
				lasty = cheight;
				terrainSeeded = FALSE;
			}
			return TRUE;

		case WM_ENTERSIZEMOVE:
			dragging = TRUE;
			break;

		case WM_EXITSIZEMOVE:
			dragging = FALSE;
			terrainSeeded = FALSE;
			InvalidateRect(hDlg, NULL, FALSE);
			UpdateWindow(hDlg);
			break;

		case WM_PAINT:
			{	PAINTSTRUCT ps;
			    HDC hDC, hMemDC;
			    HWND hWnd = GetDlgItem(hDlg, IDC_HO_PANEL);
    			HBITMAP obmap;
    			HBRUSH obrush;
    			HPEN open;
    			HRGN brgn;
    			HPALETTE opal, mopal;
			    RECT cr;
			    HCURSOR ocurs;

                (void) BeginPaint(hDlg, &ps);
				if (dragging) {
		            HBRUSH bbrush;
		            COLORREF skycolour;

					hDC = GetDC(hWnd);
            		GetClientRect(hWnd, &cr);
					skycolour = (planet_info[0].alt < -18.0) ?
			            										RGB(0, 0, 0) :                 // Night
			            					  ((planet_info[0].alt > -1) ? RGB(0, 0, 255) :    // Day
			            						((planet_info[0].alt > -6) ? RGB(128, 0, 0) :  // Dawn/Sunset
			            					      	RGB(0, 0, 128)                             // Twilight
			            						)
			            					  );
					bbrush = CreateSolidBrush(skycolour);
		            FillRect(hDC, &cr, bbrush);
		            DeleteObject(bbrush);
					ReleaseDC(hWnd, hDC);
				} else {
	        		sprintf(tbuf, Format(46), siteName);
	        		SetWindowText(hDlg, tbuf);
					ocurs = SetCursor(LoadCursor(NULL, IDC_WAIT));
					ShowCursor(TRUE);
					hDC = GetDC(hWnd);
            		GetClientRect(hWnd, &cr);
					if (hobitmap == NULL) {
						hobitmap = CreateCompatibleBitmap(hDC, cr.right, cr.bottom);
						hobitmapValid = FALSE;
					}
					if (hobitmap == NULL) {
						int tx, ty;
						SIZE sz;
						
		    			strcpy(tbuf, rstring(idsInsuffHorMemory));
	  					GetTextExtentPoint32(hDC, tbuf, lstrlen(tbuf), &sz);
	  					SetBkMode(hDC, TRANSPARENT);
	  					tx = (cr.right - sz.cx) / 2; 
	  					ty = (cr.bottom - sz.cy) / 2;
		    			FillRect(hDC, &cr, (HBRUSH) GetStockObject(BLACK_BRUSH));
	  					SetTextColor(hDC, RGB(255, 255, 255));
	  					SetTextAlign(hDC, TA_NOUPDATECP);
	  					TextOut(hDC, tx, ty, tbuf, strlen(tbuf));
					} else {
						hMemDC = CreateCompatibleDC(hDC);
			    		if (starPal != NULL) {
			    			opal = SelectPalette(hDC, starPal, FALSE);
			    			RealizePalette(hDC);
			    			mopal = SelectPalette(hMemDC, starPal, FALSE);
			    		}
						obmap = SelectObject(hMemDC, hobitmap);
		
						if (!hobitmapValid) {
		            		DWORD calcstart = GetTickCount();
		            		HBRUSH bbrush;
		            		COLORREF skycolour;
		
							skycolour = (planet_info[0].alt < -18.0) ?
			            												RGB(0, 0, 0) :                 // Night
			            							  ((planet_info[0].alt > -1) ? RGB(0, 0, 255) :    // Day
			            								((planet_info[0].alt > -6) ? RGB(128, 0, 0) :  // Dawn/Sunset
			            					      			RGB(0, 0, 128)                             // Twilight
			            								)
			            							  );
							bbrush = CreateSolidBrush(skycolour);
		            		FillRect(hMemDC, &cr, bbrush);
		            		DeleteObject(bbrush);
							obrush = SelectObject(hMemDC, GetStockObject(WHITE_BRUSH));
							open = SelectObject(hMemDC, GetStockObject(WHITE_PEN));
		            		brgn = CreateRectRgn(cr.left, cr.top, cr.right, cr.bottom);
		            		if (brgn != NULL) {
		            			SelectClipRgn(hMemDC, brgn);
		            		}
							paintSky(paintHOR, hMemDC, &cr, horlimag);
							
							if (horShowTerrain) {
								drawTerrain(hWnd, hMemDC, faketime, horazi, horSceneryRoughness,
											horShowScenery);
							}
							
							// Label cardinal points
							
							SetTextAlign(hMemDC, TA_CENTER | TA_BOTTOM | TA_NOUPDATECP);
							SetTextColor(hMemDC, RGB(255, 255, 0));
							for (i = 0; i < 16; i++) {
								int x, y, in;
								
								xform(5.0, 360 - (i * 22.5), &x, &y, &in);
								if (in) {
									char *s = rstring(IDS_CARDINAL_ABBR + i);
									
									TextOut(hMemDC, x, cr.bottom - 2, s, strlen(s));
								}
							}
							
		            		if (brgn != NULL) {
		            			SelectClipRgn(hMemDC, NULL);
		            			DeleteObject(brgn);
		            		}
							SelectObject(hMemDC, obrush);
							SelectObject(hMemDC, open);
							chasetail = FALSE;
							SetScrollPos(GetDlgItem(hDlg, IDC_HO_HSCROLL), SB_CTL,
			            		((int) horazi), TRUE);
							chasetail = TRUE;
							hobitmapValid = TRUE;
		            		hoCalcTime = CalculationInterval((GetTickCount() - calcstart) / 1000.0);
		            		skyInterval = max(CalculationInterval(30), (skyCalcTime + hoCalcTime) * 10);
						}
		
						if (starPal != NULL) {
		            		mopal = SelectPalette(hMemDC, starPal, FALSE);
		            		opal = SelectPalette(hDC, starPal, FALSE);
		            		RealizePalette(hDC);
						}
						BitBlt(hDC, 0, 0, cr.right, cr.bottom, hMemDC, 0, 0, SRCCOPY);
						if (starPal != NULL) {
		            		SelectPalette(hMemDC, mopal, FALSE);
		            		SelectPalette(hDC, opal, FALSE);
						}
						SelectObject(hMemDC, obmap);
	            		DeleteDC(hMemDC);
					}
					ReleaseDC(hWnd, hDC);
					ValidateRgn(hWnd, NULL);
					ShowCursor(FALSE);
					SetCursor(ocurs);
				}	
				EndPaint(hDlg, &ps);
			}
			return TRUE;

		case WM_HSCROLL:
			if (chasetail) {
				switch (LOWORD(wParam)) {
					case SB_LEFT:
						horazi = 0;
						break;
	
					case SB_RIGHT:
						horazi = 360;
						break;
	
					case SB_THUMBPOSITION:
					case SB_THUMBTRACK:
						horazi = (short) HIWORD(wParam);
						break;
	
					case SB_LINELEFT:
						horazi = ((int) horazi) - 5;
						horazi = fixangle(horazi);
						SetScrollPos((HWND) lParam, SB_CTL, ((int) horazi), TRUE);
						break;
	
					case SB_LINERIGHT:
						horazi = ((int) horazi) + 5;
						horazi = fixangle(horazi);
						SetScrollPos((HWND) lParam, SB_CTL, ((int) horazi), TRUE);
						break;
	
					case SB_PAGELEFT:
						horazi = ((((int) (horazi * 10)) / 225) * 225 - 225) / 10.0;
						horazi = fixangle(horazi);
						SetScrollPos((HWND) lParam, SB_CTL, ((int) horazi), TRUE);
						break;
	
					case SB_PAGERIGHT:
						horazi = ((((int) (horazi * 10)) / 225) * 225 + 225) / 10.0;
						horazi = fixangle(horazi);
						SetScrollPos((HWND) lParam, SB_CTL, ((int) horazi), TRUE);
						break;
	
					case SB_ENDSCROLL:
						terrainSeeded = FALSE;
						updateHorizon();
						break;
				}
				chasetail = FALSE;
		        SetScrollPos(GetDlgItem(hDlg, IDC_HO_HSCROLL), SB_CTL,
		             ((int) horazi), TRUE);
				UhorAzi();
				chasetail = TRUE;
			}
			return 0L;
			
		// Point telescope at object when user clicks in horizon window

		case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
			{	POINT mp;

				mp.x = LOWORD(lParam);
				mp.y = HIWORD(lParam);
				MapWindowPoints(hDlg, GetDlgItem(hDlg, IDC_HO_PANEL), &mp, 1);
				{
					double az, alt;
					numwins = 1;
					mapwin[0] = &horwin;
					if (invxform(mp.x, mp.y, &alt, &az) >= 0) {
						double lha, ra, dec;
	                    
	                    az = fixangle(180 - az);
	                    lha = atan2(sin(dtr(az)), cos(dtr(az)) * sin(dtr(siteLat)) +
	                    		tan(dtr(alt)) * cos(dtr(siteLat)));
	                    dec = asin(sin(dtr(siteLat)) * sin(dtr(alt)) - cos(dtr(siteLat)) *
	                    		cos(dtr(alt)) * cos(dtr(az)));
	                    ra = fixangle(rtd(lham - lha));
	                    dec = rtd(dec);
						teldec = dec;
						telra = ra;
						// If telescope isn't open, try to launch it
		             	if (scopeDlg == NULL) {
		             		if (scopeDlgProc == NULL) {
		             			scopeDlgProc = (DLGPROC) MakeProcInstance((FARPROC) TelescopeDlg, hInst);
		             		}
		                	scopeDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDC_TELESCOPE), skyViewPanel,
		                		scopeDlgProc);
		             	} else {
							PostMessage(scopeDlg, WM_U_UPDATESCOPE, 0, 0L);
						}
					}
				}
			}
			return TRUE;
			
		// Identify object in object catalogue when user right clicks in horizon window

		case WM_RBUTTONDOWN:
			{	POINT mp;
				double az, alt;

				mp.x = LOWORD(lParam);
				mp.y = HIWORD(lParam);
				MapWindowPoints(hDlg, GetDlgItem(hDlg, IDC_HO_PANEL), &mp, 1);
				numwins = 1;
				mapwin[0] = &horwin;
				if (invxform(mp.x, mp.y, &alt, &az) >= 0) {
					int bcat, bobj;
					double lha, ra, dec, sep;
                    
                    az = fixangle(180 - az);
	                lha = atan2(sin(dtr(az)), cos(dtr(az)) * sin(dtr(siteLat)) +
                    		tan(dtr(alt)) * cos(dtr(siteLat)));
                    dec = asin(sin(dtr(siteLat)) * sin(dtr(alt)) - cos(dtr(siteLat)) *
                    		cos(dtr(alt)) * cos(dtr(az)));
                    ra = fixangle(rtd(lham - lha));
                    dec = rtd(dec);
					if (findObject(ra, dec, wParam & (MK_CONTROL | MK_SHIFT),
								   &bcat, &bobj, &sep)) {
						pointObjectCat(bcat, bobj);
					}
				}
			}
			return TRUE;

        case WM_COMMAND:
            switch (WM_COMMAND_ID(wParam)) {

                case IDCANCEL:
                    SendMessage(hDlg, WM_CLOSE, 0, 0L);
                    return TRUE;

                case IDC_HO_HELP:
                	WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                				((DWORD) ((LPSTR) rstring(IDS_HELP_ON_HORIZON))));
                	holped = TRUE;
                	return TRUE;

                case IDC_HO_OPTIONS:
	               {
	                      DLGPROC lpfnMsgProc;

	                      lpfnMsgProc = (DLGPROC) MakeProcInstance((FARPROC) HorOptsDlg, hInst);
	                      if (DialogBox(hInst, MAKEINTRESOURCE(IDC_HOROPTS), hDlg, lpfnMsgProc)) {
	                      	updateHorizon();
	                      }
	                      FreeProcInstance((FARPROC) lpfnMsgProc);
	                }
                	return TRUE;

                case IDC_HO_DEGREES:
					if (HIWORD(wParam) == WM_EDITRETURN && chasetail) {
						GetDlgItemText(hDlg, IDC_HO_DEGREES, tbuf, (sizeof tbuf) - 1);
						horazi = fixangle(atof(tbuf));
						chasetail = FALSE;
						UhorAzi();
			            SetScrollPos(GetDlgItem(hDlg, IDC_HO_HSCROLL), SB_CTL,
			            	((int) horazi), TRUE);
                        chasetail = TRUE;
						terrainSeeded = FALSE;
						updateHorizon();
					}
                	return TRUE;
                	
                case IDC_HO_CARDINAL:
                	if (HIWORD(wParam) == CBN_CLOSEUP && chasetail) {
                		WORD is = (WORD) SendDlgItemMessage(hDlg, IDC_HO_CARDINAL,
                			CB_GETCURSEL, 0, 0);
                		horazi = is * 22.5;
						chasetail = FALSE;
			            SetScrollPos(GetDlgItem(hDlg, IDC_HO_HSCROLL), SB_CTL,
			            	((int) horazi), TRUE);
						UhorAzi();
                        chasetail = TRUE;
						terrainSeeded = FALSE;
						updateHorizon();
                	}
                	return TRUE;

                case IDC_HO_FOV:
					if (HIWORD(wParam) == WM_EDITRETURN && chasetail) {
	                	Ufld(horfov, IDC_HO_FOV);
	                	horfov = max(0, min(horfov, 120));
						terrainSeeded = FALSE;
	                	updateHorizon();
	                }
                	return TRUE;

                case IDC_HO_LIMAG:
					if (HIWORD(wParam) == WM_EDITRETURN && chasetail) {
	                	Ufld(horlimag, IDC_HO_LIMAG);
						terrainSeeded = FALSE;
	                	updateHorizon();
	                }
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

/*  SELECTSKYOBJECTBIN  --  Point telescope given binary right ascension and declination.  */

void selectSkyObjectBin(double ra, double dec)
{
	telra = ra;
	teldec = dec;
	launchSkyWindow();
	if (scopeDlg != NULL) {
		SendMessage(scopeDlg, WM_U_UPDATESCOPE, 0, 0L);
	} else if (skyViewPanel != NULL) {
		SendMessage(skyViewPanel, WM_COMMAND, IDM_S_SCOPE, 0L);
	}
}

/*  PARSERADEC  --  Parse right ascension and declination specifications.  */

void parseRaDec(char *ra, char *dec, double *fra, double *fdec)
{
	if (strchr(ra, 'h') != NULL ||
		strchr(ra, 'H') != NULL) {
		double hh = 0, mm = 0, ss = 0;
		char c1, c2;

		sscanf(ra, Format(35), &hh, &c1, &mm, &c2, &ss);
		*fra = (hh + (mm / 60) + (ss / 3600)) * 15;
	} else {
		*fra = atof(ra);
	}
	*fra = fixangle(*fra);
	if (strchr(dec, 'd') != NULL ||
		strchr(dec, 'D') != NULL ||
		strchr(dec, '°') != NULL) {
		double dd = 0, mm = 0, ss = 0;
		char c1, c2;

		sscanf(dec, Format(35), &dd, &c1, &mm, &c2, &ss);
		*fdec = sgn(dd) * (abs(dd) + (mm / 60) + (ss / 3600));
	} else {
		*fdec = atof(dec);
	}
}

/*  SELECTSKYOBJECT  --  Point the telescope to an object selected in the
						 object catalogue.  */

void selectSkyObject(char *ra, char *dec)
{
	double telra, teldec;

	if (strlen(ra) > 0 && strlen(dec) > 0) {
    	parseRaDec(ra, dec, &telra, &teldec);
		selectSkyObjectBin(telra, teldec);
	}
}

