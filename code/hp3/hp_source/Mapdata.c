/*

		Interpret the compressed vector map database

*/

#include "sunclock.h"

/*  Codes in the map data bytestream:

	      Byte 1	Arguments	   Meaning
	    ==========	=========	============
	    00 mv1 mv2		    - Two short draw commands
	    01 dxx dyy		    - One longer draw command
	    10 mv1 000		    - One short draw command
	    11 000 000		    - Long move command
			ax ax ay ay
	    11 000 001		    - Long draw command
			ax ax ay ay
	    11 000 010		    - Medium draw command
			dx dy
	    11 000 011		    - Medium move command
			dx dy
*/

/*  MAPDRAW  --  Decode map data, scale, and output vectors.  You call
		 this function with:

		    mapdraw(<X_pixels>, <Y_pixels>, <vectorproc>);

		 Where X_pixels and Y_pixels specify the size  of  the
		 desired image area, and vectorproc is a function that
		 receives the individual  vectors  of  the  map.   The
		 image	is  scaled to look best when X_pixels is twice
                 Y_pixels, but you're free to use any aspect ratio you
		 like.	 The  vectorproc  function  should be declared
		 like this sample:

		    void vectorproc(x1, y1, x2, y2)
		      int x1, y1, x2, y2;
		    {
                        printf("Draw from %d,%d to %d,%d.\n:,
			    x1, y1, x2, y2);
		    }

		 The  pixel at the upper left of the image is 0,0, and
		 addresses increase to	the  right  and  downward.   A
		 sample call on mapdraw would then be:

		    mapdraw(512, 256, vectorproc);
*/

void mapdraw(int xsize, int ysize,
             void (PASCAL *vector)(int x1, int y1, int x2, int y2))
{
    int x1 = -32700, y1 = -32700, x2, y2;
	unsigned char far *maptr;
	unsigned char far *mapend;
	unsigned int l;
	HANDLE hRes;

	hRes = LoadResource(hInst,
				FindResource(hInst, "MapData", "MAPVECTORS"));
	maptr = LockResource(hRes);
	l = (maptr[0] << 8) | maptr[1];
	maptr += 2;
	mapend = maptr + l;

    while (maptr < mapend) {
		static int xdec[] = {-1, 0, 1, -1, 1, -1, 0, 1};
		static int ydec[] = {-1, -1, -1, 0, 0, 1, 1, 1};
		int b = *maptr++;

#define unfit(x, n) ((x) - ((1 << (n)) / 2));
#define pxy(x,y) ((int)(((x)*(long)xsize)/2048L)),((int)(((y)*(long)ysize)/1024L))

		switch (b >> 6) {
		    int d;

		    case 0:
				d = (b >> 3) & 7;
				x2 = x1 + xdec[d];
				y2 = y1 + ydec[d];
				vector(pxy(x1, y1), pxy(x2, y2));
				x1 = x2;
				y1 = y2;
				d = b & 7;
				x2 = x1 + xdec[d];
				y2 = y1 + ydec[d];
				vector(pxy(x1, y1), pxy(x2, y2));
				break;

		    case 1:
				x2 = x1 + unfit((b >> 3) & 7, 3);
				y2 = y1 + unfit(b & 7, 3);
				vector(pxy(x1, y1), pxy(x2, y2));
				break;

		    case 2:
				d = (b >> 3) & 7;
				x2 = x1 + xdec[d];
				y2 = y1 + ydec[d];
				vector(pxy(x1, y1), pxy(x2, y2));
				break;

		    case 3:
				switch (b & 077) {
				    case 0:   /* Long move */
					x2 = *maptr++;
					x2 = (x2 << 8) | *maptr++;
					y2 = *maptr++;
					y2 = (y2 << 8) | *maptr++;
					break;

				    case 1:   /* Long draw */
					x2 = *maptr++;
					x2 = (x2 << 8) | *maptr++;
					y2 = *maptr++;
					y2 = (y2 << 8) | *maptr++;
					vector(pxy(x1, y1), pxy(x2, y2));
					break;

				    case 2:   /* Medium draw */
					x2 = *maptr++;
					x2 = x1 + unfit(x2, 8);
					y2 = *maptr++;
					y2 = y1 + unfit(y2, 8);
					vector(pxy(x1, y1), pxy(x2, y2));
					break;

				    case 3:   /* Medium move */
					x2 = *maptr++;
					x2 = x1 + unfit(x2, 8);
					y2 = *maptr++;
					y2 = y1 + unfit(y2, 8);
					break;
				}
				break;
		}
		x1 = x2;
		y1 = y2;
    }
    UnlockResource(hRes);
    FreeResource(hRes);
}
