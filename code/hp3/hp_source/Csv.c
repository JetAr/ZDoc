/*

		Comma separated value database format scanner

*/

#include "sunclock.h"

static char *csptr;				// CSV scan pointer

/*  CSVSCANINIT  --  Initialise scanning of a CSV record.  */

void CSVscanInit(char *s)
{
	csptr = s;
}

/*  CSVSCANFIELD  --  Scan next field from a CSV record.  */

int CSVscanField(char *f)
{
	int foundfield = FALSE;

	if (*csptr != EOS) {
		foundfield = TRUE;
		if (*csptr == '"') {
			csptr++;
			while (*csptr != EOS) {
				if (*csptr == '"') {
					if (csptr[1] == '"') {
						*f++ = '"';
						csptr += 2;
					} else {
						csptr++;
						break;
					}
				} else {
					*f++ = *csptr++;
				}
			}
		}
		while (*csptr != ',' && *csptr != EOS) {
			*f++ = *csptr++;
		}
		if (*csptr == ',') {
			csptr++;
		}
	}
	*f++ = EOS;
	return foundfield;
}
