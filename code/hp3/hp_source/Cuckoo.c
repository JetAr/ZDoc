/*

		Cuckoo clock

*/

#include "sunclock.h"

/*  CUCKOO  --  Play the cuckoo sound resource a given number of times.  */

static void cuckoo(int tweets)
{
	HANDLE hResInfo, hRes;
	LPSTR lpRes;

	hResInfo = FindResource(hInst, "cuckoo", "WAVE");
	if (hResInfo != NULL) {
		hRes = LoadResource(hInst, hResInfo);
		if (hRes != NULL) {
			lpRes = LockResource(hRes);
			if (lpRes != NULL) {
				int i;

				for (i = 0; i < tweets; i++) {
					sndPlaySound(lpRes, SND_MEMORY | SND_SYNC | SND_NODEFAULT);
					/* If we're doing multiple tweets, let other applications sneak in
					   between chirps. */
					if (tweets > 1) {
						MSG dmsg;
						PeekMessage(&dmsg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE);
					}
				}
				UnlockResource(hRes);
			}
		}
		FreeResource(hRes);
	}
}

/*  UPDATECUCKOO  --  Check if it's time and give the user the bird if appropriate.  */

void updateCuckoo(void)
{
// #define DEBUG_BOID
    if (cuckooEnable && cuckooClock && waveAudioAvailable) {
		static int oldhh = -1, oldmm = -1;
		int newhh, newmm;
		struct tm lt;

		set_tm_time(&lt, TRUE);
    	newhh = lt.tm_hour % 12;
    	newmm = lt.tm_min;

#ifdef DEBUG_BOID
		if (animate || !runmode) {
			int hh, mmm, ss;
			jhms(faketime, &hh, &mmm, &ss);
			newhh = hh % 12;
			newmm = mmm;
		}
#endif

		if (newhh == 0) {
			newhh = 12;
		}

		// If the hour's changed, chirp the civil hour, 1 to 12.

		if (oldhh != newhh) {
			if (oldhh != -1) {
				cuckoo(newhh);
			}
			oldhh = newhh;
		}

		// If we've just passed the half-hour mark, chirp once.

		if (newmm >= 30 && oldmm < 30) {
			if (oldmm != -1) {
				cuckoo(1);
			}
		}
		oldmm = newmm;
    }
}