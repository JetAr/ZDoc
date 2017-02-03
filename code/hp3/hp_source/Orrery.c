//#define saveOrbits
#define useResource
/*

			Sunclock for Windows

			  	  Orrery

*/

#include "sunclock.h"

static int orrLat = 90;					// Orrery latitude
static int orrLong = 0;					// Orrery longitude
static int orrInner = FALSE;			// Orrery inner solar system only ?
static int orrScale = IDC_OR_LOG;		// Distance scale

static double orreryLast = -1E10;
static double orreryInterval = CalculationInterval(84000);
static double earthlong, earthrv;		// Heliocentric coordinates of the Earth
static double minperh = 0.28;			// Minimum perihelion (usually Mercury, but possible ast./comet)

static double orbitsLast = -1E10;		// Orbit bitmap last calculation time
/* The fastest-changing aspect of Solar system orbits is the perihelion shift in
   the orbit of Mercury, and that amounts to only about 1.5 degrees per century (
   0.559974 +/= 0.000041 arc seconds per century, to be precise).  So, we only
   bother to recalculate the orbits when they're a century out of date. */
static double orbitsInterval = 100.0 * 365.0;
static HBITMAP orrbmap = NULL;			// Orrery bitmap
static int orrbmapValid = FALSE;		// Is bitmap valid for refresh ?

//	Sidereal rotation period of planets in days

static double rotationPeriod[] = {
	87.969,
	224.701,
	365.256,
	686.980,
	4332.589,
	10759.22,
	30685.4,
	60189.0,
	90465.0,
	0.0									// Asteroid period calculated from orbital elements
};

/*  RELEASEORRERYBITMAPS  --  Release saved bitmap.  */

static void releaseOrreryBitmaps(void)
{
	if (orrbmap != NULL) {
		DeleteObject(orrbmap);
		orrbmap = NULL;
	}
	orrbmapValid = FALSE;
}

/*  UPDATEORRERY  --  Update orrery if needed.  */

void updateOrrery(double jd, int create)
{
	if (create || ((orreryPanel != NULL) && NeedToCalculate(orrery, jd))) {
		double sunra, sundec, sunlong;

		if (create) {
			InvalidateCalculation(orrery);
			InvalidateCalculation(orbits);
		}
		calcPlanet(jd);
		sunpos(jd, TRUE, &sunra, &sundec, &earthrv, &sunlong);
		earthlong = sunlong + 180;
		if (orreryPanel != NULL) {
			InvalidateRgn(orreryPanel, NULL, FALSE);
		}
		orreryLast = jd;
		if (NeedToCalculate(orbits, jd)) {
			orrbmapValid = FALSE;
		}
	}
}

/*  PROJECT  --  Project a longitude, latitude, and radius vector into the
				 display space.  */

static void project(double along, double alat, double arv, int cr,
					int plno, int nplan, double diau, int *u, int *v, int *z)
{
	double vlong, lrv, acoslat, vx, vy, vz, tlat;
#define MercOffset	16

	vlong = dtr(along - orrLong) + (PI / 2);	// Rotate about Z axis for longitude
	if (orrScale == IDC_OR_LOG) {
		lrv = (int) (MercOffset + (cr - MercOffset) * log10(1.0 + 9.0 * ((arv - minperh) / (diau))));
	} else if (orrScale == IDC_OR_REAL) {
		lrv = (int) (MercOffset + (cr - MercOffset) * (arv / diau));
	} else {// orrScale == IDC_OR_EQUAL
		lrv = (int) (MercOffset + (cr - MercOffset) * ((plno == 10 ? 4.5 : ((double) plno)) / ((double) nplan)));
	}

    acoslat = abs(cos(dtr(alat)));
	vx = lrv * sin(vlong) * acoslat;
	vy = lrv * cos(vlong) * acoslat;
    vz = lrv * sin(dtr(alat));
    tlat = dtr(orrLat);

    // Rotate around X axis for latitude projection

    *u = (int) vx;
    *v = (int) (vz * cos(tlat) - vy * sin(tlat));
    *z = (int) vz;
}

/*  ORRERYDLG  --  Orrery dialogue procedure.  */

DialogueProc(OrreryDlg)
{
	RECT dw, cw;
	HWND cWnd;
	static RECT button1;
	RECT hbar, vbar;
	static int hbaroff, vbaroff, shbar, svbar, yvbar, xhbar,
			   buttonY, buttonDeltaX, buttonBarycentre, lastx, lasty, imagex, imagey,
			   minx, miny;
	char tbuf[40];

#define Ifld(f, v) sprintf(tbuf, Format(21), v); SetDlgItemText(hDlg, f, tbuf)

	switch (message) {
		case WM_INITDIALOG:
			GetClientRect(hDlg, &dw);
			lastx = dw.right;
			lasty = dw.bottom;
			GetWindowRect(hDlg, &cw);
			minx = cw.right - cw.left;
			miny = cw.bottom - cw.top;
			GetWindowRect(GetDlgItem(hDlg, IDOK), &button1);
			ScreenToClient(hDlg, (POINT FAR *) &(button1.left));
			buttonY = dw.bottom - button1.top;
			GetWindowRect(GetDlgItem(hDlg, IDC_OR_HELP), &cw);
			ScreenToClient(hDlg, (POINT FAR *) &(cw.left));
			ScreenToClient(hDlg, (POINT FAR *) &(cw.right));
			buttonDeltaX = cw.left - button1.left;
			buttonBarycentre = ((dw.right - button1.left) / 2) - button1.left;
			GetWindowRect(GetDlgItem(hDlg, IDC_OR_IMAGE), &cw);
			ScreenToClient(hDlg, (POINT FAR *) &(cw.left));
			ScreenToClient(hDlg, (POINT FAR *) &(cw.right));
			imagex = cw.right - cw.left;
			imagey = cw.bottom - cw.top;
			yvbar = cw.top;
			xhbar = cw.left;
			GetWindowRect(GetDlgItem(hDlg, IDC_OR_HSCROLL), &hbar);
			GetWindowRect(GetDlgItem(hDlg, IDC_OR_VSCROLL), &vbar);
			svbar = vbar.right - vbar.left;
			shbar = hbar.bottom - hbar.top;
			ScreenToClient(hDlg, (POINT FAR *) &(hbar.left));
			ScreenToClient(hDlg, (POINT FAR *) &(vbar.left));
			hbaroff = dw.bottom - hbar.top;
			vbaroff = dw.right - vbar.left;
		    SetScrollRange(GetDlgItem(hDlg, IDC_OR_VSCROLL), SB_CTL, -90, 90, FALSE);
		    SetScrollRange(GetDlgItem(hDlg, IDC_OR_HSCROLL), SB_CTL, -180, 180, FALSE);
			SetScrollPos(GetDlgItem(hDlg, IDC_OR_VSCROLL), SB_CTL, (int) -orrLat, TRUE);
			SetScrollPos(GetDlgItem(hDlg, IDC_OR_HSCROLL), SB_CTL, (int) orrLong, TRUE);
			Ifld(IDC_OR_LAT, orrLat);
			Ifld(IDC_OR_LONG, orrLong);
			loadPlanetIcons();
            updateOrrery(faketime, TRUE);
   			return TRUE;

		case WM_CLOSE:
			DestroyWindow(hDlg);
			return TRUE;

		case WM_DESTROY:
			orreryPanel = NULL;
			releaseOrreryBitmaps();
			unloadPlanetIcons();
			return TRUE;

		case WM_COMPACTING:
			releaseOrreryBitmaps();
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
				releaseOrreryBitmaps();
#define floatChild(x)	cWnd = GetDlgItem(hDlg, x);								\
						GetWindowRect(cWnd, &cw);								\
						ScreenToClient(hDlg, (POINT FAR *) &(cw.left));			\
						cw.top += (cheight - lasty);							\
						SetWindowPos(cWnd, NULL, cw.left, cw.top, 0, 0,			\
							SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER)

				floatChild(IDOK);
				floatChild(IDC_OR_HELP);
				cWnd = GetDlgItem(hDlg, IDC_OR_IMAGE);
				// Resize the image area
				SetWindowPos(cWnd, NULL, 0, 0, imagex + (cwidth - lastx),
							imagey + (cheight - lasty),
							SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
				imagex += (cwidth - lastx);
				imagey += (cheight - lasty);
				/* ...then move the scroll bars to the edges of the dialogue and
				   resize them so they're as wide and high as the image area. */
				SetWindowPos(GetDlgItem(hDlg, IDC_OR_VSCROLL), NULL,
							cwidth - vbaroff, yvbar,
							svbar,
							imagey,
							SWP_NOACTIVATE | SWP_NOZORDER);
				SetWindowPos(GetDlgItem(hDlg, IDC_OR_HSCROLL), NULL,
							xhbar, cheight - hbaroff,
							imagex,
							shbar,
							SWP_NOACTIVATE | SWP_NOZORDER);
				lastx = cwidth;
				lasty = cheight;
			}
			return TRUE;

		case WM_EXITSIZEMOVE:
			InvalidateRect(hDlg, NULL, FALSE);
			UpdateWindow(hDlg);
			break;

		case WM_PAINT:
#define nSteps		64
			{	PAINTSTRUCT ps;
			    HDC hMemDC;
			    HWND hWnd = GetDlgItem(hDlg, IDC_OR_IMAGE);
			    HPEN open, pospen, negpen, curpen;
			    HBRUSH obrush;
			    HRGN brgn, orgn;
			    HBITMAP obmap;
			    HDC hDC;
			    RECT icr;
			    int i, cx, cy, cr, nplan = aTracked ? 10 : (orrInner ? 4 : 9),
			    	eplan = aTracked ? (orrInner ? 5 : 9) : (orrInner ? 4 : 9);
			    double diau = orrInner ? (aTracked ?
			       max(1.7, min(5.5, ((ast_info.SemiMajorAU * (1.0 + ast_info.Eccentricity)) * 1.1)))
			       		: 1.7) : 50.0;
			    double dialim = diau * 1.4;
			    char tbuf[60];
#ifdef saveOrbits
				static int oddone = FALSE;
				static short ods;
				static unsigned short uods;
				FILE *fp = NULL;
				
				if (!oddone) {
					oddone = TRUE;
					fp = fopen("orrdata.bin", "wb");
				}
#endif			    

                (void) BeginPaint(hDlg, &ps);
                hDC = GetDC(hWnd);
                sprintf(tbuf, Format(37), aTracked ? ": " : "",
                	aTracked ? ast_info.Name : "");
                SetWindowText(hDlg, tbuf);
            	GetClientRect(hWnd, &icr);
				if (orrbmap == NULL) {
					orrbmap = CreateCompatibleBitmap(hDC, icr.right, icr.bottom);
					orrbmapValid = FALSE;
				}
				hMemDC = CreateCompatibleDC(hDC);
				obmap = SelectObject(hMemDC, orrbmap);
	            cx = icr.right / 2;
	            cy = icr.bottom / 2;
	            cr = min(cx, cy);
				cr -= (MercOffset / 2);

	            if (!orrbmapValid) {
	                double plong, plat, prv, sjd, timestep;
	                int vx, vy, vz, fvx, fvy, ftf = TRUE, nsteps;
	                char spi[sizeof(planet_info)];
	            	HCURSOR ocurs = SetCursor(LoadCursor(NULL, IDC_WAIT));
//#define TimingTest
#ifdef TimingTest
					DWORD ticker = GetTickCount();
					DWORD asegments = 0;
#endif
					HANDLE hRes = NULL;
					unsigned short FAR *optr = NULL;

#ifdef useResource
 
					// Constrain use of canned orbits to decent interval around J2000.0
					
					if (abs((faketime - J2000) / JulianCentury) <= 5) {
						hRes = LoadResource(hInst,
									FindResource(hInst, "ORBITS", "ORRERYDATA"));
						if (hRes != NULL) {
							optr = LockResource(hRes);
						}
					}
#endif				
                	ShowCursor(TRUE);

		            FillRect(hMemDC, &icr, GetStockObject(WHITE_BRUSH));
		            open = SelectObject(hMemDC, GetStockObject(BLACK_PEN));
		            obrush = SelectObject(hMemDC, GetStockObject(NULL_BRUSH));

		            // Orrery is always heliocentric.  Paint the Sun at the centre.

	                DrawIcon(hMemDC, cx - 16, cy - 16, planetIcons[0]);

	                /* If we're tracking an asteroid, deduce its rotation period from
	                   the semimajor axis of its orbit. */

                    minperh = 0.28;                    
	                if (aTracked) {
	                	if (ast_info.cometary) {
	                		if (ast_info.PeriAU < minperh) {
	                			minperh = ast_info.PeriAU * 0.9;
	                		}
	                	} else {
	                		double pau = ast_info.SemiMajorAU * (1.0 - ast_info.Eccentricity);

	                		if (pau < minperh) {
	                			minperh = pau * 0.9;
	                		}
	                	}
	                }

	                // Plot the orbits of the planets

	                pospen = CreatePen(PS_SOLID, 0, RGB(0, 0, 255));	// Above the ecliptic pen
	                negpen = CreatePen(PS_SOLID, 0, RGB(0, 255, 0));	// Below the ecliptic pen
	                SelectObject(hMemDC, curpen = pospen);
	                for (i = 1; i <= min(nplan, 9); i++) {
	                	ftf = TRUE;

	                	/* Test for zero radius vector which indicates planetary theory not
	                	   known for this date (Pluto only).  If that's the case, skip
	                	   drawing the planet. */

	                 	if ((orrInner && (i > 4)) ||
	                 		((i != 3) && (planet_info[i].hrv == 0.0))) {
	                		continue;
	                	}
						plutoPrecise = FALSE;			// Allow suspect values to close Pluto orbit
	                	memcpy(spi, planet_info, sizeof spi);
	                	nsteps = nSteps;

	                	/* Draw the orbital path for the planet by stepping forward in
	                	   time one revolution period.  (N.b. for the planets whose
	                	   orbits are almost perfectly circular, it would be *enormously*
	                	   faster to just draw circles of the mean radius.  But for the
	                	   moment we do it the hard way for all of them.  This may be
	                	   slow, but it's *right*.)  */

                        timestep = rotationPeriod[i - 1] / nSteps;
						for (sjd = faketime; sjd <= faketime + rotationPeriod[i - 1]; sjd += timestep) {

							if (optr != NULL && i <= 9) {
								plong = (*optr++ * 360.0) / 65535.0;
								plat = (*((signed short FAR *) optr)++ * 180.0) / 32767.0;
								prv = (*optr++ * 50.0) / 65535.0;
							} else {
								if (i == 3) {
									double sunra, sundec;
	
									sunpos(sjd, TRUE, &sunra, &sundec, &prv, &plong);
									plong += 180;
									plat = 0;
		                		} else {
									planets(sjd, 1 << i);
		                			plong = planet_info[i].hlong;
		                			plat = planet_info[i].hlat;
		                			prv = planet_info[i].hrv;
		                		}
	                		}
#ifdef saveOrbits
							if (fp != NULL && i <= 9) {
								uods = (unsigned short) ((plong * 65535.0) / 360.0);
								fwrite(&uods, sizeof uods, 1, fp);
								ods = (short) ((plat * 32767.0) / 180.0);
								fwrite(&ods, sizeof ods, 1, fp);
								uods = (unsigned short) ((prv * 65535.0) / 50.0);
								fwrite(&uods, sizeof uods, 1, fp);
							}
#endif	                		
	                		if (prv > 0) {
								project(plong, plat, prv, cr, i, eplan, diau, &vx, &vy, &vz);
								vx += cx;
								vy = cy - vy;
							 	if (ftf) {
							 		MoveTo(hMemDC, fvx = vx, fvy = vy);
							 		ftf = FALSE;
							 	} else {
							 		if (plat < 0) {
							 			if (curpen != negpen) {
							 				SelectObject(hMemDC, curpen = negpen);
							 			}
							 		} else {
							 			if (curpen != pospen) {
							 				SelectObject(hMemDC, curpen = pospen);
							 			}
							 		}
							 		LineTo(hMemDC, vx, vy);
							 	}
							}
						}
						if (!ftf) {
							// Draw line to guarantee orbit is closed
							LineTo(hMemDC, fvx, fvy);
						}
						plutoPrecise = TRUE;
	                	memcpy(planet_info, spi, sizeof spi);
	                }

					/* If an asteroid or comet is being tracked, we have to handle it
					   specially.  First of all, we can never use the canned planetary
					   orbits to avoid calculation.  Second, unlike planetary orbits,
					   asteroidal and cometary orbits can have large eccentricity,
					   which means that we can't use a naive approximation to the
					   number of steps needed to draw the orbit smoothly, but must
					   instead take into account the orbital velocity at various
					   locations on the orbit and adjust the time step accordingly.
					   Finally, the orbit may not be closed either because it is
					   genuinely parabolic or hyperbolic, or merely such an eccentric
					   ellipse that the curve is truncated at the edge of the display. */

	                if (aTracked) {
	                	double eSmA, lprv, aprv;
	                	int arm, avx = 0, avy = 0, lvx, lvy;
	                	BOOL aclosed = FALSE;
	                
	                	i = 10;
	                	memcpy(spi, planet_info, sizeof spi);
                        
                        /* Because the orbit may not be closed, we start at the
                           perihelion date and plot into the future until we
                           either arrive at the aphelion or we run into the
                           cutoff based on the scale of the display.  Then we go
                           back to the perihelion and plot backward until we
                           encounter the equivalent event on the other branch. */
                        
                        eSmA = ast_info.Eccentricity < 1.0 ? ast_info.SemiMajorAU :
                        		40;
                        		
                        for (arm = -1; arm <= 1; arm += 2) {    
	                		ftf = TRUE;					// No vector on first point
	                        sjd = ast_info.PeriDate;
	                        lprv = 0;
	
							while (TRUE) {
								double ov;
	
								planets(sjd, 1 << 10);
	                			plong = planet_info[i].hlong;
	                			plat = planet_info[i].hlat;
	                			prv = planet_info[i].hrv;
			                			
		                		if (prv > 0) {
		                			lvx = vx;
		                			lvy = vy;
									project(plong, plat, prv, cr, 10, eplan, diau, &vx, &vy, &vz);
									vx += cx;
									vy = cy - vy;
								 	if (ftf) {
								 		MoveTo(hMemDC, vx, vy);
								 		ftf = FALSE;
								 	} else {
								 		if (plat < 0) {
								 			if (curpen != negpen) {
								 				SelectObject(hMemDC, curpen = negpen);
								 			}
								 		} else {
								 			if (curpen != pospen) {
								 				SelectObject(hMemDC, curpen = pospen);
								 			}
								 		}
								 		LineTo(hMemDC, vx, vy);
								 	}
								}
								
								if (prv == 0 || prv > dialim || prv < lprv) {
//								if (prv < lprv || avx < 0 || avx >= icr.right ||
//												  avy < 0 || avy >= icr.bottom) {
									if (arm > 0) {
										// If orbit closed, connect ends of orbit
//										if (aclosed && prv <= lprv) {
										if (aprv <= lprv && prv <= lprv) {
											LineTo(hMemDC, avx, avy);
											LineTo(hMemDC, avx, avy);		// Set last pixel 
										}
									} else {
										aclosed = prv < lprv;
										aprv = prv;
										avx = lvx;
										avy = lvy;
									}
									break;
								}
								lprv = prv;
								
								/* Orbital velocity.  Note that since we plot both branches of
								   the orbit outward from the perihelion, we always overestimate
								   the average velocity for the segment, guaranteeing that the
								   curve will be smooth at the expense of a few extra segments. */
								   
								ov = 42.1219 * sqrt(1 / prv - 1 / (2 * eSmA));
//{ char s[80]; sprintf(s, "Prv = %.2g  Orbital velocity: %.2g km/sec\r\n", prv, ov); OutputDebugString(s); }

								/* Time step to move 0.1 AU in inner solar system, correspondingly
								   more in full system. */
								   
								sjd += arm * (orrInner ? 1 : (prv < 1.0 ? 1 : max(1, (eSmA / 5)))) *
										(((AstronomicalUnit / ov) / (24.0 * 60 * 60)) / 10);							
//{ char s[80]; sprintf(s, "Time step: %.2g days\r\n", ((AstronomicalUnit / ov) / (24.0 * 60 * 60)) / 10); OutputDebugString(s); }
#ifdef TimingTest
								asegments++;
#endif
							}
						}
	                	memcpy(planet_info, spi, sizeof spi);
	                }

	                SelectObject(hMemDC, open);
	                DeleteObject(pospen);
	                DeleteObject(negpen);
	                SelectObject(hMemDC, obrush);
	            	ShowCursor(FALSE);
	            	SetCursor(ocurs);
	                orbitsLast = faketime;
	                orrbmapValid = TRUE;
	                if (optr != NULL) {
				    	UnlockResource(hRes);
				    }
				    if (hRes != NULL) {
				    	FreeResource(hRes);
				    }
	                
#ifdef TimingTest
					{	char tbuf[80];
						long et = GetTickCount() - ticker;
						 
						sprintf(tbuf, "Orrery time: %ld.%03ld  Asegments = %ld\r\n",
							et / 1000, et % 1000, asegments);
						OutputDebugString(tbuf);
					}
#endif
                }
            	InvalidateRgn(hWnd, NULL, TRUE);		// Guarantee paint whole image
	            BitBlt(hDC, 0, 0, icr.right, icr.bottom, hMemDC, 0, 0, SRCCOPY);
	            SelectObject(hMemDC, obmap);
	            DeleteDC(hMemDC);
#ifdef saveOrbits
				if (fp != NULL) {
					fclose(fp);
				}
#endif

	            // Now that the orbits are done, plot the planets along them
                
                brgn = CreateRectRgn(0, 0, icr.right, icr.bottom);
                if (brgn != NULL) {
                	orgn = SelectObject(hDC, brgn);
                	DeleteObject(brgn);
                }
	            for (i = 1; i <= nplan; i++) {
	            	double plong, plat, prv;
	            	int vx, vy, vz;
	            	if (i == 3) {
	            		plong = earthlong;
	            		plat = 0;
	            		prv = earthrv;
	            	} else {
	            		plong = planet_info[i].hlong;
	            		plat = planet_info[i].hlat;
	            		prv = planet_info[i].hrv;
	            	}
	            	if ((orrInner && (i > 4 && i <= 9)) || (prv == 0.0)) {
	            		continue;			// Planetary theory for Pluto invalid at this time
	            	}
					project(plong, plat, prv, cr, i, eplan, diau, &vx, &vy, &vz);
	        		DrawIcon(hDC, cx + vx - 16, (cy - vy) - 16, PlanetIcon(i));
	            }
	            ReleaseDC(hWnd, hDC);
	            orreryLast = faketime;

				SetScrollPos(GetDlgItem(hDlg, IDC_OR_HSCROLL), SB_CTL, (int) orrLong, TRUE);
				SetScrollPos(GetDlgItem(hDlg, IDC_OR_VSCROLL), SB_CTL, (int) -orrLat, TRUE);
				SendDlgItemMessage(hDlg, IDC_OR_INNER, BM_SETCHECK, orrInner, 0L);
				SendDlgItemMessage(hDlg, IDC_OR_ALLPLAN, BM_SETCHECK, !orrInner, 0L);
				SendDlgItemMessage(hDlg, IDC_OR_REAL, BM_SETCHECK, orrScale == IDC_OR_REAL, 0L);
				SendDlgItemMessage(hDlg, IDC_OR_LOG, BM_SETCHECK, orrScale == IDC_OR_LOG, 0L);
				SendDlgItemMessage(hDlg, IDC_OR_EQUAL, BM_SETCHECK, orrScale == IDC_OR_EQUAL, 0L);
				EndPaint(hDlg, &ps);
			}
			return TRUE;

		case WM_VSCROLL:
			switch (LOWORD(wParam)) {
				case SB_BOTTOM:
					orrLat = -90;
					break;

				case SB_TOP:
					orrLat = 90;
					break;

				case SB_THUMBTRACK:
					orrLat = -((short) HIWORD(wParam));
					{
			    		HWND hWnd = GetDlgItem(hDlg, IDC_OR_IMAGE);
			    		HPEN open;
			    		HBRUSH obrush;
			    		HRGN brgn, orgn;
			    		HDC hDC;
			    		RECT icr;
			    		int cx, cy, cr, ch;

                		hDC = GetDC(hWnd);
            			GetClientRect(hWnd, &icr);
		                brgn = CreateRectRgn(0, 0, icr.right, icr.bottom);
		                if (brgn != NULL) {
		                	orgn = SelectObject(hDC, brgn);
		                	DeleteObject(brgn);
		                }
	            		FillRect(hDC, &icr, GetStockObject(WHITE_BRUSH));
			            cx = icr.right / 2;
			            cy = icr.bottom / 2;
			            cr = min(cx, cy);
						cr -= (MercOffset / 2);
			            open = SelectObject(hDC, GetStockObject(BLACK_PEN));
			            obrush = SelectObject(hDC, GetStockObject(NULL_BRUSH));
		                DrawIcon(hDC, cx - 16, cy - 16, planetIcons[0]);
		                ch = (int) (cr * sin(dtr(orrLat)));
		                Ellipse(hDC, cx - cr, cy - ch, cx + cr, cy + ch);
                		SelectObject(hDC, open);
                		SelectObject(hDC, obrush);
	            		ReleaseDC(hWnd, hDC);
					}
					break;

				case SB_THUMBPOSITION:
					orrLat = -((short) HIWORD(wParam));
					break;

				case SB_LINEDOWN:
					orrLat -= 15;
					if (orrLat < -90) {
						orrLat = -90;
					}
					break;

				case SB_LINEUP:
					orrLat += 15;
					if (orrLat > 90) {
						orrLat = 90;
					}
					break;

				case SB_PAGEDOWN:
					orrLat -= 45;
					if (orrLat < -90) {
						orrLat = -90;
					}
					break;

				case SB_PAGEUP:
					orrLat += 45;
					if (orrLat > 90) {
						orrLat = 90;
					}
					break;

				case SB_ENDSCROLL:
					orrbmapValid = FALSE;
					InvalidateRect(hDlg, NULL, FALSE);
					break;
			}
			SetScrollPos((HWND) lParam, SB_CTL, -orrLat, TRUE);
			Ifld(IDC_OR_LAT, orrLat);
			return TRUE;

		case WM_HSCROLL:
			switch (LOWORD(wParam)) {
				case SB_LEFT:
					orrLong = 0;
					break;

				case SB_RIGHT:
					orrLong = 180;
					break;

				case SB_THUMBTRACK:
					orrLong = (short) (HIWORD(wParam));
					{
			    		HWND hWnd = GetDlgItem(hDlg, IDC_OR_IMAGE);
			    		HPEN open;
			    		HBRUSH obrush;
			    		HRGN brgn, orgn;
			    		HDC hDC;
			    		RECT icr;
			    		int cx, cy, cr, ch, vx, vy, vz, ooscale = orrScale;

                		hDC = GetDC(hWnd);
            			GetClientRect(hWnd, &icr);
		                brgn = CreateRectRgn(0, 0, icr.right, icr.bottom);
		                if (brgn != NULL) {
		                	orgn = SelectObject(hDC, brgn);
		                	DeleteObject(brgn);
		                }
	            		FillRect(hDC, &icr, GetStockObject(WHITE_BRUSH));
			            cx = icr.right / 2;
			            cy = icr.bottom / 2;
			            cr = min(cx, cy);
						cr -= (MercOffset / 2);
			            open = SelectObject(hDC, GetStockObject(BLACK_PEN));
			            obrush = SelectObject(hDC, GetStockObject(NULL_BRUSH));
		                DrawIcon(hDC, cx - 16, cy - 16, planetIcons[0]);
		                ch = (int) (cr * sin(dtr(orrLat)));
		                Ellipse(hDC, cx - cr, cy - ch, cx + cr, cy + ch);
		                orrScale = IDC_OR_EQUAL;
						project(earthlong, 0.0, 1.0, cr, 1, 1, 50.0, &vx, &vy, &vz);
            			DrawIcon(hDC, cx + vx - 16, (cy - vy) - 16, planetIcons[3]);
                		SelectObject(hDC, open);
                		SelectObject(hDC, obrush);
	            		ReleaseDC(hWnd, hDC);
	            		orrScale = ooscale;
					}
					break;

				case SB_THUMBPOSITION:
					orrLong = (short) (HIWORD(wParam));
					break;

				case SB_LINELEFT:
					orrLong -= 15;
					if (orrLong < -180) {
						orrLong = -180;
					}
					break;

				case SB_LINERIGHT:
					orrLong += 15;
					if (orrLong > 180) {
						orrLong = 180;
					}
					break;

				case SB_PAGELEFT:
					orrLong -= 45;
					if (orrLong < -180) {
						orrLong = -180;
					}
					break;

				case SB_PAGERIGHT:
					orrLong += 45;
					if (orrLong > 180) {
						orrLong = 180;
					}
					break;

				case SB_ENDSCROLL:
					orrbmapValid = FALSE;
					InvalidateRect(hDlg, NULL, FALSE);
					break;
			}
			SetScrollPos((HWND) wParam, SB_CTL, orrLong, TRUE);
			Ifld(IDC_OR_LONG, orrLong);
			return TRUE;

        case WM_COMMAND:
            switch (WM_COMMAND_ID(wParam)) {

                case IDOK:
                    SendMessage(hDlg, WM_CLOSE, 0, 0L);
                    return TRUE;

                case IDC_OR_HELP:
                	WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                				((DWORD) ((LPSTR) rstring(IDS_HELP_ON_ORRERY))));
                	holped = TRUE;
                	return TRUE;

                case IDC_OR_INNER:
                	orrbmapValid = FALSE;
                	orrInner = TRUE;
                	InvalidateRect(hDlg, NULL, FALSE);
                	return TRUE;

                case IDC_OR_ALLPLAN:
                	orrbmapValid = FALSE;
                	orrInner = FALSE;
                	InvalidateRect(hDlg, NULL, FALSE);
                	return TRUE;

               	case IDC_OR_REAL:
               	case IDC_OR_LOG:
               	case IDC_OR_EQUAL:
               		if (wParam != (WPARAM) orrScale) {
               			orrbmapValid = FALSE;
               			orrScale = wParam;
               			InvalidateRect(hDlg, NULL, FALSE);
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
