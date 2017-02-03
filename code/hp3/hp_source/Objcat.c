/*

			Sunclock for Windows

			  Object Catalogue

*/

#include "sunclock.h"

#define MaxRecord	350			// Maximum record length

int currentObjCat;				// Last selected object catalogue

static char catfile[80];		// Current catalogue file name
static char dbfields[MaxRecord];// Field names from current catalogue
static int ndbfields;			// Number of field names
static char imagefile[40];		// Image file name
static char soundfile[40];		// Sound file name
static int rafield, decfield;	// RA and Dec field indices for DBC_RADEC databases
static int imagefield;			// Image field index
static int soundfield;			// Sound field index
static int typefield, labelfield; // Type and label field indices for user catalogues
static int force = -1;			// For force-feeding selections to object catalogue
static int currentUserCat = -1;	// Current user catalogue index or -1 if none

#define DBC_NONE		0		// No special category
#define DBC_ASTEROIDS	1       // Database of asteroid elements
#define DBC_COMETS		2		// Database of cometary elements
#define DBC_RADEC		3		// Database containing selectable RA and Dec fields, find-eligible
#define DBC_PLANETS		4		// Planets from Solar System Objects database
#define DBC_RADEC_NF	5		// Database containing selectable RA and Dec, ineligible for find
#define DBC_RADEC_SHIFT	6		// Database containing selectable RA and Dec fields, SHIFT find-eligible
#define DBC_RADEC_CTRL	7		// Database containing selectable RA and Dec fields, CTRL find-eligible
#define DBC_RADEC_CSHFT	8		// Database containing selectable RA and Dec fields, CTRL/SHIFT find-eligible
#define DBC_RADEC_CHART	9		// Database containing selectable RA and Dec fields, find only if charted

static int databaseClass = DBC_NONE; // Database class for "select" button

#define isDBC_RADEC_findable(x) (((x) == DBC_RADEC) ||			\
							     ((x) == DBC_RADEC_SHIFT) ||	\
							     ((x) == DBC_RADEC_CTRL) ||		\
							     ((x) == DBC_RADEC_CSHFT) ||	\
							     ((x) == DBC_RADEC_CHART))
							     
#define caseDBC_RADEC_findable DBC_RADEC: case DBC_RADEC_SHIFT:	\
					      case DBC_RADEC_CTRL: case DBC_RADEC_CSHFT: case DBC_RADEC_CHART 							     

static double cAsteroid[9];		// Elements of current asteroid
static char nAsteroid[40];		// Name of current asteroid
static char cObjRA[15], cObjDec[15]; // Current object right ascension and declination
static int cPlanet;				// Currently selected planet index
static int acCat, acObj;		// Current asteroid or comet catalogue and object index

/*  CJDATE  --  Scan a date specified either in civil ([yy]yy-mm-dd.ddd) or Julian
				dddddd.ddddd format.  */

static double cjdate(char *field)
{
	long yy;
	int mm;
	double dd;

	if (sscanf(field, "%ld-%d-%lf", &yy, &mm, &dd) >= 3) {
		int idd = (int) dd;

		if (yy < 100) {
			yy += (yy > 80) ? 1900 : 2000;
		}
		return ucttoj(yy, mm - 1, idd, 0, 0, 0) + (dd - idd);
	} else {
		return atof(field);
	}
}

/*  SCANDBFIELD  --  Scan database fields for known-format databases.  */

static void scanDbField(int class, int fieldno, char *field)
{
	switch (class) {
		case DBC_ASTEROIDS:
			if (fieldno == 0) {
				strncpy(nAsteroid, field, 39);
				nAsteroid[39] = EOS;
			} else if (fieldno == 1) {
				cAsteroid[0] = cjdate(field);
			} else if (fieldno <= 9) {
				cAsteroid[fieldno - 1] = atof(field);
			}
			break;

		case DBC_COMETS:
			if (fieldno == 0) {
				strncpy(nAsteroid, field, 39);
				nAsteroid[39] = EOS;
			} else if (fieldno == 1) {
				cAsteroid[0] = cjdate(field);
			} else if (fieldno <= 9) {
				cAsteroid[fieldno - 1] = atof(field);
			}
			break;

		case caseDBC_RADEC_findable:
		case DBC_RADEC_NF:
			if (fieldno == rafield) {
				strncpy(cObjRA, field, sizeof(cObjRA) - 1);
			} else if (fieldno == decfield) {
				strncpy(cObjDec, field, sizeof(cObjDec) - 1);
			}
			break;
	}
}

/*  OBJECTCATDLG  --  Object catalogue dialogue procedure.  */

DialogueProc(ObjectCatDlg)
{
	FILE *fp;
	RECT dw, cw;
	HWND cWnd;
	static RECT button1;
	static int buttonY, buttonDeltaX, buttonBarycentre, lastx, lasty, imagex, imagey,
			   minx, miny, dbIndex, dbObject;
	static BOOL chasetail = TRUE;
	int tabs = 64;						// Tab stop array
	char irec[MaxRecord], catname[80], orec[786], nfld[80];

	switch (message) {
		case WM_INITDIALOG:
			catfile[0] = EOS;
			imagefile[0] = EOS;
			soundfile[0] = EOS;
			fp = fopen(rstring(IDS_OBJCAT_FILENAME), "r");
			GetClientRect(hDlg, &dw);
			lastx = dw.right;
			lasty = dw.bottom;
			GetWindowRect(hDlg, &cw);
			minx = cw.right - cw.left;
			miny = cw.bottom - cw.top;
			GetWindowRect(GetDlgItem(hDlg, IDOK), &button1);
			ScreenToClient(hDlg, (POINT FAR *) &(button1.left));
			buttonY = dw.bottom - button1.top;
			GetWindowRect(GetDlgItem(hDlg, IDC_OC_HELP), &cw);
			ScreenToClient(hDlg, (POINT FAR *) &(cw.left));
			ScreenToClient(hDlg, (POINT FAR *) &(cw.right));
			buttonDeltaX = cw.left - button1.left;
			buttonBarycentre = ((dw.right - button1.left) / 2) - button1.left;
			GetWindowRect(GetDlgItem(hDlg, IDC_OC_IMAGE), &cw);
			ScreenToClient(hDlg, (POINT FAR *) &(cw.left));
			ScreenToClient(hDlg, (POINT FAR *) &(cw.right));
			imagex = cw.right - cw.left;
			imagey = cw.bottom - cw.top;
			SendDlgItemMessage(hDlg, IDC_OC_DBINFO, EM_SETTABSTOPS, 1,
				(LPARAM) (const int FAR *) &tabs);
			if (fp != NULL) {
            	SendDlgItemMessage(hDlg, IDC_OC_CATEGORY, WM_SETREDRAW, FALSE, 0L);
				while (fgets(irec, sizeof irec, fp)) {
					irec[strlen(irec) - 1] = EOS;
					if (irec[0] != ';') {
						CSVscanInit(irec);

						CSVscanField(catname);
               			SendDlgItemMessage(hDlg, IDC_OC_CATEGORY, CB_ADDSTRING,
                    							 0, (LPARAM) (LPSTR) (catname));
					}
				}
            	fclose(fp);
				SendDlgItemMessage(hDlg, IDC_OC_CATEGORY, CB_SETCURSEL, currentObjCat, 0L);
            	SendDlgItemMessage(hDlg, IDC_OC_CATEGORY, WM_SETREDRAW, TRUE, 0L);
            	/* Dirty trick: this routine uses a lot of stack space (n.b. it could
            	   just as well use temporary gloabl heap, but I wanted to avoid all
            	   the futzing with handles, locking and unlocking, etc.) and if we
            	   use SendMessage to update the category it generates a recursive call
            	   which compounds the problem.  So use the message queue to serialise
            	   the update process. */
            	if (force == -1) {
//            		PostMessage(hDlg, WM_COMMAND, IDC_OC_CATEGORY, MAKELONG(0, CBN_CLOSEUP));
					PostMessage(hDlg, WM_COMMAND, MAKELONG(IDC_OC_CATEGORY, CBN_CLOSEUP), 0);
            	}
			}
   			return TRUE;

		case WM_CLOSE:
			DestroyWindow(hDlg);
			return TRUE;

		case WM_DESTROY:
			objectCatPanel = NULL;
			return TRUE;

        case WM_PALETTECHANGED:
        	if (chasetail) {
        		chasetail = FALSE;
	        	// Force repaint if somebody changes the system palette
	        	if (hDlg != ((HWND) wParam)) {
	        		UpdateWindow(hDlg);
	        		return TRUE;	        		
	        	}
	        	chasetail = TRUE;
	        }
			return FALSE;			

		case WM_GETMINMAXINFO:
			{	MINMAXINFO FAR *mmi = (MINMAXINFO FAR *) lParam;

				mmi->ptMinTrackSize.x = minx;
				mmi->ptMinTrackSize.y = miny;
			}
			break;

		case WM_SIZE:
			{
				int cwidth, cheight;

				cwidth = LOWORD(lParam);
				cheight = HIWORD(lParam);
#define floatChild(x)	cWnd = GetDlgItem(hDlg, x);								\
						GetWindowRect(cWnd, &cw);								\
						ScreenToClient(hDlg, (POINT FAR *) &(cw.left));			\
						cw.top += (cheight - lasty);							\
						SetWindowPos(cWnd, NULL, cw.left, cw.top, 0, 0,			\
							SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER)

				floatChild(IDOK);
				floatChild(IDC_OC_HELP);
				floatChild(IDC_OC_AIM);
				cWnd = GetDlgItem(hDlg, IDC_OC_IMAGE);
				SetWindowPos(cWnd, NULL, 0, 0, imagex + (cwidth - lastx),
							imagey + (cheight - lasty),
							SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
				imagex += (cwidth - lastx);
				imagey += (cheight - lasty);
				lastx = cwidth;
				lasty = cheight;
			}
			return TRUE;
 
		case WM_EXITSIZEMOVE:
			InvalidateRect(hDlg, NULL, FALSE);
			UpdateWindow(hDlg);
			break;

		case WM_PAINT:
			{	PAINTSTRUCT ps;
			    HWND hWnd = GetDlgItem(hDlg, IDC_OC_IMAGE);
			    HDC hDC;
    			HBITMAP ibmap = NULL;
    			int bx, by;
			    RECT cr;
			    HPALETTE bpal = NULL, opal;

                (void) BeginPaint(hDlg, &ps);
                hDC = GetDC(hWnd);
            	GetClientRect(hWnd, &cr);
	            FillRect(hDC, &cr, GetStockObject(BLACK_BRUSH));

	            if (strlen(imagefile) > 0) {
	            	HFILE fp;
	            	OFSTRUCT of;

	            	strcpy(nfld, rstring(IDS_IMAGE_DIRECTORY));
	            	strcat(nfld, imagefile);
	            	strcat(nfld, ".bmp");
	            	fp = OpenFile(nfld, &of, OF_READ);
	            	if (fp == HFILE_ERROR) {
	            		FILE *ap;

	            		/* Image not found.  See if there's an alias for the name that
	            		   does exist. */

	            		ap = fopen(rstring(IDS_ALIAS_FILENAME), "r");
						if (ap != NULL) {
							while (fgets(irec, sizeof irec, ap)) {
								irec[strlen(irec) - 1] = EOS;
								if (irec[0] != ';') {
									CSVscanInit(irec);

									CSVscanField(nfld);
									if (stricmp(imagefile, nfld) == 0) {
										CSVscanField(catname);
	            						strcpy(nfld, rstring(IDS_IMAGE_DIRECTORY));
	            						strcat(nfld, catname);
	            						strcat(nfld, ".bmp");
	            						fp = OpenFile(nfld, &of, OF_READ);
	            						break;
									}
								}
							}
							fclose(ap);
						}

	            	}

	            	// Nothing found.  Try to display the default image

	            	if (fp == HFILE_ERROR) {
		            	strcpy(nfld, rstring(IDS_IMAGE_DIRECTORY));
		            	strcat(nfld, rstring(IDS_IMAGE_DEFAULT));
		            	fp = OpenFile(nfld, &of, OF_READ);
	            	}

	            	if (fp != HFILE_ERROR) {
	            		BITMAPFILEHEADER bfh;
	            		HGLOBAL hbmi = NULL, hbits = NULL;
	            		BITMAPINFOHEADER FAR *bmi;
	            		char *bits;
	            		char *rits;

	            		hbmi = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
	            		if (hbmi != NULL) {
	            			DWORD bsize;

	            			bmi = (BITMAPINFOHEADER FAR *) GlobalLock(hbmi);
	            			_lread(fp, &bfh, sizeof bfh);
	            			_lread(fp, bmi, sizeof(BITMAPINFOHEADER));
	            			bx = (int) bmi->biWidth;
	            			by = (int) bmi->biHeight;
	            			if (bmi->biClrUsed == 0) {
	            				bmi->biClrUsed = 1L << bmi->biBitCount;
	            			}
	            			_lread(fp, ((char FAR *) bmi) + sizeof(BITMAPINFOHEADER),
	            							(int) (bmi->biClrUsed * sizeof(RGBQUAD)));
	            			_llseek(fp, bfh.bfOffBits, 0);
	            			bsize = (bmi->biSizeImage == 0) ?
	            						(bmi->biHeight *
	            							((((bmi->biWidth * bmi->biBitCount) / 8) + 3) & 0xFFFFFFFC)) :
	            						bmi->biSizeImage;
	            			bsize = max(bsize, (sizeof(LOGPALETTE) + (256 * sizeof(PALETTEENTRY))));

	            			if ((hbits = GlobalAlloc(GHND, bsize)) != NULL) {
	            				int i;
	            				LPLOGPALETTE lp;
	            				BITMAPINFO FAR *bh = (BITMAPINFO FAR *) bmi;
	            				unsigned short FAR *palidx;

	            				rits = bits = (char FAR *) GlobalLock(hbits);

	            				/* Dirty trick--borrow the bitmap buffer to assemble the palette. */
	            				lp = (LOGPALETTE FAR *) bits;
	            				palidx = (unsigned short FAR *) bh->bmiColors;
	            				lp->palVersion = 0x0300;
	            				lp->palNumEntries = (WORD) bmi->biClrUsed;
	            				for (i = 0; i < ((int) bmi->biClrUsed); i++) {
	            					lp->palPalEntry[i].peRed = bh->bmiColors[i].rgbRed;
	            					lp->palPalEntry[i].peGreen = bh->bmiColors[i].rgbGreen;
	            					lp->palPalEntry[i].peBlue = bh->bmiColors[i].rgbBlue;
	            					lp->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
	            					palidx[i] = (unsigned short) i;
	            				}
	            				bpal = CreatePalette(lp);
	            				if (bpal != NULL) {
    								opal = SelectPalette(hDC, bpal, FALSE);
    								RealizePalette(hDC);
	            				}

	            				while (bsize > 0xFF00) {
	            					_lread(fp, rits, 0xFF00);
	            					rits += 0xFF00;
	            					bsize -= 0xFF00;
	            				}
	            				_lread(fp, rits, (UINT) bsize);
	            				ibmap = CreateDIBitmap(hDC, bmi, CBM_INIT,
	            							bits, (BITMAPINFO FAR *) bmi, DIB_PAL_COLORS);
	            				GlobalUnlock(hbits);
	            				GlobalFree(hbits);
	            			}
	            			GlobalUnlock(hbmi);
	            			GlobalFree(hbmi);
	            		}
	            		_lclose(fp);
	            	}
	            }

	            if (ibmap != NULL) {
	            	HDC hMemDC;
	            	HBITMAP obmap;
	            	HPALETTE mopal;

					hMemDC = CreateCompatibleDC(hDC);
					if (bpal != NULL) {
						mopal = SelectPalette(hMemDC, bpal, FALSE);
					}
					obmap = SelectObject(hMemDC, ibmap);
					if (bx <= cr.right && by <= cr.bottom) {
	            		BitBlt(hDC, (cr.right - bx) / 2, (cr.bottom - by) / 2,
	            			bx, by, hMemDC, 0, 0, SRCCOPY);
					} else {
						int nx, ny;
						double xshrink = ((double) bx) / cr.right,
							   yshrink = ((double) by) / cr.bottom;

						if (xshrink > yshrink) {
							nx = cr.right;
							ny = (int) (by / xshrink);
						} else {
							ny = cr.bottom;
							nx = (int) (bx / yshrink);
						}
						SetStretchBltMode(hDC, COLORONCOLOR);
						StretchBlt(hDC, (cr.right - nx) / 2, (cr.bottom - ny) / 2, nx, ny,
							hMemDC, 0, 0, bx, by, SRCCOPY);
					}
	            	SelectObject(hMemDC, obmap);
	            	DeleteObject(ibmap);
	            	if (bpal != NULL) {
	            		SelectPalette(hDC, opal, FALSE);
	            		SelectPalette(hMemDC, mopal, FALSE);
	            		DeleteObject(bpal);
	            	}
	            	DeleteDC(hMemDC);
	            }

	            ReleaseDC(hWnd, hDC);
				EndPaint(hDlg, &ps);
			}
			return TRUE;

        case WM_COMMAND:
            switch (WM_COMMAND_ID(wParam)) {

                case IDOK:
                    SendMessage(hDlg, WM_CLOSE, 0, 0L);
                    return TRUE;

                case IDC_OC_AIM:
                	switch (databaseClass) {
                		case DBC_ASTEROIDS:
                			acCat = dbIndex;
                			acObj = dbObject;
                			selectAsteroid(nAsteroid, cAsteroid);
                			break;

                		case DBC_COMETS:
                			acCat = dbIndex;
                			acObj = dbObject;
                			selectComet(nAsteroid, cAsteroid);
                			break;

                		case caseDBC_RADEC_findable:
                		case DBC_RADEC_NF:
               				selectSkyObject(cObjRA, cObjDec);
                			break;

                		case DBC_PLANETS:
                			if (cPlanet != 3) {
                				int n = (cPlanet > 3) ? (cPlanet - 1) : cPlanet;

                				planets(faketime, 0xFFFF);
                				selectSkyObjectBin(planet_info[n].ra, planet_info[n].dec);
                			}
                			break;
                	}
                	return TRUE;

                case IDC_OC_HELP:
                	WinHelp(hWndMain, rstring(IDS_HELPFILE), HELP_KEY,
                				((DWORD) ((LPSTR) rstring(IDS_HELP_ON_OBJCAT))));
                	holped = TRUE;
                	return TRUE;
                	
                case IDC_OC_PLOT:
                	if (IsDlgButtonChecked(hDlg, IDC_OC_PLOT)) {
                		currentUserCat = dbIndex + 1;
                		strcpy(userCat, catfile);
                		if (skyViewPanel == NULL) {
                			launchSkyWindow();
                		} else {
                			updateSky(faketime, TRUE);
                		}
                	} else {
                		strcpy(userCat, rstring(IDS_NONE));
                		updateSky(faketime, TRUE);
                		currentUserCat = -1;
                	}
                	return TRUE;

            	case IDC_OC_CATEGORY:
            		if (HIWORD(wParam) == CBN_CLOSEUP) {
            			int i, j, gotfields = FALSE;
            			FILE *db;

                        if (force != -1) {
            				j = force;
							SendDlgItemMessage(hDlg, IDC_OC_CATEGORY, CB_SETCURSEL, j, 0L);
							force = -1;
						}
            			j = (int) SendDlgItemMessage(hDlg, IDC_OC_CATEGORY, CB_GETCURSEL, 0, 0L);
            			currentObjCat = dbIndex = j;
            			SendDlgItemMessage(hDlg, IDC_OC_OBJNAME, CB_RESETCONTENT, 0, 0L);
            			SendDlgItemMessage(hDlg, IDC_OC_OBJNAME, CB_SETCURSEL, 0, 0L);
						// Clear record panel just in case
		            	SendDlgItemMessage(hDlg, IDC_OC_DBINFO, WM_SETTEXT, 0,
		            		(LPARAM) (LPCSTR) "");
            			fp = fopen(rstring(IDS_OBJCAT_FILENAME), "r");
            			if (fp != NULL) {
            				i = 0;
            				while (TRUE) {
            					if (fgets(irec, sizeof irec, fp) == NULL) {
            						break;
            					}
            					if (irec[0] == ';') {
            						continue;
            					}
            					if (i == j) {
									irec[strlen(irec) - 1] = EOS;
									if (irec[0] != ';') {
										CSVscanInit(irec);

										CSVscanField(catfile);
										CSVscanField(catfile);
                                        
                                        CheckDlgButton(hDlg, IDC_OC_PLOT,
                                        	stricmp(catfile, userCat) == 0);
										db = fopen(catfile, "r");
										if (db != NULL) {
											char catcat[12];

											databaseClass = DBC_NONE;
											CSVscanField(catcat);
											if (strlen(catcat) > 0) {
												databaseClass = atoi(catcat);
											}
		            						EnableWindow(GetDlgItem(hDlg, IDC_OC_AIM), databaseClass != DBC_NONE);
							            	SendDlgItemMessage(hDlg, IDC_OC_OBJNAME, WM_SETREDRAW, FALSE, 0L);
											while (fgets(irec, sizeof irec, db)) {
												irec[strlen(irec) - 1] = EOS;
												if (irec[0] != ';') {
													CSVscanInit(irec);

                                                    if (gotfields) {
														CSVscanField(catname);
														if (strlen(catname) > 0) {
															if (SendDlgItemMessage(hDlg, IDC_OC_OBJNAME, CB_ADDSTRING,
								                    							 0, (LPARAM) (LPSTR) (catname)) ==
								                    			CB_ERRSPACE) {
								                    			MessageBeep(MB_ICONEXCLAMATION);
								                    			MessageBox(hDlg, rstring(IDS_ERR_MEMOBJCAT),
								                    				rstring(IDS_APPNAME), MB_ICONEXCLAMATION | MB_OK);
								                    			break;
								                    		} 
							                    		}
							                    	} else {
							                    		char *cp = dbfields;

							                    		// First record: extract and save field titles

							                    		gotfields = TRUE;
							                    		dbfields[0] = EOS;
							                    		ndbfields = 0;
							                    		rafield = decfield = imagefield =
							                    				  typefield = labelfield = soundfield = -1;
							                    		while (CSVscanField(catname)) {
							                    			if ((isDBC_RADEC_findable(databaseClass) ||
							                    				(databaseClass == DBC_RADEC_NF)) &&
							                    				(strcmp(catname, rstring(IDS_OC_RA)) == 0)) {
							                    					rafield = ndbfields;
							                    			}
							                    			if ((isDBC_RADEC_findable(databaseClass) ||
							                    				(databaseClass == DBC_RADEC_NF)) &&
							                    				(strcmp(catname, rstring(IDS_OC_DEC)) == 0)) {
							                    					decfield = ndbfields;
							                    			}
							                    			if (strcmp(catname, rstring(IDS_OC_IMAGE)) == 0) {
							                    				imagefield = ndbfields;
							                    			} else if (strcmp(catname, rstring(IDS_OC_SOUND)) == 0) {
							                    				soundfield = ndbfields;
									                		} else if (strcmp(catname, rstring(IDS_OC_TYPE)) == 0) {
									                			typefield = ndbfields; 
									                		} else if (strcmp(catname, rstring(IDS_OC_LABEL)) == 0) {
									                			labelfield = ndbfields;							                    			
									                		}

							                    			strcpy(cp, catname);
							                    			cp += strlen(cp) + 1;
							                    			ndbfields++;
							                    		}
							                    		*cp++ = EOS;
							                    	}
												}
											}
							            	SendDlgItemMessage(hDlg, IDC_OC_OBJNAME, WM_SETREDRAW, TRUE, 0L);
											SendDlgItemMessage(hDlg, IDC_OC_OBJNAME, CB_SETCURSEL, 0, 0L);
							            	fclose(db);
                                            EnableWindow(GetDlgItem(hDlg, IDC_OC_PLOT),
                                            	!(rafield < 0 || decfield < 0 || (typefield < 0 && labelfield < 0)));	
            								PostMessage(hDlg, WM_COMMAND, MAKELONG(IDC_OC_OBJNAME, CBN_CLOSEUP), 0);
										}
            							break;
            						}
            					}
            					i++;
            				}
            				fclose(fp);
            			}
           			}
            		break;

            	case IDC_OC_OBJNAME:
            		if (HIWORD(wParam) == CBN_CLOSEUP) {
            			int i, j, k, l;
            			FILE *db;

						imagefile[0] = EOS;
						soundfile[0] = EOS;
						db = fopen(catfile, "r");
						if (db != NULL) {
							if (force != -1) {
								j = force;
								SendDlgItemMessage(hDlg, IDC_OC_OBJNAME, CB_SETCURSEL, j, 0L);
								force = -1;
							}
							j = (int) SendDlgItemMessage(hDlg, IDC_OC_OBJNAME, CB_GETCURSEL, 0, 0L);
							dbObject = j;
							for (i = 0; i <= (j + 1); ) {
								if (fgets(irec, sizeof irec, db) == NULL) {
									j = -1;
									break;
								}
								if (irec[0] != ';') {
									i++;
								}
				            	CSVscanInit(irec);
					            CSVscanField(nfld);
					            if (strlen(nfld) == 0) {
					            	dbObject = j = (j + 1);
					            }
							}
			            	fclose(db);
			            	if (j >= 0) {
			            		if (databaseClass == DBC_PLANETS) {
			            			cPlanet = j;
			            		}
			            		irec[strlen(irec) - 1] = EOS;
				            	CSVscanInit(irec);
				            	orec[0] = 0;
				            	for (l = k = 0; k < ndbfields; k++) {
					            	CSVscanField(nfld);
				            		if (databaseClass != DBC_NONE) {
				            			scanDbField(databaseClass, k, nfld);
				            		}
//				            		if (k != imagefield && k != soundfield) {
									if (dbfields[l] != '(') {
					            		strcat(orec, dbfields + l);
					            		strcat(orec, ":\t");
					            		strcat(orec, nfld);
					            		strcat(orec, "\r\n");
				            		}
				            		l += strlen(dbfields + l) + 1;
				            		if (k == 0 || k == imagefield) {
				            			if (strchr(nfld, ' ') != NULL) {
											*strchr(nfld, ' ') = EOS;
										}
										strcpy(imagefile, nfld);
				            		}
				            		if (k == soundfield) {
				            			if (strchr(nfld, ' ') != NULL) {
											*strchr(nfld, ' ') = EOS;
										}
										strcpy(soundfile, nfld);
				            		}
				            	}
				            	if (waveAudioAvailable && (strlen(soundfile) > 0)) {
					            	strcpy(nfld, rstring(IDS_SOUND_DIRECTORY));
					            	strcat(nfld, soundfile);
					            	strcat(nfld, ".wav");
					            	sndPlaySound(nfld, SND_ASYNC | SND_NODEFAULT);

				            	}
				            	SendDlgItemMessage(hDlg, IDC_OC_DBINFO, WM_SETTEXT, 0,
				            		(LPARAM) (LPCSTR) orec);
				            	InvalidateRgn(hDlg, NULL, FALSE);
			            	}
			            }
           			}
            		break;


                default:
                	break;
            }
            break;

        default:
        	break;

	}
	return FALSE;
}

/*  LAUNCHOBJECTCAT  --  Launch object catalogue if not already running.  */

void launchObjectCat(void)
{
 	if (objectCatPanel == NULL) {
 		if (objectCatProc == NULL) {
 			objectCatProc = (DLGPROC) MakeProcInstance((FARPROC) ObjectCatDlg, hInst);
 		}
    	objectCatPanel = CreateDialog(hInst, MAKEINTRESOURCE(IDC_OBJCAT), hWndMain,
    		objectCatProc);
 	}
}

struct findItem {
	unsigned char ocat;					// Object catalogue number
	unsigned char octype;				// Object catalaogue type
	int objnum;							// Object record number within catalogue
	float ra, dec;						// Right ascension and declination
};

#define findItems	1000				// Number of findItem structures per findTable

struct findTable {
	HGLOBAL ftnext;						// Handle of next findTable
	int ftnum;							// Number of occupied items in this table
	struct findItem fttab[findItems];	// Table of item structures
};

static HGLOBAL findlist = NULL;			// Object finder list
static HGLOBAL ltab = NULL;				// Current finder table handle
static struct findTable FAR *cftab = NULL; // Current finder table pointer
static int planetCat = -1;				// Index of solar system object catalogue

/*  RELFINDINDEX  --  Discard any existing object finder list.  */

void relFindIndex(void)
{
	while (findlist != NULL) {
		struct findTable FAR *ft = (struct findTable FAR *) GlobalLock(findlist);
		HGLOBAL fln = ft->ftnext;
		GlobalUnlock(findlist);
		GlobalFree(findlist);
		findlist = fln;
	}
	cftab = NULL;
	ltab = NULL;
}

/*  GETFINDITEM  --  Obtain pointer to next object finder item, allocating
					 as necessary.  */

static struct findItem FAR *getFindItem(void)
{
	HGLOBAL ntab;
	struct findTable FAR *nftab;

	// If we're currently filling a table, see if there's room in it for another entry

	if (cftab != NULL) {
		if (cftab->ftnum < findItems) {
			return &(cftab->fttab[(cftab->ftnum)++]);
		}
	}

    // Allocate a new find list table

	ntab = GlobalAlloc(GMEM_MOVEABLE, sizeof(struct findTable));
	if (ntab == NULL) {
		return NULL;
	}
	nftab = (struct findTable FAR *) GlobalLock(ntab);
	if (nftab == NULL) {
		GlobalFree(ntab);
		return NULL;
	}
	nftab->ftnext = NULL;				// Clear forward pointer in new table
	nftab->ftnum = 0;                   // Mark no objects in new table
	if (cftab != NULL) {
		cftab->ftnext = ntab;			// Chain this table to last in chain
		GlobalUnlock(ltab);
	}
	cftab = nftab;
	ltab = ntab;						// Save this as last table
	if (findlist == NULL) {
		findlist = ntab;				// First table: place at head of chain
	}
	return &(cftab->fttab[(cftab->ftnum)++]); // Return first item in new table
}

/*  ENDGETFIND  --  Terminate allocation of find items.  */

static void endGetFind(void)
{
	if (cftab != NULL) {
		GlobalUnlock(findlist);
		cftab = NULL;
	}
}

/*  BUILDFINDINDEX  --  Construct the back-index from celestial co-ordinates to
						members of the object catalogue used to find objects in
						the sky and telescope windows.  */

void buildFindIndex(void)
{
	FILE *fp;

    if (findlist != NULL) {
    	return;							// Find list already built
    }
    planetCat = -1;
	fp = fopen(rstring(IDS_OBJCAT_FILENAME), "r");
	if (fp != NULL) {
		int i = 0;
		char irec[MaxRecord], catname[80];

		while (TRUE) {
			if (fgets(irec, sizeof irec, fp) == NULL) {
				break;
			}
			if (irec[0] == ';') {
				continue;
			}
			irec[strlen(irec) - 1] = EOS;
			if (irec[0] != ';') {
				FILE *db;
				int gotfields = FALSE;
				char catcat[12];

				i++;
				CSVscanInit(irec);
				CSVscanField(catfile);
				CSVscanField(catfile);
				databaseClass = DBC_NONE;
				CSVscanField(catcat);
				if (strlen(catcat) > 0) {
					databaseClass = atoi(catcat);
				}

                if (databaseClass == DBC_PLANETS) {
                	planetCat = i;
				} else if (isDBC_RADEC_findable(databaseClass)) {
					db = fopen(catfile, "r");
					if (db != NULL) {
						int j = 0;

						while (fgets(irec, sizeof irec, db)) {
							irec[strlen(irec) - 1] = EOS;
							if (irec[0] != ';') {
								CSVscanInit(irec);

	                            if (gotfields) {
	                            	int k;
	                            	char nfld[80];

	                                j++;
					            	for (k = 0; k < ndbfields; k++) {
						            	CSVscanField(nfld);
					            		scanDbField(databaseClass, k, nfld);
					            	}
					            	if (strlen(cObjRA) > 0 && strlen(cObjDec) > 0) {
					            		double dra, ddec;
					            		struct findItem FAR *fi = getFindItem();

					            		if (fi != NULL) {
					            			fi->ocat = (unsigned char) i;
					            			fi->octype = (unsigned char) databaseClass;
					            			fi->objnum = j;
					            			parseRaDec(cObjRA, cObjDec, &dra, &ddec);
					            			fi->ra = (float) dra;
					            			fi->dec = (float) ddec;
					            		}
					            	}
		                    	} else {
		                    		char *cp = dbfields;

		                    		// First record: extract and save field titles

		                    		gotfields = TRUE;
		                    		dbfields[0] = EOS;
		                    		ndbfields = 0;
		                    		rafield = decfield = imagefield = soundfield = -1;
		                    		while (CSVscanField(catname)) {
		                    			if ((strcmp(catname, rstring(IDS_OC_RA)) == 0)) {
		                    					rafield = ndbfields;
		                    			}
		                    			if ((strcmp(catname, rstring(IDS_OC_DEC)) == 0)) {
		                    					decfield = ndbfields;
		                    			}
		                    			strcpy(cp, catname);
		                    			cp += strlen(cp) + 1;
		                    			ndbfields++;
		                    		}
		                    		*cp++ = EOS;
		                    	}
							}
						}
		            	fclose(db);
					}
				}
			}
		}
		fclose(fp);
		endGetFind();
	}
}

/*  FINDOBJECT  --  Find closest object in catalogue to a given position.  */

int findObject(double ra, double dec, WPARAM keysdown,
			   int *catno, int *objno, double *separation)
{
	int i, found = FALSE, bcat = 0, bobj = 0, kmod;
	double bsep = 3 * PI, dra, ddec, sindec, cosdec;
    HCURSOR ocurs = SetCursor(LoadCursor(NULL, IDC_WAIT));

    ShowCursor(TRUE);
    kmod = (keysdown & MK_SHIFT) ? ((keysdown & MK_CONTROL) ? DBC_RADEC_CSHFT : DBC_RADEC_SHIFT) :
    			((keysdown & MK_CONTROL) ? DBC_RADEC_CTRL : DBC_RADEC);
    dra = dtr(ra);
    ddec = dtr(dec);
    sindec = sin(ddec);
    cosdec = cos(ddec);
	buildFindIndex();						// Construct object finder index, if needed
	if (findlist != NULL) {
		HGLOBAL fl = findlist, fln;

		assert(cftab == NULL);
		while (fl != NULL) {
			cftab = GlobalLock(fl);
			if (cftab == NULL) {
				break;
			}
			for (i = 0; i < cftab->ftnum; i++) {
				double rra = dtr(cftab->fttab[i].ra),
					   rdec = dtr(cftab->fttab[i].dec),
					   d;

				d = sindec * sin(rdec) + cosdec * cos(rdec) * cos(dra - rra);
				if (d < 0.999995) {
					d = acos(d);
				} else {
					double t1 = (dra - rra) * cos((rdec + ddec) / 2),
						   t2 = (ddec - rdec);
					d = sqrt(t1 * t1 + t2 * t2);
				}
				if (d < bsep) {
					int octyp = cftab->fttab[i].octype;
					
					/* If this database is marked eligible for searching only
					   if currently charted, set the type to findable or not
					   eligible for finding based upon whether it's the current
					   user catalogue. */
					
					if (octyp == DBC_RADEC_CHART) {
						octyp = (currentUserCat == cftab->fttab[i].ocat) ?
									DBC_RADEC : DBC_RADEC_NF; 
					}  
				
					/* If this is a catalogue to which keyboard modifiers apply,
					   make sure the modifiers match the keyboard state at the
					   time of the mouse click. */
					   
					if ((octyp != DBC_RADEC_NF) && (!isDBC_RADEC_findable(octyp) ||
													(kmod == octyp))) {
						found = TRUE;
						bsep = d;
						bcat = cftab->fttab[i].ocat;
						bobj = cftab->fttab[i].objnum;
					}
				}
			}
			fln = cftab->ftnext;
			GlobalUnlock(fl);
			cftab = NULL;
			fl = fln;
		}
	}

	/*	Check for hits on solar system objects.  Note that we do this only
		if no keyboard modifiers are in effect.  We don't want to override
		a user's explicit request for a given catalogue.  */
    
    if (kmod == DBC_RADEC) {
	    planets(faketime, 0xFFFF);
	    for (i = 0; i <= (aTracked ? 10 : 9); i++) {
	        if ((i < 9) || (planet_info[i].hrv > 0)) {
				double rra = dtr(planet_info[i].ra),
					   rdec = dtr(planet_info[i].dec),
					   d;
	
				d = sindec * sin(rdec) + cosdec * cos(rdec) * cos(dra - rra);
				if (d < 0.999995) {
					d = acos(d);
				} else {
					double t1 = (dra - rra) * cos((rdec + ddec) / 2),
						   t2 = (ddec - rdec);
					d = sqrt(t1 * t1 + t2 * t2);
				}
				if (d < bsep) {
					found = TRUE;
					bsep = d;
					if (i < 10 && planetCat >= 0) {
						bcat = planetCat;
						bobj = ((i < 3) ? i : i + 1) + 1;
					} else {
						bcat = acCat + 1;
						bobj = acObj + 1;
					}
				}
			}
	    }
    }


	ShowCursor(FALSE);
	SetCursor(ocurs);

	*catno = bcat - 1;
	*objno = bobj - 1;
	*separation = rtd(bsep);
	return found;
}

/*  POINTOBJECTCAT  --  Direct object catalogue to a specific catalogue and item.  */

void pointObjectCat(int catno, int objno)
{
	force = 1;						// Suppress auto-category selection if launched
	launchObjectCat();
	force = catno;
//	SendMessage(objectCatPanel, WM_COMMAND, IDC_OC_CATEGORY, MAKELONG(0, CBN_CLOSEUP));
	SendMessage(objectCatPanel, WM_COMMAND, MAKELONG(IDC_OC_CATEGORY, CBN_CLOSEUP), 0);
	force = objno;
//	SendMessage(objectCatPanel, WM_COMMAND, IDC_OC_OBJNAME, MAKELONG(0, CBN_CLOSEUP));
	SendMessage(objectCatPanel, WM_COMMAND, MAKELONG(IDC_OC_OBJNAME, CBN_CLOSEUP), 0);
}
