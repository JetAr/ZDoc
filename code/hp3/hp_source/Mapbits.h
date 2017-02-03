/*

	Interface definition to scenery generation library
	
*/

extern int FAR PASCAL mapbitsCount(void);

extern
void FAR PASCAL mapbitsGetBitMap(int bitmapNumber,				// Bitmap number to get
										 HANDLE FAR *hBitmap);	// Handle to bitmap or NULL = error

extern
void FAR PASCAL mapbitsGetDesc(int bitmapNumber,				// Bitmap description to get
									   LPSTR sbuf, UINT buflen);// Buffer and length
									   