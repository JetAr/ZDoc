/*

        Home Planet for Windows

            Definitions

*/

#include <windows.h>
#include <winbase.h>
#include <mmsystem.h>
#ifdef ScreenSaver
#include <scrnsave.h>
#endif
#include <commdlg.h>
#include <shellapi.h>
#include <string.h>
#include <time.h>
#ifndef RC_INVOKED
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <sys/types.h>
#include <sys/timeb.h>
#endif

typedef RGBQUAD *LPRGBQUAD;
#define MoveTo(h, x, y)	MoveToEx((h), (x), (y), NULL)

#define WM_COMMAND_ID(x)	(LOWORD(x))
#define WM_COMMAND_NOTIFY	(HIWORD(wParam))

#ifdef ScreenSaver
#include "Hpssave.h"
#else

//	DLL import libraries

#include "Scenery.h"
#include "Mapbits.h"

//	Resource ID definitions

#include "Resource.h"

//	Module definitions

#include "Elements.h"
#endif

#define Format(n)   rstring(IDS_FORMATS + (n))

//	Definitions for screen saver unique objects

#ifdef ScreenSaver
#define ID_HELP                     110
#endif

//      Intra-application messages

#define WM_ST_NEWDBFILE (WM_USER + 1) // Change satellite database message
#define WM_EDITRETURN	(WM_USER + 2) // Return pressed in edit box
#define WM_EDITTAB		(WM_USER + 3) // Tab pressed in edit box

#define PI 3.14159265358979323846  /* Assume not near black hole nor in
                                      Tennessee */

#define V       (void)

#define EOS     '\0'

//	Frequently used astronomical constants

#define J2000				2451545.0		// Julian day of J2000 epoch
#define JulianCentury		36525.0			// Days in Julian century
#define AstronomicalUnit	149597870.0		// Astronomical unit in kilometres
#define SunSMAX	 (AstronomicalUnit * 1.000001018) // Semi-major axis of Earth's orbit
#define EarthRad			6378.14			// Earth's equatorial radius, km (IAU 1976)
#define LunatBase			2423436.0		/* Base date for E. W. Brown's numbered
											   series of lunations (1923 January 16) */
#define SynMonth			29.53058868		// Synodic month (mean time from new Moon to new Moon)

//	Precession calculation modes

#define PrecAuto		0					// Precess if more then PrecYears from PrecEpoch
#define PrecAlways		1					// Always precess
#define PrecNever		2					// Never correct for precession
#define PrecEpoch		J2000				// All databases are epoch J2000.0
#define PrecYears		25					// Consider databases valid for this time around epoch

/*  Handy mathematical functions  */

#ifdef abs
#undef abs
#endif

/* As an indication of how carefully Microsoft tests software prior
   to shipment, and their deep committment to numerical applications,
   Visual C++ 1.5 (C 8.0) shipped with a library in which tan(x)
   returns the wrong sign for arguments between -90 and 90 degrees
   but only if the math coprocessor is not installed.  Can you believe
   this?  So, define tan(x) as sin(x) / cos(x) to work around this
   unbelievable example of lack of regression testing. */
   
//#define MICROSOFT_CAN_T_COUNT_WORTH_BEANS
#ifdef MICROSOFT_CAN_T_COUNT_WORTH_BEANS
#define tan(x)	(sin(x) / cos(x))
#endif // MICROSOFT_CAN_T_COUNT_WORTH_BEANS 

#define sgn(x) (((x) < 0) ? -1 : ((x) > 0 ? 1 : 0))       // Extract sign
#define abs(x) ((x) < 0 ? (-(x)) : (x))                   // Absolute val
#define fixangle(a) ((a) - 360.0 * (floor((a) / 360.0)))  // Fix angle   
#define fixangr(a)  ((a) - (PI*2) * (floor((a) / (PI*2))))  // Fix angle in radians
#define dtr(x) ((x) * (PI / 180.0))                       // Degree->Radian
#define rtd(x) ((x) / (PI / 180.0))                       // Radian->Degree

#define NeedToCalculate(qty, time)  ((abs((qty##Last) - (time)) > qty##Interval) ? \
                                     (qty##Last = (time), TRUE) : FALSE)
#define InvalidateCalculation(qty)      qty##Last = -1e10
#define ValidateCalculation(qty, time) qty##Last = (time)
#define CalculationInterval(x)  ((x) / (24.0 * 60 * 60))

#define POLLY   100				// Draw with Polylines of 100 segments

//  Determine number of elements in an array

#define ELEMENTS(array) (sizeof(array)/sizeof((array)[0]))

//  Calculate the instaneous free space on the stack and report in a message box

#define ProbeStack() { extern int stackavail(void);						\
                       char s[40];										\
                       sprintf(s, "%d", stackavail());					\
                       MessageBox(hDlg, s, "Free stack", MB_OK); }


//  Assertion checking for Windows

#ifndef NDEBUG
#define assert(x) { if (!(x)) { char erm[128]; wsprintf((LPSTR) erm, \
        (LPSTR) "Assertion %s failed at line %d of file %s.", (LPCSTR) #x, __LINE__,(LPSTR) __FILE__); \
        FatalAppExit(0, (LPCSTR) erm); }}
#else
#define assert(x)
#endif

//	Make sure no debug output remains in production builds: use Honk; for debug MessageBeep(x)

#ifdef NDEBUG
#ifdef OutputDebugString
#undef OutputDebugString
#endif 
#define OutputDebugString(x) &&&&&&&&&}}}}}}}}}}}}}}
#define Honk &&&&&&&&&}}}}}}}}}}}}}}
#else
#define Honk	MessageBeep(MB_ICONEXCLAMATION)
#endif

extern char szString[128];		// Variable to load resource strings
#ifdef ScreenSaver
#define hInst	hMainInstance
#define hWndMain hMainWindow
#undef IsIconic
#define IsIconic(x) FALSE
#else
extern char szAppName[20];		// Class name for the window
#endif
extern HINSTANCE hInst;			// Class instance pointer
extern HWND hWndMain;			// Main window pointer
extern HCURSOR mapCursor;       // Map window current cursor
extern HCURSOR xhairCursor;		// Pick site on map cursor
extern HWND satTrackDlg;        // Satellite tracking control panel
extern HWND sunMoonPanel;       // Sun and moon information panel
extern HWND planetPanel;        // Planet information panel
extern HWND satViewPanel;       // View from satellite panel
extern HWND skyViewPanel;       // Sky map panel
extern HWND objectCatPanel;     // Object catalogue panel
extern DLGPROC objectCatProc;	// Object catalogue dialogue procedure
extern HWND orreryPanel;        // Orrery panel
extern HICON satIcon;           // Satellite image icon
extern HICON micon;				// Moon icon calculated for correct phase
extern HICON planetIcons[12];	// Planet icons
extern HANDLE earthImageDIB;	// Handle to master image DIB
extern HPALETTE imagePal;		// Custom palette to display earthImageDIB
extern char whichSat[];         // Name of satellite we're tracking
extern int satIconIndex;        // Which icon to use for satellites ?
extern HBITMAP satsbitmap;      // Satellite save image bitmap
extern int satDisplayed;        // Is satellite icon displayed ?
extern double satCalcTime;		// Time satellite position last calculated
extern double satLAT, satLONG;	// Subsatellite latitude and longitude
extern double satAZ, satEL;		// Satellite azimuth and elevation at observer's location
extern int moonDisplayed;       // Is moon displayed ?
extern int moonx, moony;        // Moon icon position on screen
extern double moonposLast;      // Moon position last calculation time
extern HBITMAP moonsbitmap;     // Moon save background bitmap
extern int cuckooClock;         // Cuckoo clock desired ?
extern int cuckooEnable;		// Is this the first instance which manages the cuckoo clock ?
extern int waveAudioAvailable;	// Does this system implement wave audio output ?
extern int ddeActive;           // Has at least one DDE request been made ?
extern int holped;              // Was help invoked ?
extern int satvterm;            // Show terminator on texture mapped satellite view ?
extern int texturing;           // Texture map view from window ?
extern int viewfrom;            // View from location selection
extern int viewFromFrustrated;	// View from waiting for bitmap to be generated
extern double siteLat;          // Observing site latitude
extern double siteLon;          // Obeserving site longitude
extern char siteName[128];      // Observing site name
extern int sitePicking;         // Currently picking site on map
extern int currentObjCat;       // Last selected object catalogue
extern int plutoPrecise;        // Precise position for Pluto required

extern TIME_ZONE_INFORMATION tzInfo;// Time zone information
extern char *tzName[2];			// Time zone names

extern int skyShowName;			// Show star names in sky map ?
extern int skyShowBflam;		// Show Bayer/Flamsteed numbers in sky ?
extern int skyShowDeep;			// Show deep sky objects in sky ?
extern int precessionCalculation; // Precession calculation mode
extern int skyShowConstellations; // Show constellations in sky window ?
extern int skyShowConbounds;	// Show constellation boundaries in sky map ?
extern int skyShowConnames;		// Show constellation names in sky map ?
extern int skyAlignConnames;	// Align constellation names with horizon in sky map ?
extern int skyShowCoords;		// Show coordinate grid ?
extern int skyShowPlanets;		// Show solar system objects ?

extern int telActive;			// Telescope active flag	
extern double telra, teldec, telalt, telazi;	// Telescope aim point 
extern int telShowName;			// Show star names in telescope ?
extern double telNameMag;		// Maximum magnitude to show names in telescope
extern int telShowBflam;		// Show Bayer/Flamsteed numbers in telescope ?
extern double telBflamMag;		// Maximum magnitude to show Bayer/Flamsteed in telescope
extern int telShowMag;			// Show magnitudes in telescope ?
extern double telShowMagMax, telShowMagMin; // Show magnitude range for telescope
extern int telShowDeep;			// Show deep sky objects in telescope ?
extern double telDeepMag;		// Maximum magnitude to show deep sky objects in telescope
extern int telSouthUp;			// South up in telescope ?
extern int telShowConstellations; // Show constellations in telescope ?
extern int telShowConbounds;	// Show constellation boundaries in telescope ?
extern int telShowConnames;		// Show constellation names in telescope ?
extern int telShowCoords;		// Show coordinate grid in telescope ?
extern int telShowPlanets;		// Show solar system objects in telescope ?

extern int horShowName;			// Show star names at horizon ?
extern double horNameMag;		// Maximum magnitude to show names at horizon
extern int horShowBflam;		// Show Bayer/Flamsteed numbers at horizon ?
extern double horBflamMag;		// Maximum magnitude to show Bayer/Flamsteed at horizon
extern int horShowDeep;			// Show deep sky objects at horizon ?
extern double horDeepMag;		// Maximum magnitude to show deep sky objects at horizon
extern int horShowConstellations; // Show constellations at horizon ?
extern int horShowConbounds;	// Show constellation boundaries at horizon ?
extern int horShowConnames;		// Show constellation names at horizon ?
extern int horShowCoords;		// Show coordinate grid at horizon ?
extern int horShowPlanets;		// Show solar system objects at horizon ?
extern int horShowTerrain;		// Show terrain at horizon ?
extern int horShowScenery;		// Show scenery at horizon ?
extern double horSceneryRoughness;	// Fractal dimension of terrain at horizon

extern int starQuality;			// Star quality rendering
extern int starCatalogue;		// Star catalogue selected
extern int showStarColours;		// Show star colours ?
extern char userCat[];			// User catalogue file name
extern int aTracked;			// Tracking an asteroid ?

struct planet {                 // Planet information entry
    double hlong;               // FK5 Heliocentric longitude
    double hlat;                // FK5 Heliocentric latitude
    double hrv;                 // Heliocentric radius vector
    double dhlong;              // Dynamical Heliocentric longitude
    double dhlat;               // Dynamical Heliocentric latitude
    double ra;                  // Apparent right ascension
    double dec;                 // Apparent declination
    double dist;                // True distance from the Earth
    double mag;                 // Approximate magnitude
    double lha;                 // Local hour angle
    double alt;                 // Altitude above (-below) horizon
    double az;                  // Azimuth from South: West positive, East negative
};
extern struct planet planet_info[11];		// Calculated planetary information

struct asteroid_info {			// Asteroid information
	int cometary;				// Nonzero if cometary element format
	char Name[40];				// Name and number
	double MagH;				// IAU Commission 20 magnitude argument H
	double MagG;				// IAU Commission 20 magnitude argument G
	double SemiMajorAU;			// Semimajor axis in AU
	double Eccentricity;		// Eccentricity of orbit
	double Inclination;			// Inclination of orbit to ecliptic
	double ArgP;				// Argument of perihelion
	double LANode;				// Longitude of the ascending node
	double mAnomaly;			// Mean anomaly at the epoch
	double Epoch;				// Epoch of elements
	double PeriDate;			// Time of perihelion passage
	double PeriAU;				// Perihelion distance, AU
};

extern struct asteroid_info ast_info;	// Information about currently-tracked asteroid

#define PlanetIcon(n)	planetIcons[(n) + ((((n) == 10) && ast_info.cometary) ? 1 : 0)]

#define HSTmagic "HGSC"			// HST file header sentinel

struct HSTfilehead {
    char magic[4];
    short nregions;				// Regions in this file
};

struct HSTritem {				// Region directory item
    short regid;				// Region number
    short numobj;				// Objects in this region
    long fileaddr;				// File address
};

struct HSTobject {
    long ra, dec;				// Fixed point right ascension and declination
    unsigned short classmag;    // Class and fixed point magnitude packed into 3:13 bits
};

extern int pluto(double jd, double *l, double *b, double *r); // Special calculation for Pluto

#define RealTime (runmode && !animate)  // Time represents actual wall-clock time
#define Simulating (!RealTime)			// Time is simulated time at Greenwich

#define WindowProc(x) LRESULT CALLBACK x(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)

extern WindowProc(WndProc);
extern WindowProc(SkyWndProc);

/* Dialogue service functions. */

#define DialogueProc(name) BOOL FAR PASCAL name(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)

extern DialogueProc(About);
extern DialogueProc(TimeZoneDlg);
extern DialogueProc(SetJdate);
extern DialogueProc(SetUtime);
extern DialogueProc(SattrackDlg);
extern DialogueProc(SatIconDlg);
extern DialogueProc(SunMoonDlg);
extern DialogueProc(SatViewDlg);
extern DialogueProc(planetDlg);
extern DialogueProc(SetObserverLocation);
extern DialogueProc(TelescopeDlg);
extern DialogueProc(HorizonDlg);
extern DialogueProc(ObjectCatDlg);
extern DialogueProc(OrreryDlg);

extern int updsat(double jd, int dotrail, int repaint);
extern void changesatview(double satlat, double satlon, double satalt);
extern void changemoonview(void);
extern void changesunview(void);
extern void satviewchanged(void);
extern void release_satview_bitmaps(void);

extern void drawsat(HWND hWnd);
extern void undrawsat(HWND hWnd);
extern void restoresat(HWND hWnd);
extern int hitsat(int x, int y);
extern void setSatFromName(void);

//	From Sunmoon.c

extern void upd_sun_moon(double jd, int realtime, int repaint);
extern void updmoon(HWND hWnd, HDC hDC, RECT windex, double jd);
extern void drawmoon(HDC hDC);
extern void undrawmoon(HDC hDC);
extern void restoremoon(HDC hDC);
extern int hitmoon(int x, int y);

//	From Objcat.c

extern void launchObjectCat(void);
extern void buildFindIndex(void);
extern void relFindIndex(void);
extern int findObject(double ra, double dec, WPARAM keysdown,
					  int *catno, int *objno, double *separation);
extern void pointObjectCat(int catno, int objno);

//	From Planetp.c

extern void updatePlanet(double jd, int normal);
extern void planetSiteChanged(void);
extern void calcPlanet(double jd);

//	From Sky.c

extern void launchSkyWindow(void);
extern void loadPlanetIcons(void);
extern void unloadPlanetIcons(void);
extern void sitePicked(WORD px, WORD py, WORD sx, WORD sy);
extern void updateSky(double jd, int force);
extern void skySatChanged(void);
extern void selectSkyObjectBin(double ra, double dec);
extern void parseRaDec(char *ra, char *dec, double *fra, double *fdec);
extern void selectSkyObject(char *ra, char *dec);
extern void updateOrrery(double jd, int force);

//	From Terrain.c

extern BOOL terrainSeeded;
extern void drawTerrain(HWND hWnd, HDC hDC, double jd, double azimuth,
						double roughness, int scenery);

//	From Cuckoo.c

extern void updateCuckoo(void);

//  From Asteroid.c

extern void selectAsteroid(char *aname, double aelem[8]);
extern void selectComet(char *aname, double aelem[8]);
extern void trackAsteroid(double jd, double *ra, double *dec, double *dist,
						  double *hlong, double *hlat, double *hrv, int quick);

//  From Settings.c

extern void saveSettings(char *appName);
extern void loadSettings(char *appName);
extern void defSettings(char *appName);

//  Calculation functions from Suncalc.c

extern void set_tm_time(struct tm *t, BOOL islocal);
extern double jtime(struct tm *t);
extern void jyear(double td, long *yy, int *mm, int *dd),
            jhms(double j, int *h, int *m, int *s);
extern double gmst(double jd);
extern double ucttoj(long year, int mon, int mday,
                     int hour, int min, int sec);
extern void sunpos(double jd, int apparent,
				   double *ra, double *dec, double *rv, double *slong);
extern double phase(
					double  pdate, double  *pphase, double  *mage, double  *dist,
					double  *angdia, double  *sudist, double  *suangdia);
extern void phasehunt(double sdate, double phases[5]);
extern void highmoon(double jd, double *l, double *b, double *r);
extern double obliqeq(double jd);
extern void ecliptoeq(double jd, double Lambda, double Beta, double *Ra, double *Dec);
extern void definePrecession(double epoch);
extern void precessObject(double ira, double idec, double *ora, double *odec);


extern void invalidate_refresh(void);
extern void release_bitmaps(void);
extern void localtimeformat(void);

extern void mapdraw(int xsize, int ysize,
                    void (PASCAL *vector)(int x1, int y1, int x2, int y2));
extern void maplatlon(void (*llfunc)(int lat1, int lon1, int lat2, int lon2));

extern void ringgg(HWND hWnd, HDC hDC, int repaint, int copyCat);
extern void go_iconic(HWND hWnd);
extern void new_display_mode(HWND hWnd, int mode);
extern char *rstring(int resid);
extern LPBITMAPINFOHEADER palMapStart(LPBITMAPINFOHEADER bh, int *bmode);
extern void palMapEnd(void);

//  DDE link management functions from Ddeserv.c

extern void ddeInit(HINSTANCE hInstance);
extern void ddeAdvise(void);
extern void ddeTerm(void);
extern void updateDDEInfo(void);
extern void updateScopeDDE(void);

//      Comma separated value database scanning routines from Csv.c

extern void CSVscanInit(char *s);
extern int CSVscanField(char *f);

// Orbit calculation routines from Orbit.c

typedef double mat3x3[3][3];

extern void InitOrbitRoutines(double EpochDay);
extern long GetDayNum(int Year, int Month, int Day);
extern double Kepler(double MeanAnomaly, double Eccentricity);
extern void GetSubSatPoint(double SatX, double SatY, double SatZ,
						   double Time,
                           double *Latitude, double *Longitude, double *Height);
extern void GetPrecession(double SemiMajorAxis, double Eccentricity, double Inclination,
                   double *RAANPrecession, double *PerigeePrecession);
extern void GetSatPosition(double EpochTime, double EpochRAAN, double EpochArgPerigee,
						   double SemiMajorAxis, double Inclination, double Eccentricity,
						   double RAANPrecession, double PerigeePrecession, double Time,
						   double TrueAnomaly,
						   double *X, double *Y, double *Z,
						   double *Radius, double *VX, double *VY, double *VZ);
extern int Eclipsed(double SatX, double SatY, double SatZ,
                         double SatRadius, double CurrentTime);
extern void GetSitPosition(double SiteLat, double SiteLong, double SiteElevation,
						   double CurrentTime,
                    	   double *SiteX, double *SiteY, double *SiteZ,
                    	   double *SiteVX, double *SiteVY, mat3x3 SiteMatrix);
extern void GetBearings(double SatX, double SatY, double SatZ,
						double SiteX, double SiteY, double SiteZ,
                 		mat3x3 SiteMatrix, double *Azimuth, double *Elevation);
                 		
//	From Vsop87.c

#define FULL_PLANET_INFO	0x8000			// Calculate complete high-precision planet info
extern void planets(double jd, int which);	/* Update planetary positions */
extern void nutation(double jd, double *deltaPsi, double *deltaEpsilon);                 		

extern int animate, runmode, displaymode, lincr,
                   idir, tracking, satx, saty, xdots, ydots, anirate,
                   showmoon;
extern int ImageWid, ImageHgt;				// Earth bitmap width, height                   
extern int imageBitmap;						// Selected Earth bitmap index
extern int bitmapNum;						// Number of bitmaps available in DLL
extern double faketime;						// Real or simulated time
extern struct satellite sat;				// Satellite being tracked
extern char satDBFile[128];					// Selected satellite database
