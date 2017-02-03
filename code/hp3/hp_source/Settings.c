/*

		Save and restore program settings (user preferences)

*/

#include "sunclock.h"


/*	The following table provides pointers to the settings specified by
	settings resource.  These pointers must correspond, line for line
	with the settings named in the resource.  */

static void *setvars[] = {
	&satIconIndex,
	&showmoon,
	&cuckooClock,
	&displaymode,
	&imageBitmap,
	&lincr,
	&idir,
	&anirate,
	&satvterm,
	&currentObjCat,
	&precessionCalculation,
	&skyShowConstellations,
	&skyShowConbounds,
	&skyShowConnames,
	&skyAlignConnames,
	&skyShowCoords,
	&skyShowPlanets,
	&skyShowName,
	&skyShowBflam,
	&skyShowDeep,
	&telShowConstellations,
	&telShowConbounds,
	&telShowConnames,
	&telShowCoords,
	&telShowPlanets,
	&telShowName,
	&telShowBflam,
	&telShowMag,
	&telShowDeep,
	&telSouthUp,
	&starCatalogue,
	&starQuality,
	&showStarColours,
	&horShowName,
	&horShowBflam,
	&horShowDeep,
	&horShowConstellations,
	&horShowConbounds,
	&horShowConnames,
	&horShowCoords,
	&horShowPlanets,
	&horShowTerrain,
	&horShowScenery,	

	// Floating point settings
	&siteLat,
	&siteLon,
	&telNameMag,
	&telBflamMag,
	&telShowMagMax,
	&telShowMagMin,
	&telDeepMag,
	&horNameMag,
	&horBflamMag,
	&horDeepMag,
	&horSceneryRoughness,

	// Strings, all of which MUST be 128 characters in length
	whichSat,
	satDBFile,
	siteName,
	userCat
};

static char restype[] = "Settings";				// Settings resource type code

static HKEY keyh;								// Registry key handle


/*  ENUMSETTINGS  --  Iterate over all settings, calling a given procedure for each.  */

static void enumSettings(char *appName,
						 void (*action)(void *vptr, LPCSTR varname, LPCSTR varval))
{
	HGLOBAL settabH;
	LPCSTR settab;
	char rsc[256];

	settabH = LoadResource(hInst, FindResource(hInst, appName, restype));
	if (settabH != NULL) {
		settab = LockResource(settabH);
    	if (settab != NULL) {
    		int i = 0;

    		while (*settab != ';') {
    			LPSTR es = strchr(settab, '\r'),
    				  vs = strchr(settab, '=');

    			if (es == 0 || vs == 0) {
    				char errm[80];

    			    wsprintf((LPSTR) errm, (LPSTR) "Error in Settings, line %d.", i + 1);
    				MessageBox(NULL, errm, appName, MB_ICONSTOP | MB_OK);
    				break;
    			}
				if ((es - settab) >= (sizeof rsc)) {
    				char errm[80];

    			    wsprintf((LPSTR) errm, (LPSTR) "Length overflow in Settings, line %d.", i + 1);
    				MessageBox(NULL, errm, appName, MB_ICONSTOP | MB_OK);
    				break;
				}
				memcpy(rsc, settab, (es - settab) + 1);
				rsc[es - settab] = 0;
				rsc[vs - settab] = 0;
				vs = rsc + (vs - settab);
    			action(setvars[i], rsc, vs + 1);
    			settab = es + 2;
    			i++;
    		}
    		UnlockResource(settabH);
    	}
    	FreeResource(settabH);
    }
}

/*  SAVESETTINGS  --  Save current settings in the registry.  */

static void saveAction(void *vptr, LPCSTR varname, LPCSTR varval)
{
	char tbuf[80];

#define regStore(name, value) RegSetValueEx(keyh, name, 0, REG_DWORD, (BYTE *) value, sizeof(*value))
#define regStoreS(name, value) RegSetValueEx(keyh, name, 0, REG_SZ, (BYTE *) value, lstrlen(value) + 1)

	switch (varval[0]) {
		case 'i':
			regStore(varname, (DWORD *) vptr);
			break;

		case 'f':
			sprintf(tbuf, "%.6g", *((double *) vptr));
			regStoreS(varname, tbuf);
			break;

		case 's':
			regStoreS(varname, (LPCSTR) vptr);
			break;
	}
#undef regStore
#undef regStoreS
}

void saveSettings(char *appName)
{
	TCHAR keyname[80];
	
	strcpy(keyname, rstring(IDS_REGISTRY_SECTION));
	strcat(keyname, rstring(IDS_APPNAME));
	if (RegCreateKeyEx(HKEY_CURRENT_USER,
				keyname,					// Subkey name
				0,							// Reserved
				"",							// Bogus class string
				REG_OPTION_NON_VOLATILE,	// Options
				KEY_ALL_ACCESS,				// Access rights			
				NULL,						// Security attributes
				&keyh,						// Returned handle to the key
				NULL) == ERROR_SUCCESS) {	// Returned disposition (not required)				
		enumSettings(appName, saveAction);
	}
}

/*  LOADSETTINGS  --  Load current settings from initialisation file.  */

static void loadAction(void *vptr, LPCSTR varname, LPCSTR varval)
{
	char tbuf[128];
	DWORD vsize;
	
#define regGet(name, value)  vsize = sizeof(*value); RegQueryValueEx(keyh, name, NULL, NULL, (BYTE *) value, &vsize)
#define regGetS(name, value) vsize = 128; RegQueryValueEx(keyh, name, NULL, NULL, (BYTE *) value, &vsize)
#define regGetF(name, value) regGetS(name, tbuf); sscanf(tbuf, "%lf", value)

	switch (varval[0]) {
		case 'i':
			regGet(varname, (DWORD *) vptr);
			break;

		case 'f':
			regGetF(varname, (double *) vptr);
			break;

		case 's':
			regGetS(varname, (LPSTR) vptr);
			break;
	}
#undef regGet
#undef regGetS
#undef regGetF
}

void loadSettings(char *appName)
{
	TCHAR keyname[80];	
	
	strcpy(keyname, rstring(IDS_REGISTRY_SECTION));
	strcat(keyname, rstring(IDS_APPNAME));

	if (RegOpenKeyEx(HKEY_CURRENT_USER,
			keyname,				// Subkey name
			0,						// Reserved
			KEY_QUERY_VALUE,		// Security access mask
			&keyh) == ERROR_SUCCESS) {
		enumSettings(appName, loadAction);
	} else {
		defSettings(appName);
	}
}

/*  DEFSETTINGS  --  Reset all settings to initial defaults.  */

static void defAction(void *vptr, LPCSTR varname, LPCSTR varval)
{
	char tbuf[80];

	switch (varval[0]) {
		case 'i':
			lstrcpy(tbuf, varval + 1);
			*((int *) vptr) = atoi(tbuf);
			break;

		case 'f':
			lstrcpy(tbuf, varval + 1);
			*((double *) vptr) = atof(tbuf);
			break;

		case 's':
			lstrcpy((LPSTR) vptr, varval + 1);
			break;
	}
}

void defSettings(char *appName)
{
	enumSettings(appName, defAction);
}
