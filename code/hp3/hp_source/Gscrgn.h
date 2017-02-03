/*  Hubble guide star catalogue internal structure definitions. */

typedef struct {
    double lora, hira, lodec, hidec;
    double rawidth, decwidth;
} GSCRgnBounds, *GSCRgnBoundsPtr;

/* this structure is used to pass GSC region information */

typedef struct {
    int Band, LgRgn, SmlRgn;
    GSCRgnBounds Lg;
    GSCRgnBounds Sm;
} GSCRgnInfo, *GSCRgnInfoPtr;

/* GSCFolders gives GSC folder names for declination bands */

extern char *GSCFolders[25];

/* NumInBand gives the number of GSC large regions in a declination band */

extern char NumInBand[25];

/* FirstInBand gives the first large region number in a declination band */

extern int FirstInBand[25];

/* FirstSmlRgn gives the first small region number in a large region */

extern int FirstSmlRgn[732];

/* the square of NumSmlRgns gives the number of small regions in the large region */

extern char NumSmlRgns[732];

/* Prototypes for GSC catalogue access functions.  */

extern void GSCRegions(double ra, double dec, GSCRgnInfoPtr G);
extern void LargeRegionInfo(GSCRgnInfoPtr G);
extern void SmallRegionInfo(GSCRgnInfoPtr G);
extern int FindSmallRegion(int rgn, GSCRgnInfoPtr G);