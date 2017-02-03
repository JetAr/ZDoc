/*

    Orbital element scanner interface definitions

*/

#define ERR_BADSATNAME  1
#define ERR_UNEXPEOF    2
#define ERR_CKSUM       3
#define ERR_FORMAT      4

/* The  "Official"  element  sets  limit  the  satellite  name  to  12
   characters, but other, less formal sets, such as the TVRO satellite
   elements  posted  to  rec.video.satellite  use  longer  names.  The
   following  definition  lets  you  specify  the  longest  name   and
   compensate for this.  */

#define MAX_Sat_Name_Length 39

/*  Satellite data structure.  */

struct satellite {
    char satname[MAX_Sat_Name_Length + 1]; /* Satellite name */
    long number;                      /* Satellite number */
    struct {                          /* International designator */
        int launchyear;               /* Last 2 digits of launch year */
        int launchno;                 /* Launch number of the year */
        char pieceno[4];              /* Piece number of launch */
    } intdes;
    int epochyear;                    /* Last two digits of epoch year */
    double epochday;                  /* Julian day and fraction of epoch */
    double meanmotiond1;              /* First time derivative of mean
                                         motion */
    double meanmotiond2;              /* Second time derivative of mean
                                         motion */
    double dragterm;                  /* BSTAR drag term or radiation
                                         pressure coefficient. */
    int ephtype;                      /* Ephemeris type */
    int elemnumber;                   /* Element number */

    double inclination;               /* Inclination in degrees */
    double rascendnode;               /* Right ascension of ascending node
                                         in degrees */
    double eccentricity;              /* Eccentricity */
    double argperigee;                /* Argument of perigee in degrees */
    double meananomaly;               /* Mean anomaly in degrees */
    double meanmotion;                /* Mean motion in revs per day */
    long revno;                       /* Revolution number at epoch */
};

extern void insat(FILE *in, int (*dosat)(struct satellite *sat),
		          int *err, int *lineerr);
