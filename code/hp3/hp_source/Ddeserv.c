/*

		DDE Server

		This module uses the DDEML library to implement a DDE server

*/

#include "sunclock.h"
#include <ddeml.h>
#include "wrapper.h"

#define ddeUpdateInterval	5	 	// Hot link information update interval (seconds)

static double ijdate, igmst, sunra, sundec, sunrv, sunlong, subslong,
			  moonphase, cphase, aom, cdist, cdister, cangdia, csund, csuang, phasar[5],
			  nptime = 0, moonra, moondec, moonlong, moonclong, moonclat, mooncrv;
static int lunation;

int ddeActive = FALSE;				// Set true when first DDE request occurs

//	Forward functions

static void ddeAdviseSelective(int service, int topic);

// #define AniDDE    				// Define to have animation affect DDE calculation

/*  UPDATEDDEINFO  --  Recalculate information we make available via DDE.  */

void updateDDEInfo(void)
{
	struct tm gt;
	struct tm *ct;
	time_t cctime;
	static time_t lctime = 0;

    V time(&cctime);
    if (
#ifdef AniDDE
		TRUE
#else
        (cctime - lctime) > ddeUpdateInterval
#endif
       ) {
		set_tm_time(&gt, FALSE);
		ct = &gt;
    	lctime = cctime;
#ifdef AniDDE
		ijdate = faketime;
#else
		ijdate = jtime(ct);
#endif
    	sunpos(ijdate, TRUE, &sunra, &sundec, &sunrv, &sunlong);
    	igmst = gmst(ijdate);
    	subslong = (igmst * 15) - sunra;
   		if (subslong > 180) {
   			subslong = -(360 - subslong);
   		} else if (subslong < -180) {
   			subslong += 360;
   		}
    	moonphase = phase(ijdate, &cphase, &aom, &cdist, &cangdia, &csund, &csuang);
//    	cdister = cdist / EarthRad;
	    if (ijdate > nptime) {
	        phasehunt(ijdate + 0.5, phasar);
	        lunation = (int) floor(((phasar[0] + 7) - LunatBase) / SynMonth) + 1;
	        nptime = phasar[4];
	    }
		highmoon(ijdate, &moonclong, &moonclat, &mooncrv);
		cdister = mooncrv / EarthRad;
		ecliptoeq(ijdate, moonclong, moonclat, &moonra, &moondec);
   		moonlong = (igmst * 15) - moonra;
   		if (moonlong > 180) {
   			moonlong = -(360 - moonlong);
   		} else if (moonlong < -180) {
   			moonlong += 360;
   		}
		ddeAdvise();
	}
}

//	UPDATESCOPEDDE  --  Update DDE info when telescope status changes

void updateScopeDDE(void)
{
	ddeAdviseSelective(0, 3);				// Just notify scope watchers
}

/*  iDDE   --  Initialise DDE if required and perform first calculation.  */

static void iDDE(void)
{
	if (!ddeActive) {
		ddeActive = TRUE;
		updateDDEInfo();
	}
}

#define DDESvcF(x)	static HDDEDATA PASCAL Rq_DDE_##x(HDDEDATA hDataOut) { char sz[40]; \
						iDDE(); sprintf(sz, Format(12), (x)); \
						return DdeAddData(hDataOut, (LPBYTE)sz, (DWORD) (strlen(sz) + 1), 0L); }

#define DDESvcI(x)	static HDDEDATA PASCAL Rq_DDE_##x(HDDEDATA hDataOut) { char sz[40]; \
						iDDE(); sprintf(sz, Format(21), (x)); \
						return DdeAddData(hDataOut, (LPBYTE)sz, (DWORD) (strlen(sz) + 1), 0L); }

#define DDESvcFa(x, e)	static HDDEDATA PASCAL Rq_DDE_##x(HDDEDATA hDataOut) { char sz[40]; \
						iDDE(); sprintf(sz, Format(12), (e)); \
						return DdeAddData(hDataOut, (LPBYTE)sz, (DWORD) (strlen(sz) + 1), 0L); }

	DDESvcFa(ijdate, ijdate/*JD + 0.5*/)
	DDESvcF(igmst)
	DDESvcF(sunra)
	DDESvcF(sundec)
	DDESvcF(sunrv)
	DDESvcF(sunlong)
	DDESvcF(subslong)
	DDESvcF(moonphase)
	DDESvcF(cphase)
	DDESvcF(aom)
	DDESvcF(cdist)
	DDESvcF(cdister)
	DDESvcF(cangdia)
	DDESvcF(csund)
	DDESvcF(csuang)
	
	DDESvcF(moonra)
	DDESvcF(moondec)
	DDESvcF(moonlong)
	DDESvcF(moonclong)
	DDESvcF(moonclat)
	DDESvcI(lunation)
	DDESvcFa(phlnew, phasar[0]/*JD + 0.5*/)
	DDESvcFa(phq1, phasar[1]/*JD + 0.5*/)
	DDESvcFa(phhalf, phasar[2]/*JD + 0.5*/)
	DDESvcFa(phq2, phasar[3]/*JD + 0.5*/)
	DDESvcFa(phnnew, phasar[4]/*JD + 0.5*/)
	
	DDESvcI(telActive)
	DDESvcF(telra)
	DDESvcF(teldec)
	DDESvcF(telalt)
	DDESvcF(telazi)

static HDDEDATA PASCAL RequestHelp(HDDEDATA hDataOut)
{
    static char szHelp[] = "DDE Help for the Home Planet Service.\r\r\n\t"
        "Topics are Time, Sun, Moon, and Telescope\r\n\t"
        "Updates are every 60 seconds or when Telescope moves\r\n\t"
        "See Home Planet Help for details of items.";

    return(DdeAddData(hDataOut, (LPBYTE) szHelp, (DWORD) sizeof(szHelp), 0L));
}

static DDEFORMATTBL SystopicHelpFormats[] = {
    "TEXT", CF_TEXT, 0, NULL, RequestHelp
};

static DDEITEMTBL SystopicItems[] = {
    SZDDESYS_ITEM_HELP, 0, NULL, ELEMENTS(SystopicHelpFormats), 0, SystopicHelpFormats
};

#define FTAB(x) static DDEFORMATTBL x##_FMT_t[] = { { "TEXT", CF_TEXT, 0, NULL, Rq_DDE_##x} }

	FTAB(ijdate);
	FTAB(igmst);
	FTAB(sunra);
	FTAB(sundec);
	FTAB(sunrv);
	FTAB(sunlong);
	FTAB(subslong);
	FTAB(moonphase);
	FTAB(cphase);
	FTAB(aom);
	FTAB(cdist);
	FTAB(cdister);
	FTAB(cangdia);
	FTAB(csund);
	FTAB(csuang);
	
	FTAB(moonra);
	FTAB(moondec);
	FTAB(moonlong);
	FTAB(moonclong);
	FTAB(moonclat);
	FTAB(lunation);
	FTAB(phlnew);
	FTAB(phq1);
	FTAB(phhalf);
	FTAB(phq2);
	FTAB(phnnew);
	
	FTAB(telActive);
	FTAB(telra);
	FTAB(teldec);
	FTAB(telalt);
	FTAB(telazi);

#define F(n, x) { n, 0, NULL, ELEMENTS(x##_FMT_t), 0, x##_FMT_t }

static DDEITEMTBL TimeItems[] = {
	F("Jdate", ijdate),
	F("Gmst", igmst)
};

static DDEITEMTBL SunItems[] = {
	F("RA", sunra),
	F("Dec", sundec),
	F("DistanceAU", sunrv),
	F("DistanceKM", csund),
	F("Subtends", csuang),
	F("Elong", sunlong),
	F("Long", subslong),
};

static DDEITEMTBL MoonItems[] = {
	F("RA", moonra),
	F("Dec", moondec),
	F("Phase", cphase),
	F("Age", aom),
	F("DistanceER", cdister),
	F("DistanceKM", cdist),
	F("Subtends", cangdia),
	F("Long", moonlong),
	F("ELat", moonclat),
	F("ELong", moonclong),
	F("Lunation", lunation),
	F("PhLastNew", phlnew),
	F("PhFQuarter", phq1),
	F("PhHalf", phhalf),
	F("PhLQuarter", phq2),
	F("PhNextNew", phnnew)
};

static DDEITEMTBL TelescopeItems[] = {
	F("Active", telActive),
	F("RA", telra),
	F("Dec", teldec),
	F("Azi", telazi),
	F("Alt", telalt)
};

static DDETOPICTBL Topics[] = {
    { "Time",         0, ELEMENTS(TimeItems),     	0, TimeItems },
    { "Sun",		  0, ELEMENTS(SunItems),      	0, SunItems },
    { "Moon", 		  0, ELEMENTS(MoonItems),     	0, MoonItems },
    { "Telescope", 	  0, ELEMENTS(TelescopeItems),	0, TelescopeItems },
    { SZDDESYS_TOPIC, 0, ELEMENTS(SystopicItems), 	0, SystopicItems },
};

static DDESERVICETBL MyServiceInfo[] = {
    { "HPlanet", 0, ELEMENTS(Topics), 0, Topics }
};

extern DWORD idInst;

void ddeAdvise(void)
{
	int h, i, j;

	for (h = 0; h < ELEMENTS(MyServiceInfo); h++) {
		for (i = 0; i < (int) (MyServiceInfo[h].cTopics - 1); i++) {
			for (j = 0; j < ((int) MyServiceInfo[h].topic[i].cItems); j++) {
	    		DdePostAdvise(idInst, MyServiceInfo[h].topic[i].hszTopic,
					MyServiceInfo[h].topic[i].item[j].hszItem);
			}
		}
	}
}

static void ddeAdviseSelective(int service, int topic)
{
	int j;
	
	for (j = 0; j < ((int) MyServiceInfo[service].topic[topic].cItems); j++) {
		DdePostAdvise(idInst, MyServiceInfo[service].topic[topic].hszTopic,
			MyServiceInfo[service].topic[topic].item[j].hszItem);
	}	
}

void ddeInit(HINSTANCE hInstance)
{
    InitializeDDE(NULL, &idInst, MyServiceInfo,
            CBF_FAIL_EXECUTES | CBF_SKIP_ALLNOTIFICATIONS, hInstance);
}

void ddeTerm(void)
{
    UninitializeDDE();
}
