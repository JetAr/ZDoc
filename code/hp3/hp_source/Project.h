
#define FALSE 0
#define TRUE 1

/* Projection Modes */
#define SANSONS 1
#define STEREOGR 2
#define GNOMONIC 3
#define ORTHOGR 4
#define RECTANGULAR 5
#define OTHERPROJ 6

/* The variables in the first few lines MUST be set by driver */
typedef struct {
  int width, height, x_offset, y_offset; /* Size and position,
                                            in integer device coords */

/* The next several variables may be set by the driver, but the main routines
   may reset them (and the driver routines may then override that) */
   
  int proj_mode;                    /* Projection mode for this map */

  int invert;                       /* Invert (flip north/south) */
  int mirror;						/* Mirror (flip east/west) */

/* The following are set by the main routines */
  double racen, dlcen, scale;       /* R.A. and decl. of center,
                                       scale in degrees */
  double c_scale;                   /* One second of arc
                                       in display units */
} mapwindow;

extern mapwindow skywin, telwin, horwin;// Sky, Telescope, and Horizon map windows
extern mapwindow *mapwin[3];			// Window list
extern int numwins;						// Number of windows

extern int initxform(mapwindow *win);
extern void xform(double lat, double lon, int *xloc, int *yloc, int *inregion);
extern int invxform(int x, int y, double *latp, double *lonp);
extern int clipr_xform(double lat1, double lon1, double lat2, double lon2,
				int *xloc1, int *yloc1, int *xloc2, int *yloc2, int great_circle,
				double *plat1, double *plon1, double *plat2, double *plon2);
extern void drawcurveline(double  lat1, double lon1, double lat2, double lon2,
				   int xloc1, int yloc1, int xloc2, int yloc2,
				   int line_style, int great_circle, int clevel);
extern void drawAltAzCurveline(double alt1, double az1, double alt2, double az2,
						double sitelat, double lham,
						int xloc1, int yloc1, int xloc2, int yloc2,
						int line_style, int clevel);

extern void D_movedraw(int xloc1, int yloc1, int xloc2, int yloc2, int line_style);