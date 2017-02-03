/*

			Sunclock for Windows

		   Planet information panel

*/

#include "sunclock.h"

static double planetLast = -1E10; 	/* Planet last position calculation */
static double planetInterval = CalculationInterval(10);

static double lastalt[10] = {-9999};		// Last calculated altitudes
static double lastsaltime = -9999;			// Time of last altitude calculation

#define Set(field) SetDlgItemText(planetPanel, (field), tbuf)

/*  PLANETSITECHANGED  --  Notify planet panel of change in observing site.  */

void planetSiteChanged(void)
{
	if (planetPanel != NULL) {
		char tbuf[80];

		InvalidateCalculation(planet);
	    InvalidateRgn(planetPanel, NULL, FALSE);
	    sprintf(tbuf, Format(31), siteName);
	    SetWindowText(planetPanel, tbuf);
	    if (!runmode) {
	    	updatePlanet(faketime, TRUE);
	    }
	}
}

/*  CALCPLANET  --  Calculate planetary positions and altitude and azimuth from
					viewer's position.  */

void calcPlanet(double jd)
{
	int i;
	double igmst, latsin, latcos;

	planets(jd, 0xFFFF);
	igmst = gmst(jd);
	latsin = sin(dtr(siteLat));
	latcos = cos(dtr(siteLat));
	for (i = 0; i <= (aTracked ? 10 : 9); i++) {
		planet_info[i].lha = dtr(fixangle((igmst * 15) - siteLon - planet_info[i].ra));
		planet_info[i].az = rtd(atan2(sin(planet_info[i].lha), cos(planet_info[i].lha) * latsin -
								tan(dtr(planet_info[i].dec)) * latcos));
		planet_info[i].alt = rtd(asin(latsin * sin(dtr(planet_info[i].dec)) +
								latcos * cos(dtr(planet_info[i].dec)) * cos(planet_info[i].lha)));
	}
}

/*  REFRESHLASTALT  --  Refresh last altitude to determine rising from setting.  */

static void refreshLastSalt(double jd)
{
	int i;

	calcPlanet(lastsaltime = (jd - (1.0 / 48.0)));
	for (i = 0; i <= (aTracked ? 10 : 9); i++) {
		lastalt[i] = planet_info[i].alt;
	}
}

/*  UPDATEPLANET  --  Recalculate planet positions (if necessary) and update
				      the fields in the dialogue.  */

void updatePlanet(double jd, int normal)
{
	if (planetPanel != NULL) {
		int i;
		char tbuf[80];

	    if (!normal) {
	    	InvalidateCalculation(planet);
	    	sprintf(tbuf, Format(31), siteName);
	    	SetWindowText(planetPanel, tbuf);
	    }
		if (NeedToCalculate(planet, jd)) {
			double d, md, mangdia, suangdia, setalt, upalt;

			if (lastsaltime > jd || ((jd - lastsaltime) > (1.0 / 24.0))) {
				refreshLastSalt(jd);
			}
			calcPlanet(jd);
			phase(jd, &d, &d, &md, &mangdia, &d, &suangdia);
			lastsaltime = jd;

			if (aTracked) {
				SetDlgItemText(planetPanel, IDM_PL_ASTEROID, ast_info.Name);
			}
#define ShowAster(id) ShowWindow(GetDlgItem(planetPanel, id + 10), aTracked ? SW_SHOWNORMAL : SW_HIDE)
			ShowWindow(GetDlgItem(planetPanel, IDM_PL_ASTEROID), aTracked ? SW_SHOWNORMAL : SW_HIDE);
			ShowAster(IDM_PL_RA);
			ShowAster(IDM_PL_DEC);
			ShowAster(IDM_PL_DIST);
			ShowAster(IDM_PL_ALT);
			ShowAster(IDM_PL_AZ);
			ShowAster(IDM_PL_UPSET);

			for (i = 0; i <= (aTracked ? 10 : 9); i++) {
				if (i == 9) {
					int st = (planet_info[i].hrv) > 0 ? SW_SHOWNORMAL : SW_HIDE;

					// If outside the known calculable orbit of Pluto, hide the bogus numbers

#define DisPluto(id) ShowWindow(GetDlgItem(planetPanel, id + i), st)

				    EnableWindow(GetDlgItem(planetPanel, IDM_PL_PLUTO), planet_info[i].hrv > 0);
					DisPluto(IDM_PL_RA);
					DisPluto(IDM_PL_DEC);
					DisPluto(IDM_PL_DIST);
					DisPluto(IDM_PL_ALT);
					DisPluto(IDM_PL_AZ);
					DisPluto(IDM_PL_UPSET);
				}
				{	double m = fixangle(planet_info[i].ra) / 15;

					sprintf(tbuf, Format(34), (int) m, ((int) (m * 60)) % 60,
							fmod(m * 3600, 60.0));
				}
				Set(IDM_PL_RA + i);
				{	double m = abs(planet_info[i].dec);

					sprintf(tbuf, Format(38), planet_info[i].dec < 0 ? '-' : '+',
							(int) m, fmod(m * 60, 60));
				}
				Set(IDM_PL_DEC + i);
				if (i == 3) {
                    sprintf(tbuf, Format(30), md / EarthRad);
				} else {
					sprintf(tbuf, Format(29), planet_info[i].dist);
				}
				Set(IDM_PL_DIST + i);
				sprintf(tbuf, Format(29), planet_info[i].alt);
				Set(IDM_PL_ALT + i);
				sprintf(tbuf, Format(29), planet_info[i].az);
				Set(IDM_PL_AZ + i);
				setalt = -0.5667;
				upalt = 0;

				/* (Approximately) compensate for the effect of refraction by
				   the Earth's atmosphere so that "rising" and "setting" appears
				   at the time the object actually becomes visible or disappears
				   rather than considering pure geometry of touching the
				   horizon.  These calculations follow the guidance of Meeus in
				   chapter 14 of Astronomical Algorithms.

				   Note that the Sunrise is considered to be the first appearance
				   of the limb above the horizon.

				   Moonrise is even more complicated.  First of all, in addition to
				   refraction we must also compensate for the parallax of the
				   Moon which, in turn, varies due to the eccentricity of the Moon's
				   orbit.  Further (departing from normal astronomical convention),
				   if we wish to also time moonrise from first appearance of the
				   limb rather than the centre of the disc (we adopt this nonstandard
				   criterion to prevent squawks from folks who see the Moon rising
				   out their window before we've labeled it as "rising"), we must add
				   the semidiameter of the Moon to our refraction correction.  That,
				   of course, *also* changes from perigee to apogee, so there goes
				   another correction into the stew.  Ain't this fun?

				   Then there's the question of how long "rising" and "setting"
				   lasts.  For a planet it's almost instantaneous, but we
				   compromise and consider rising and setting to be the interval
				   between refraction-corrected rise/set and geometric (about half
				   a degree to the horizon.  For the Sun and Moon, we must adjust the
				   end of rise/set times for the diameter of the disc.  */

				if (i == 0) {				// Sun
					setalt -= suangdia / 2;
					upalt = setalt + suangdia;
				} else if (i == 3) {		// Moon
#define msmax       384401.0					/* Semi-major axis of Moon's orbit in km */
#define mparallax   0.9507						/* Parallax at distance msmax from Earth */
					setalt += 0.7275 * (mparallax * (md / msmax));
					setalt -= mangdia / 2;
					upalt = setalt + mangdia;
				}
				if ((planet_info[i].alt > setalt) && (planet_info[i].alt < upalt) && runmode) {
					strcpy(tbuf, rstring((planet_info[i].alt > lastalt[i]) ? IDS_RISING : IDS_SETTING));
				} else {
					if (planet_info[i].alt > upalt) {
						strcpy(tbuf, (abs(planet_info[i].lha) < dtr(0.5)) ?
								 rstring(IDS_TRANSIT) : rstring(IDS_UP));
					} else {
						strcpy(tbuf, rstring(IDS_SET));
					}
				}
				Set(IDM_PL_UPSET + i);
				lastalt[i] = planet_info[i].alt;		// Save last altitude
			}
		}
	}
}

DialogueProc(planetDlg)
{
	switch (message) {
		case WM_INITDIALOG:
			planetPanel = hDlg;
			updatePlanet(faketime, FALSE);
			return TRUE;

		case WM_CLOSE:
			DestroyWindow(hDlg);
			return TRUE;
			
		case WM_DESTROY:
			planetPanel = NULL;
			return TRUE;

        case WM_COMMAND:
            switch (WM_COMMAND_ID(wParam)) {

                case IDOK:
                    SendMessage(hDlg, WM_CLOSE, 0, 0L);
                    return TRUE;

                case IDM_PL_HELP:
                	WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                				((DWORD) ((LPSTR) rstring(IDS_HELP_ON_PLANETS))));
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
