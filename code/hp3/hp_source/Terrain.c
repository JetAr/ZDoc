/*

	Draw synthetic terrain

*/

#include "Sunclock.h"

/*  Globals exported.  */

BOOL terrainSeeded = FALSE; 		/* Initial seed computed ? */

/*	Local variables  */

static int nrand;				   /* Gauss() sample count */
static double arand, gaussadd, gaussfac; /* Gaussian random parameters */
static unsigned rseed;			   /* Current random seed */
static int xdots, ydots;		   /* Graphics screen size */
static double fracdim = 0.7;	   /* Fractal dimension */
static double power = 1.0;		   /* Power law scaling exponent */
static double elevfac = 1.0;	   /* Elevation factor */

/*	FOUR1  --  One dimensional discrete fast Fourier transform.

			   As given in Press, et al, Numerical Recipes in C,
			   Second Edition, section 12.2.
*/

#define SWAP(a,b) tempr=(a); (a)=(b); (b)=tempr

static void four1(float *data, int nn, int isign)
{
	int n, mmax, m, j, istep, i;
	double wtemp, wr, wpr, wpi, wi, theta;
	float tempr, tempi;

	n = nn << 1;
	j = 1;
	for (i = 1; i < n; i += 2) {
	   if (j > i) {
		  SWAP(data[j], data[i]);
		  SWAP(data[j + 1], data[i + 1]);
	   }
	   m = n >> 1;
	   while (m >= 2 && j > m) {
		  j -= m;
		  m >>= 1;
	   }
	   j += m;
	}
	mmax = 2;
	while (n > mmax) {
	   istep = 2 * mmax;
	   theta = 6.28318530717959 / (isign * mmax);
	   wtemp = sin(0.5 * theta);
	   wpr = -2.0 * wtemp * wtemp;
	   wpi = sin(theta);
	   wr = 1.0;
	   wi = 0.0;
	   for (m = 1; m < mmax; m += 2) {
		  for (i = m; i <= n; i += istep) {
			 j = i + mmax;
			 tempr = (float) (wr * data[j] - wi * data[j + 1]);
			 tempi = (float) (wr * data[j + 1] + wi * data[j]);
			 data[j] = data[i] - tempr;
			 data[j + 1] = data[i + 1] - tempi;
			 data[i] += tempr;
			 data[i + 1] += tempi;
		  }
		  wr = (wtemp = wr) * wpr - wi * wpi + wr;
		  wi = wi * wpr + wtemp * wpi + wi;
	   }
	   mmax = istep;
	}
}
#undef SWAP

/*	INITGAUSS  --  Initialise random number generators.  As given in
				   Peitgen & Saupe, page 77.  */

static void initgauss(void)
{
	nrand = 4;						// Terms to sum for gaussian noise
	arand = pow(2.0, 15.0) - 1.0;	// Range of random generator
	gaussadd = sqrt(3.0 * nrand);
	gaussfac = 2 * gaussadd / (nrand * arand);
}

/*	GAUSS  --  Return a Gaussian random number.  As given in Peitgen
			   & Saupe, page 77.  */

static double gauss(void)
{
	int i;
	double sum = 0.0;

	for (i = 1; i <= nrand; i++) {
	   sum += rand();
	}
	return gaussfac * sum - gaussadd;
}

/*	INITSEED  --  Generate initial random seed, if needed.
				  The operation of this function is somewhat subtle.
				  If terrainSeeded is FALSE, we check whether
				  everSeeded is also FALSE.  If so, we've never been
				  called before, and we perform the full initialisation
				  and primary seed generation.  If everSeeded is TRUE, we
				  simply create a new seed from the next value from the
				  generator, set rseed to it, and set terrainSeeded.
				  If terrainSeeded is TRUE, we re-seed the generator with
				  the current setting of rseed, which causes multiple
				  terrain generations to be the same as long as terrainSeeded
				  remains set.  This keeps the terrain from changing as the
				  horizon window is updated.
*/

static void initseed(void)
{
	if (!terrainSeeded) {
	   static BOOL everSeeded = FALSE;
	   int i;

	   if (!everSeeded) {
		  i = (int) (time((long *) NULL) * 0xF37C);
		  srand(i);
		  for (i = 0; i < 7; i++) {
			 V rand();
		  }
		  rseed = rand();
		  everSeeded = terrainSeeded = TRUE;
		  initgauss();
	   } else {
		  rseed = rand();
		  srand(rseed);
		  terrainSeeded = TRUE;
	   }
	} else {
		srand(rseed);
	}
}

/*	SPECTRAL1D	--	One dimensional fractal profile synthesis by
					spectral synthesis from Brownian motion.  */

static void spectral1d(float **x, int n, double h)
{
		int i, j = 1;
		double beta = 2 * h + 1, rad, phase;
		float *a;

		*x = a = (float *) LocalAlloc(LPTR, (n * 2 + 1) * sizeof(float));
		if (a == NULL)
		   return;
		for (i = 0; i < (n / 2); i++) {
		   rad = pow((double) (i + 1), -beta / 2) * gauss();
		   phase = 2 * PI * (rand() / arand);
		   a[j++] = (float) (rad * cos(phase));
		   a[j++] = (float) (rad * sin(phase));
		}
		four1(a, n, -1);
}

/*  DRAWTERRAIN  --  Draw synthetic terrain at bottom of window.  */

void drawTerrain(HWND hWnd, HDC hDC, double jd, double azimuth,
				 double roughness, int scenery)
{
	RECT cr;
	float *a, *r;
	int i, np;
	double ymin = 1E50 , ymax = -1E50;
	HPEN dpen, open;
	
	initseed();
	GetClientRect(hWnd, &cr);
	xdots = cr.right - cr.left;
	ydots = cr.bottom - cr.top;
	 
	fracdim = roughness;
	for (i = 1; (1 << i) < xdots; i++) ;
	np = 1 << i; 
	spectral1d(&a, np, 2.0 - fracdim);
    
    if (a != NULL) {
    
    	// Apply the power factor and elevation scaling, and plot terrain
    
		r = a;
		for (i = 0; i < xdots; i++) {
		   double cr = *r;
	
		   if ((power != 1.0) && (cr > 0.0)) {
			  *r = (float) (cr = pow(cr, power));
		   }
	       *r = (float) (cr = cr * elevfac * (ydots / 20.0));
		   ymin = min(ymin, cr);
		   ymax = max(ymax, cr);
		   r += 2;
		}
	    
	    dpen = CreatePen(PS_SOLID, 0, RGB(0, 128, 0));
	    open = SelectObject(hDC, dpen);
	    for (i = 0; i < xdots; i++) {
	    	int ey;
	    	
	    	MoveTo(hDC, cr.left + i, ey = cr.bottom - ((ydots / 20) + ((int) a[i * 2])));
	    	LineTo(hDC, cr.left + i, cr.bottom);
	    	a[i * 2] = (float) min(ey, cr.bottom - 1);	// Save terrain top for icon positioning below
	    }
	    
	    // Sprinkle interesting icons along the horizon
	    
	    if (scenery) {
			WORD numIcons, iconSize, iconDensity;
            
            numIcons = 0;
			sceneryInit(jd, siteLat, siteLon, azimuth, ydots, xdots,
						rand(), &numIcons, &iconSize, &iconDensity);

            if (numIcons > 0) {
			    for (i = 0; i < xdots; i++) {
			    	if ((rand() % (iconSize * iconDensity)) == 0) {
			    		HICON h = NULL;
						int j, my = -1;
						WORD imgwid;
							
						for (j = (cr.left + i) - (iconSize / 2);
							 j < (cr.left + i) + (int) (iconSize / 2); j++) {
							if (j >= 0 && j < cr.right) {
								int ey = (int) a[j * 2];
								my = max(my, ey);
							}
						}
						my = (int) ((my - iconSize) +
										((rand() * ((long) (cr. bottom - my))) / RAND_MAX)); 
						sceneryIcon(i, my, rand(), &h, &imgwid);
						if (h != NULL) { 
							DrawIcon(hDC, cr.left + i, cr.top + my, h);
							DestroyIcon(h);
							i += imgwid;			// Avoid collision between adjacent icons
						}  
					}    		                                                          		
			    }
			}
			sceneryTerm();
		}
		SelectObject(hDC, open);
		DeleteObject(dpen);
		
		LocalFree(a);
	}
}
