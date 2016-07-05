/***************************************************************/
/*         convert.h - H.261 Codec (TU-Dresden)                */
/*                                                             */
/*         Autor: Dagmar Geske                                 */
/*         Datum: 05.08.1997                                   */   
/*                                                             */
/***************************************************************/

// modified for use as lib: 16.12.97, Henrik Thuermer

#ifndef _CONVERT_H
#define _CONVERT_H

// I use a class because I don't know how to export simple
// functions to the library: If I use simple functions the
// linker does'nt find the functions in the library :-( .   
// (Henrik)

class ColorSpaceConversions {

public:

	ColorSpaceConversions();

	void RGB24_to_YV12(unsigned char * in, unsigned char * out,int w, int h);
	void YV12_to_RGB24(unsigned char *src0,unsigned char *src1,unsigned char *src2,unsigned char *dst_ori,int width,int height);
	void YVU9_to_YV12(unsigned char * in,unsigned char * out, int w, int h);
	void YUY2_to_YV12(unsigned char * in,unsigned char * out, int w, int h);
	void YV12_to_YVU9(unsigned char * in,unsigned char * out, int w, int h);
	void YV12_to_YUY2(unsigned char * in,unsigned char * out, int w, int h);
};

#endif /* _CONVERT_H */

