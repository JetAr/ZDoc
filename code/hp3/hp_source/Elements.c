/*

    Scan two-line orbital elements in NASA/NORAD format as defined below:

    From: tkelso@galaxy.afit.af.mil
          Dr TS Kelso
          Assistant Professor of Space Operations
    Organization: Air Force Institute of Technology
    0ubject: Two-Line Orbital Element Set Format

    As  a  service  to  the  satellite  user  community, the following
    description of the NORAD two-line orbital element  set  format  is
    uploaded  to  sci.space  and  rec.radio.amateur.misc  on a monthly
    basis.  The most current orbital elements from the NORAD  two-line
    element sets are carried on the Celestial BBS, (513) 427-0674, and
    are updated several  times  weekly.   Documentation  and  tracking
    software are also available on this system.  The Celestial BBS may
    be accessed 24 hours/day at 300, 1200, 2400,  4800,  or  9600  bps
    using  8  data  bits, 1 stop bit, no parity.  In addition, element
    sets (updated weekly) and some documentation and software are also
    available  via  anonymous  ftp  from  blackbird.afit.af.mil in the
    directory pub/space.

    Data for each satellite consists of three lines in  the  following
    format:

AAAAAAAAAAA
1 NNNNNU NNNNNAAA NNNNN.NNNNNNNN +.NNNNNNNN +NNNNN-N +NNNNN-N N NNNNN
2 NNNNN NNN.NNNN NNN.NNNN NNNNNNN NNN.NNNN NNN.NNNN NN.NNNNNNNNNNNNNN

    Line 0 is a eleven-character name.

    Lines 1 and 2 are the standard Two-Line Orbital Element Set Format
    identical to that used by NORAD and NASA.  The format  description
    is:

Line 1
Column     Description
 01-01     Line Number of Element Data
 03-07     Satellite Number
 10-11     International Designator (Last two digits of launch year)
 12-14     International Designator (Launch number of the year)
 15-17     International Designator (Piece of launch)
 19-20     Epoch Year (Last two digits of year)
 21-32     Epoch (Julian Day and fractional portion of the day)
 34-43     First Time Derivative of the Mean Motion
        or Ballistic Coefficient (Depending on ephemeris type)
 45-52     Second Time Derivative of Mean Motion (decimal point assumed;
           blank if N/A)
 54-61     BSTAR drag term if GP4 general perturbation theory was used.
           Otherwise, radiation pressure coefficient.  (Decimal point assumed)
 63-63     Ephemeris type
 65-68     Element number
 69-69     Check Sum (Modulo 10)
           (Letters, blanks, periods = 0; minus sign = 1)

Line 2
Column     Description
 01-01     Line Number of Element Data
 03-07     Satellite Number
 09-16     Inclination [Degrees]
 18-25     Right Ascension of the Ascending Node [Degrees]
 27-33     Eccentricity (decimal point assumed)
 35-42     Argument of Perigee [Degrees]
 44-51     Mean Anomaly [Degrees]
 53-63     Mean Motion [Revs per day]
 64-68     Revolution number at epoch [Revs]
 69-69     Check Sum (Modulo 10)

    All other columns are blank or fixed.

    Example:

NOAA 6
1 11416U          86 50.28438588 0.00000140           67960-4 0  5293
2 11416  98.5105  69.3305 0012788  63.2828 296.9658 14.24899292346978

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "elements.h"

#define FALSE   0
#define TRUE    1

#define EOS     '\0'

extern long atol();
extern double atof();

#define errout(n)   *err = n; *lineerr = lineno; return

/*  CKSUM  --  Validate checksum on line.  Returns TRUE if a checksum
               error is detected and FALSE if the line is OK.  */

static int cksum(char *line)
{
    int i, sum = 0;

    for (i = 0; i < 68; i++) {
        unsigned char ch = line[i];

        if (ch == '-') {
            ch = 1;
        } else if (ch >= '0' && ch <= '9') {
            ch -= '0';
        } else {
            ch = 0;
        }
        sum += ch;
    }
    if (line[68] < '0' || line[68] > '9' ||
        (line[68] - '0') != (sum % 10)) {
/* printf("%s\n*Sum %d, char: %c\n", line, sum, line[68]); */
        return TRUE;
    }
    return FALSE;
}

/*  TRIM  --  Trim trailing white space off lines.  We allow arbitrary
              white space at the end of lines, since that permits us to
              accept MS-DOS CR-LF terminated files even on Unix platforms. */

static void trim(char *line)
{
    while (strlen(line) > 0 && isspace(line[strlen(line) - 1])) {
        line[strlen(line) - 1] = EOS;
    }
}

/*  GETINT  --  Extract a (long) integer value from a line.  */

static long getint(char *line, int startcol, int endcol)
{
    char intbuf[20];
    int i;

    assert(startcol <= endcol);
    assert ((endcol - startcol) + 2 < sizeof intbuf);
    memcpy(intbuf, line + (startcol - 1), (endcol - startcol) + 1);
    intbuf[(endcol - startcol) + 1] = EOS;
    for (i = 0; i < (endcol - startcol) + 1; i++) {
        if (isspace(intbuf[i])) {
            intbuf[i] = '0';
        }
    }
/* printf("Int: %s\n", intbuf); */
    return atol(intbuf);
}

/*  GETSIGN  --  Process a sign.  */

static int getsign(char *line, int col)
{
    return (line[col - 1] == '-') ? -1 : 1;
}

/*  GETDOUBLE  --  Extract a double floating value from a line.  */

static double getdouble(char *line, int startcol, int endcol)
{
    char dblbuf[22];
    int i;

    assert(startcol <= endcol);
    assert ((endcol - startcol) + 3 < sizeof dblbuf);
    memcpy(dblbuf, line + (startcol - 1), (endcol - startcol) + 1);
    dblbuf[(endcol - startcol) + 1] = EOS;
    for (i = 0; i < (endcol - startcol) + 1; i++) {
        if (isspace(dblbuf[i])) {
            dblbuf[i] = '0';
        }
    }
    for (i = 0; i < (endcol - startcol) + 1; i++) {
        if (dblbuf[i] == '+' || dblbuf[i] == '-') {
            int j;

            for (j = (endcol - startcol) + 2; j > i; j--) {
                dblbuf[j] = dblbuf[j - 1];
            }
            dblbuf[i] = 'E';
            break;
        }
    }
/* printf("Double: %s\n", dblbuf); */
    return atof(dblbuf);
}

/*  PRESENT  --  Test if a given field is blank.  */

static int present(char *line, int startcol, int endcol)
{
    int i;

    assert(startcol <= endcol);
    for (i = startcol; i <= endcol; i++) {
        assert(line[i] != EOS);
        if (!isspace(line[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

#ifdef NEEDED

/*  OUTAMSAT  --  Output satellite description to a given file in AMSAT
                  format.  */

void outamsat(FILE *out, struct satellite *sat)
{
    fprintf(out, "Satellite: %s\n", sat->satname);
    fprintf(out, "Catalog number: %d\n", sat->number);
    fprintf(out, "Epoch time: %f\n",
                 (sat->epochyear * 1000.0) + sat->epochday);
    fprintf(out, "Element set: %d\n", sat->elemnumber);
    fprintf(out, "Inclination: %f deg\n", sat->inclination);
    fprintf(out, "RA of node: %f deg\n", sat->rascendnode);
    fprintf(out, "Eccentricity: %f\n", sat->eccentricity);
    fprintf(out, "Arg of perigee: %f deg\n", sat->argperigee);
    fprintf(out, "Mean anomaly: %f deg\n", sat->meananomaly);
    fprintf(out, "Mean motion: %f rev/day\n", sat->meanmotion);
    fprintf(out, "Decay rate: %e rev/day^2\n", sat->meanmotiond1);
    fprintf(out, "Epoch rev: %ld\n\n", sat->revno);
}
#endif

/*  INSAT  --  Process a file of satellite elements and call a function
               DOSAT for each set of elements.  If an error is detected,
               processing of the file is aborted, ERR is set to the
               error code, and LINEERR is set to the line number on which
               the error was detected.  */

void insat(FILE *in, int (*dosat)(struct satellite *sat),
		   int *err, int *lineerr)
{
    char sname[132], line1[132], line2[132];
    int lineno = 0;
    struct satellite sat;

    *err = 0;
    while (fgets(sname, sizeof sname, in)) {
        lineno++;

        /* Not-strictly-compatible extension: allow comments marked with a
           semicolon in column 1.  Comments may not be embedded within the
           body of a satellite description, only between satellites. */

        if (sname[0] == ';') {
        	continue;
        }

        trim(sname);
#ifdef ERR_ON_SATNAME_TOO_LONG
        if (strlen(sname) < 1 || strlen(sname) > MAX_Sat_Name_Length) {
            errout(ERR_BADSATNAME);
        }
#else
		sname[MAX_Sat_Name_Length] = 0;
#endif
		while (strlen(sname) > 0 && isspace(sname[strlen(sname) - 1])) {
#ifdef ERR_ON_SATNAME_TOO_LONG
			errout(ERR_BADSATNAME);
#else
			sname[strlen(sname) - 1] = 0;
#endif
		}
		if (strlen(sname) < 1) {
			continue;
		}
        lineno++;
        if (fgets(line1, sizeof line1, in) == NULL) {
            errout(ERR_UNEXPEOF);
        }
        trim(line1);
        /* The test for "U" in column 8 is commented out below because I've
           encountered elements posted in the Space Forum on CompuServe which
           are otherwise correct but missing the "U". */
        if (line1[0] != '1' || /* line1[7] != 'U' || */ strlen(line1) < 69) {
            errout(ERR_FORMAT);
        }
        if (cksum(line1)) {
            errout(ERR_CKSUM);
        }
        lineno++;
        if (fgets(line2, sizeof line2, in) == NULL) {
            errout(ERR_UNEXPEOF);
        }
        trim(line2);
        if (line2[0] != '2' || strlen(line2) < 69) {
            errout(ERR_FORMAT);
        }
        if (cksum(line2)) {
            errout(ERR_CKSUM);
        }

        /* OK, it seems to be valid.  Extract the fields into a
           satellite structure. */

        memset(&sat, 0, sizeof sat);				/* Clear structure to allow equality tests */
        strcpy(sat.satname, sname);
        sat.number = getint(line1, 3, 7);
        sat.intdes.launchyear = (int) getint(line1, 10, 11);
        sat.intdes.launchno = (int) getint(line1, 12, 14);
        memcpy(sat.intdes.pieceno, line1 + 14, 3);
        sat.intdes.pieceno[3] = EOS;
        sat.epochyear = (int) getint(line1, 19, 20);
        sat.epochday = getdouble(line1, 21, 32);
        sat.meanmotiond1 = getsign(line1, 34) * getdouble(line1, 35, 43);
        if (present(line1, 45, 52)) {
            sat.meanmotiond2 = getsign(line1, 45) * getdouble(line1, 46, 52) *
                               10e-5;
        }
        sat.dragterm = getsign(line1, 54) * getdouble(line1, 55, 61) *
                               10e-5;
        sat.ephtype = (int) getint(line1, 63, 63);
        sat.elemnumber = (int) getint(line1, 65, 68);

        if (sat.number != getint(line2, 3, 7)) {
            errout(ERR_FORMAT);
        }
        sat.inclination = getdouble(line2, 9, 16);
        sat.rascendnode = getdouble(line2, 18, 25);
        sat.eccentricity = getint(line2, 27, 33) * 1.0e-7;
        sat.argperigee = getdouble(line2, 35, 42);
        sat.meananomaly = getdouble(line2, 44, 51);
        sat.meanmotion = getdouble(line2, 53, 63);
        sat.revno = getint(line2, 64, 68);

        if (!dosat(&sat)) {
        	break;
        }
    }
}
