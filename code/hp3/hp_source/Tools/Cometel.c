/*

    Convert comet elements in the formats used by the Minor Planet
    Center (and in the Minor Planet Electronic Circulars) to Home
    Planet CSV format.  Multiple comet announcements can be extracted
    from a file containing other irrelevant information.  This allows
    you, for example, to process an entire Unix mail folder and extract
    elements from all archived circulars within.
    
    Designed and implemented by John Walker in November of 1994.
    Revision 1 by John Walker in April 1997 to use presence of
    eccentricity to flag periodic comet format.

                     Elliptical Orbital Elements
                     ---------------------------
    
PERIODIC COMET SHOEMAKER 4 (1994k)

            . . .
 
     T = 1994 Oct. 31.231 TT          Peri. = 196.521
     e = 0.52851                      Node  =  92.603   2000.0
     q = 2.92503 AU                   Incl. =  25.345
       a =  6.20375 AU     n = 0.063786     P =  15.45 years
          1         2         3         4         5         6         7
01234567890123456789012345678901234567890123456789012345678901234567890

    Some time after the original version of this program was released,
    IAUCs dropped the "PERIODIC " designation, relying on the presence
    or absence of eccentricity to distinguish elliptical and parabolic
    elements.  The form for periodic elements as of this revision
    (April 1997) is as follows:

COMET P/1997 B1 (KOBAYASHI)

            . . .

     T = 1997 Mar.  4.295 TT          Peri. = 184.202
     e = 0.76831                      Node  = 329.032   2000.0
     q = 2.06219 AU                   Incl. =  12.490
       a =  8.90047 AU     n = 0.037118     P =  26.55 years
          1         2         3         4         5         6         7
01234567890123456789012345678901234567890123456789012345678901234567890

                      Parabolic Orbital Elements
                      --------------------------

COMET MUELLER (1994c)
                                                                           
                . . .
                                                                                             
     T = 1993 Dec.  3.990 TT          Peri. = 102.512
                                      Node  =   4.933   2000.0
     q = 1.81118 AU                   Incl. = 145.454
          1         2         3         4         5         6         7
01234567890123456789012345678901234567890123456789012345678901234567890

                            Output Format
                            -------------
    
Name,Perihelion time,Perihelion AU,Eccentricity,Long. perihelion,Long. node,Inclination,Semimajor axis,Period
P/1996 P1 (IRAS),1996-10-29.9968,1.702690,0.696589,356.8876,357.7007,45.9617,5.611833 AU,13.294 years
C/1996 Q1 (TABUR),1996-11-3.562,0.84202,1,57.234,31.516,73.234,,Parabolic
C/1996 P2 (RUSSELL-WATSON),1996-2-29.518,2.01067,1,188.661,101.556,29.134,,Parabolic
P/1996 N2 (ELST-PIZARRO),1996-4-16.0159,2.617717,0.170415,132.4528,160.3483,1.3754,3.155453 AU,5.605 years
C/1996 R1 (HERGENROTHER-SPAHR),1996-8-28.566,1.90096,1,139.250,149.648,145.876,,Parabolic
P/1996 R2 (LAGERKVIST),1997-1-30.55,2.4783,0.3685,339.83,40.72,2.55,3.9245 AU,7.77 years
P/1996 S2 (SHOEMAKER-HOLT 1),1997-11-20.2541,3.047026,0.321273,210.2130,214.5275,4.3617,4.489325 AU,9.512 years
P/1996 U1 (SHOEMAKER-LEVY 3),1998-3-4.9475,2.817332,0.247914,181.3886,303.7122,5.0086,3.746025 AU,7.250 years
    
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#ifdef unix
#define _strnicmp strncasecmp
#define READMODE "r"
#else
/*  On DOS-like systems we read the input file in binary mode
    so that a ^Z embedded in a Unix mail folder (they get there
    when a DOS user embeds a file in a mail message) doesn't
    truncate the input.  */
#define READMODE "rb"
#endif

/*  Define ECHO to embed the source lines from which each line of
    CSV was generated.  This is handy for debugging when the format
    changes.  */

#ifdef ECHO
#define ECHOLINE(x) strcpy(s[x], s[0])
#else
#define ECHOLINE(x)
#endif

static char s[8][132];                  /* Buffer to hold complete entry */

static int rs(char *s, FILE *fp)
{
    if (fgets(s, 132, fp) == NULL) {
        return 0;
    }
    while (strlen(s) > 0 && s[strlen(s) - 1] < ' ') {
        s[strlen(s) - 1] = 0;
    }
    return 1;
}

static void sc(char *b, int line, int start, int end)
{
    char *c = s[line];
    int i, n;
    
    for (i = 0, n = start; n <= end; n++) {
        if (c[n] == 0) {
            break;
        }
        if (!isspace(c[n])) {
            b[i++] = c[n];
        }
    }
    b[i] = 0;
}


static void sct(char *b, int line, int start, int end)
{
    char *c = s[line], sb[132];

    memcpy(sb, c + start, (end - start) + 1);
    sb[(end - start) + 1] = 0;
    while (strlen(sb) > 0 && isspace(sb[strlen(sb) - 1])) {
        sb[strlen(sb) - 1] = 0;
    }
    c = sb;
    while (*c && isspace(*c)) {
        c++;
    }
    strcpy(b, c);
}


int main(int argc, char *argv[])
{
    int i, running = 1, written = 0;
    FILE *fi = stdin, *fo = stdout;         /* Input and output file names */
    static char cname[80], epoch[80], periau[80], perilong[80],
                eccen[80], longnode[80], inclin[80], semimaj[80], period[80];
    
    if (argc > 1) {
        fi = fopen(argv[1], READMODE);      /* Input file */
        if (fi == NULL) {
            fprintf(stderr, "Cannot open input file %s\n", argv[1]);
            return 2;
        }
        if (argc > 2) {
            fo = fopen(argv[2], "w");       /* Output file */
            if (fo == NULL) {
                fprintf(stderr, "Cannot open output file %s\n", argv[2]);
                return 2;
            }
        }
    }

    fprintf(fo, "Name,Perihelion time,Perihelion AU,Eccentricity,Long. perihelion,\
Long. node,Inclination,Semimajor axis,Period\n");
    
    while (running) {
        int periodic = -1;
        char *u, *v;
        
        while (1) {
            if (!rs(s[0], fi)) {
                running = 0;
                break;
            }
            if (strncmp(s[0], "COMET ", 6) == 0) {
                sct(cname, 0, 6, 80);
                periodic = 0;
                break;  
            } else if (strncmp(s[0], "PERIODIC COMET ", 15) == 0) {
                sct(cname, 0, 15, 80);
                periodic = 1;
                break;
            } 
        }
        
        if (!running) {
            break;
        }

        ECHOLINE(1);
        
        /* Scan until we either find a line which begins with the "T ="
           specification or run into another comet header (indicating
           this circular didn't contain elements. */
           
        while (1) {
            char t[132];
        
            if (!rs(s[0], fi)) {
                running = 0;
                break;
            }
            if (strncmp(s[0], "COMET ", 6) == 0) {
                sct(cname, 0, 6, 80);
                periodic = 0;
                ECHOLINE(1);
                continue;  
            } else if (strncmp(s[0], "PERIODIC COMET ", 15) == 0) {
                sct(cname, 0, 15, 80);
                periodic = 1;
                ECHOLINE(1);
                continue;
            }
            ECHOLINE(2);
            sc(t, 0, 0, 80);
            if (strncmp(t, "T=", 2) == 0 && isdigit(t[2])) {
                char y[20], m[20], daf[20];
                static char mname[] = "janfebmaraprmayjunjulaugsepoctnovdec";
                
                sscanf(u = strchr(s[0], '=') + 1, "%s %s", y, m);
                for (i = 0; i < 12; i++) {
                    if (_strnicmp(m, mname + i * 3, 3) == 0) {
                        sprintf(m, "%d", i + 1);
                        break;
                    }
                }
                
                /* Tiptoe up to the start of the year, since in the
                   case of "Sept.199x" the month runs into the first
                   digit of the year.  This code is resiliant in case
                   a space is added later. */
                
                while (*u && isspace(*u)) {
                    u++;
                }
                while (*u && !isspace(*u)) {
                    u++;
                }
                while (*u && isspace(*u)) {
                    u++;
                }
                while (*u && !isdigit(*u)) {
                    u++;
                }
                sscanf(u, "%s", daf);
                sprintf(epoch, "%s-%s-%s", y, m, daf);
                v = strchr(u, '=');
                if (v != NULL) {
                    sc(perilong, 0, (v + 1) - s[0], 80);
                } else {
                    continue;
                }
                
                /* Process second line. */
                
                if (!rs(s[0], fi)) {
                    running = 0;
                    break;
                }
                ECHOLINE(3);
                sc(t, 0, 0, 80);
                if (strncmp(t, "e=", 2) == 0) {
                    sscanf(u = strchr(s[0], '=') + 1, "%s", eccen);
                    v = strchr(u, '='); 
                    periodic = 1;
                } else {
                    strcpy(eccen, "1");         /* Parabolic */
                    v = strchr(s[0], '=');
                }
                if (v != NULL) {
                    sscanf(v + 1, "%s", longnode);
                } else {
                    continue;
                }
                
                /* Process third line. */
                
                if (!rs(s[0], fi)) {
                    running = 0;
                    break;
                }
                ECHOLINE(4);
                u = strchr(s[0], '=');
                if (u != NULL) {
                    v = strchr(u + 1, '=');
                }
                if (u != NULL && v != NULL) {
                    sscanf(u + 1, "%s", periau);
                    sscanf(v + 1, "%s", inclin);
                } else {
                    continue;
                }
                
                /* Process fourth line if comet is periodic. */
                
                strcpy(semimaj, "");
                strcpy(period, "Parabolic");
                if (periodic) {
                    if (!rs(s[0], fi)) {
                        running = 0;
                        break;
                    }
                    ECHOLINE(5);
                    u = strchr(s[0], '=');
                    if (u != NULL) {
                        v = strchr(u + 1, '=');
                        if (v != NULL) {
                            v = strchr(v + 1, '=');
                        }
                    }
                    if (u != NULL && v != NULL) {
                        char t1[30], t2[30];
                        
                        sscanf(u + 1, "%s %s", t1, t2);
                        sprintf(semimaj, "%s %s", t1, t2);
                        sscanf(v + 1, "%s %s", t1, t2);
                        sprintf(period, "%s %s", t1, t2);
                    } else {
                        continue;
                    }
                }
                 
                break;
            } 
        }
        
        if (!running) {
            break;
        }

#ifdef ECHO
        fprintf(fo, "\n\
          1         2         3         4         5         6         7\n\
01234567890123456789012345678901234567890123456789012345678901234567890\n");
        for (i = 1; i <= (periodic ? 5 : 4); i++) {
            fprintf(fo, "%s\n", s[i]);
        }
#endif
        
        /* Output information in CSV format. */

        fprintf(fo, "%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                    cname, epoch, periau, eccen, perilong, longnode, inclin,
                    semimaj, period);
        written++;                  
    }
    
    fclose(fi);
    fclose(fo);
    fprintf(stderr, "%d record%s written.\n", written, written == 1 ? "" : "s");        
    
    return 0;
}
