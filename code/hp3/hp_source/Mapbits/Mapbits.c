/*

		Map bitmap library
		
*/

#include <windows.h>
#include <stdlib.h>
#include "resource.h"
#include "mapbits.h"

static HMODULE hInst;				// Instance handle of library

/*	Properties determined by strings in the resource file.
	Obtaining them this way allows the user to edit the
	DLL directly and modify these values.  */
	
static WORD nBitmaps = 0;	

//	Library initialisation

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	hInst = hModule;				// Save instance for later use
	return 1;
}					  

//	Return number of bitmaps in library

int FAR PASCAL mapbitsCount(void)
{
	static char s[80];
	
	if (LoadString(hInst, IDS_NUMBITMAPS, s, sizeof s) > 0) {
		nBitmaps = atoi(s);
	} else {
		nBitmaps = 0;
	}
	return nBitmaps;
}

//	Obtain a bitmap by explicit number from the DLL

void FAR PASCAL mapbitsGetBitMap(int bitmapNumber,				// Bitmap number to get
										 HANDLE FAR *hBitmap)	// Handle to bitmap or NULL = error
{
	if (nBitmaps == 0) {
		(void) mapbitsCount();
	}

	if (bitmapNumber >= 0 && bitmapNumber < nBitmaps) {
		*hBitmap = LoadResource(hInst, FindResource(hInst,
					MAKEINTRESOURCE(IDB_BITMAP_INDEX + bitmapNumber), RT_BITMAP));
	} else {
		*hBitmap = NULL;
	}
}

//	Get bitmap description string	

void FAR PASCAL mapbitsGetDesc(int bitmapNumber,				// Bitmap description to get
								   LPSTR sbuf, UINT buflen)		// Buffer and length
{
	if (nBitmaps == 0) {
		(void) mapbitsCount();
	}

	if (bitmapNumber >= 0 && bitmapNumber < nBitmaps) {
		if (LoadString(hInst, IDS_BITMAP_NAME + bitmapNumber, sbuf, buflen) <= 0) {
			sbuf[0] = 0;
		}
	} else {
		sbuf[0] = 0;
	}
}																	