/*

		Scenery library
		
*/

#include <windows.h>
#include <stdlib.h>
#include "resource.h"
#include "scenery.h"

static HINSTANCE hInst;					// Instance handle of library

/*	Properties determined by strings in the resource file.
	Obtaining them this way allows the user to edit the
	DLL directly and modify these values.  */
	
static WORD nIcons, sizeIcons, densityIcons;	

//	Library initialisation

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	hInst = hModule;				// Save instance for later use
	return 1;
}					  

//	Initialise for scenery generation for an image

void FAR PASCAL sceneryInit(double julianDate,			// Time and date
									double siteLat,		// Observer latitude
									double siteLon,		// Observer longitude
									double viewAzimuth,	// Azimuth of window center
									WORD imageHeight,	// Image height
									WORD imageWidth,	// Image width
									WORD randomNumber,	// A 15 bit random value
									WORD FAR *numIcons,	// Return: Number of icons
									WORD FAR *iconSize,	//         Icon size
									WORD FAR *iconDensity) //	   Icon density
{
	static char s[80];
	
	if (LoadString(hInst, IDS_NUMICONS, s, sizeof s) > 0) {
		nIcons = atoi(s);
	} else {
		nIcons = 0;
	}
	*numIcons = nIcons;
	
	if (LoadString(hInst, IDS_ICONSIZE, s, sizeof s) > 0) {
		sizeIcons = atoi(s);
	} else {
		sizeIcons = 32;
	}
	*iconSize = sizeIcons;
	
	if (LoadString(hInst, IDS_DENSITY, s, sizeof s) > 0) {
		densityIcons = atoi(s);
	} else {
		densityIcons = 10;
	}
	*iconDensity = densityIcons;
}

//  Terminate scenery generation for an image

void FAR PASCAL sceneryTerm(void)
{
}

//	Obtain an icon to draw at a given location in the image

void FAR PASCAL sceneryIcon(int xPos, int yPos,				// Where icon is to be drawn
									int randomNumber,		// A 15 bit random number
									HICON FAR *hIcon,		// Return handle to icon
									WORD FAR *hWidth)		// Width (advance) after this icon
{
	*hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1 + (randomNumber % nIcons)));
	*hWidth = sizeIcons;
}

//	Obtain an icon by explicit number from the DLL

void FAR PASCAL getSceneryIcon(int iconNumber,				// Icon number to get
									   HICON FAR *hIcon)	// Handle to icon or NULL = error
{
	static char s[80];
	static WORD ticons;
	
	if (LoadString(hInst, IDS_TOTICONS, s, sizeof s) > 0) {
		ticons = atoi(s);
	} else {
		ticons = 0;
	}
	if (iconNumber >= IDI_ICON1 && iconNumber <= ticons) {
		*hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1 + (iconNumber - 1)));
	} else {
		*hIcon = NULL;
	}
}																	