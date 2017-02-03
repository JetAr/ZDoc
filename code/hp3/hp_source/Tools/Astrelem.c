/*

    Convert asteroid elements in the 8-line format used
    by the Minor Planet Center (and in the Minor Planet
    Electronic Circulars) to our CSV format.
    
    Designed and implemented by John Walker in October of 1994.
    
     (4646) Kwee
    Epoch 1993 Jan. 13.0 TT = JDT 2449000.5  (M-P)          Oishi
    M 220.20030              (2000.0)            P               Q
    n   0.26003234     Peri.  350.28914     +0.79481320     +0.60665475
    a   2.4309973      Node   332.34432     -0.55450494     +0.71559595
    e   0.1921938      Incl.    1.92064     -0.24656897     +0.34625491
    P   3.79           H   14.0           G   0.15
    From observations at 4 oppositions, 1960-1990.  Ref. MPC 17195.
              1         2         3         4         5         6         7
    01234567890123456789012345678901234567890123456789012345678901234567890
    
    Name,Magnitude H,Magnitude G,Mean anomaly,Arg. perihelion,Long. node,Inclination,Eccentricity,Semimajor axis,Epoch (MJD)
    AALTJE 677,9.70,0.15,47.68385,272.29565,272.71469,8.48713,0.04530949,2.95528649,48600

    
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#ifdef unix
#define READMODE "r"
#else
/*  On DOS-like systems we read the input file in binary mode
    so that a ^Z embedded in a Unix mail folder (they get there
    when a DOS user embeds a file in a mail message) doesn't
    truncate the input.  */
#define READMODE "rb"
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

static void sc(char *b, int line, int start, int end) {
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

int main(int argc, char *argv[])
{
    int i, p, running = 1, recov, written = 0;
#define IBN 4   
    static char ib[IBN][132];
    FILE *fi = stdin, *fo = stdout;         /* Input and output file names */
#ifdef EDITMODE
    FILE *fn = NULL;
#endif  
    static char anum[80], aname[80], epoch[80], meanan[80], semimaj[80],
        eccen[80], argperi[80], longnode[80], inclin[80], magh[80], magg[80];
    
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
#ifdef EDITMODE         
            if (argc > 3) {
                fn = fopen(argv[3], "w");   /* Edit mode file */
                if (fn == NULL) {
                    fprintf(stderr, "Cannot open numeric format output file %s\n", argv[3]);
                    return 2;
                }
            }
#endif          
        }
    }

#ifdef EDITMODE 
    fprintf(fo, "Number,Name,Number Name,Name Number,Magnitude H,Magnitude G,Mean anomaly,Arg. perihelion,Long. node,Inclination,Eccentricity,Semimajor axis,Epoch (MJD)\n");
    if (fn != NULL) {
        fprintf(fn, "Number,Name,Magnitude H,Magnitude G,Mean anomaly,Arg. perihelion,Long. node,Inclination,Eccentricity,Semimajor axis,Epoch (MJD)\n");
    }
#else
    fprintf(fo, "Name,Magnitude H,Magnitude G,Mean anomaly,Arg. perihelion,Long. node,Inclination,Eccentricity,Semimajor axis,Epoch (MJD)\n");
#endif
    
    while (1) {
        char *u, *v;

        p = 0;
        while (1) {
#define ibm(x)  ib[(p + (IBN - (x))) % IBN]
            if (!rs(ib[p], fi)) {
                running = 0;
                break;
            }
            if (strncmp(ib[p], "Epoch ", 6) == 0 &&
                strstr(ib[p], "TT") != NULL &&
                strchr(ib[p], '=') != NULL &&
                strstr(ib[p], "JDT") != NULL) {
        
                /* MPECs for recovered objects contain an "Id." line after the name
                   and before the Epoch, instead of a "From" line at the end of the
                   elements.  If this is the case with these elements, move the ID
                   to the end of the elements and shift everything else up one line. */
                   
                recov = strncmp(ibm(1), "Id", 2) == 0;

                strcpy(s[0], ibm(recov ? 2 : 1));
                strcpy(s[1], ib[p]);
                for (i = 0; i < 5; i++) {
                    if (!rs(s[i + 2], fi)) {
                        running = 0;
                        break;
                    }
                }
                p = (p + 1) % IBN; 
                break;
            }
            p = (p + 1) % IBN; 
        }
        if (!running) {
            break;
        }       
        
        /*  Scan asteroid number, if one has been assigned. */
        
        if ((u = strchr(s[0], '(')) != NULL && (v = strchr(u + 1, ')')) != NULL) {
            u += 1;
            *v++ = 0;
            strcpy(anum, u);
        } else {
            v = s[0];
            strcpy(anum, "(Unnamed)");
        }
        
        /*  Scan name or other designation */
        
        while (isspace(*v)) {
            v++;
        }
        while (strlen(v) > 0 && isspace(v[strlen(v) - 1])) {
            v[strlen(v) - 1] = 0;   
        }
        strcpy(aname, v);
        
        /* Epoch isn't always precisely aligned in MPC postings.
           Allow for some slack. */
           
        v = strstr(s[1], "JDT");
        if (v == NULL) {
            strcpy(epoch, "0");         /* Zero should get somebody's attention */
        } else {
            sprintf(epoch, "%g", atof(v + 3) - 2400000.5);
        }
        
        sc(meanan, 2, 2, 15);
        sc(semimaj, 4, 2, 15);
        sc(eccen, 5, 2, 15);
        sc(argperi, 3, 26, 35);
        sc(longnode, 4, 26, 35);
        sc(inclin, 5, 26, 35);
        sc(magg, 6, 39, 48);
        sc(magh, 6, 20, 30);
        
        /* Mean anomaly is blank in some MPECs, meaning zero.
           Home Planet correctly interprets the blank field as zero,
           but it might confuse other programs.  Replace it with 0.0 if
           blank. */
           
        if (strlen(meanan) == 0) {
            strcpy(meanan, "0.0");
        }
        
        /* Output information in CSV format */

#ifdef EDITMODE     
        fprintf(fo, "%s,%s,%s %s,%s %s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                    anum, aname, anum, aname, aname, anum, magh, magg, meanan, argperi,
                    longnode, inclin, eccen, semimaj, epoch);
        if (fn != NULL) {
            fprintf(fn, "%s,%s %s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                        anum, anum, aname, magh, magg, meanan, argperi,
                        longnode, inclin, eccen, semimaj, epoch);
        }
#else
        fprintf(fo, "%s %s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                    aname, anum, magh, magg, meanan, argperi,
                    longnode, inclin, eccen, semimaj, epoch);
#endif
        written++;                  
    }
    
    fclose(fi);
    fclose(fo);
#ifdef EDITMODE 
    if (fn != NULL) {
        fclose(fn);
    }
#endif
    fprintf(stderr, "%d record%s written.\n", written, written == 1 ? "" : "s");        
    
    return 0;
}
