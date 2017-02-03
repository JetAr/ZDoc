/*

	Interface definition to scenery generation library
	
*/

extern
void FAR PASCAL sceneryInit(double julianDate,			// Time and date
									double siteLat,		// Observer latitude
									double siteLon,		// Observer longitude
									double viewAzimuth,	// Azimuth of window center
									WORD imageHeight,	// Image height
									WORD imageWidth,	// Image width
									WORD randomNumber,	// A 15 bit random value
									WORD FAR *numIcons,	// Return: Number of icons
									WORD FAR *iconSize,	//         Icon size
									WORD FAR *iconDensity); //	   Icon density

extern void FAR PASCAL sceneryTerm(void);

extern
void FAR PASCAL sceneryIcon(int xPos, int yPos,				// Where icon is to be drawn
									int randomNumber,		// A 15 bit random number
									HICON FAR *hIcon,		// Return handle to icon
									WORD FAR *hWidth);		// Width (advance) after this icon

extern
void FAR PASCAL getSceneryIcon(int iconNumber,				// Icon number to get
									   HICON FAR *hIcon);	// Handle to icon or NULL = error
									   