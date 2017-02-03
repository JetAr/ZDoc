
#ifdef HubbleGSC

#include "gscrgn.h"

/*
    This set of routines numbers declination bands as follows :
        Band value :            Dec Range :
            0                   -90.0   to  -82.5
            1                   -82.5   to  -75.0
            2                   -75.0   to  -67.5
            3                   -67.5   to  -60.0
            .... etc.
            21                  67.5    to  75.0
            22                  75.0    to  82.5
            23                  82.5    to  90.0

            band = (int)(dec + 90.0) / 7.5;
*/

/* guide star catalog region quick index tables */

/* GSCFolders gives GSC file names for declination bands */

char *GSCFolders[25] = {
    "S8230", "S7500", "S6730", "S6000", "S5230", "S4500",
    "S3730", "S3000", "S2230", "S1500", "S0730", "S0000",
    "N0000", "N0730", "N1500", "N2230", "N3000", "N3730",
    "N4500", "N5230", "N6000", "N6730", "N7500", "N8230"
};

/* NumInBand gives the number of GSC large regions in a declination band */

char NumInBand[25] = {
     3,  9, 15, 21, 27, 32, 36, 40, 43, 45, 47, 48,
    48, 47, 45, 43, 40, 36, 32, 27, 21, 15,  9,  3,  3
};

/* FirstInBand gives the first large region number in a declination band */

short FirstInBand[25] = {
    729, 720, 705, 684, 657, 625, 589, 549, 506, 461, 414, 366,
      0,  48,  95, 140, 183, 223, 259, 291, 318, 339, 354, 363, 363
};

/* FirstSmlRgn gives the first small region number in a large region */

short FirstSmlRgn[732] = {
       1,   10,   19,   28,   37,   46,   55,   64,
      73,   82,   98,  114,  130,  146,  162,  178,
     194,  210,  226,  235,  244,  253,  262,  271,
     280,  289,  298,  307,  316,  325,  334,  350,
     366,  382,  398,  414,  430,  446,  462,  478,
     494,  510,  526,  542,  558,  567,  576,  585,
     594,  603,  612,  621,  630,  639,  648,  657,
     666,  682,  698,  714,  730,  746,  762,  778,
     794,  810,  819,  828,  837,  846,  855,  864,
     873,  882,  891,  900,  909,  918,  927,  943,
     959,  975,  991, 1007, 1023, 1039, 1055, 1071,
    1087, 1103, 1119, 1135, 1151, 1160, 1169, 1178,
    1187, 1196, 1205, 1214, 1223, 1232, 1248, 1264,
    1280, 1296, 1312, 1328, 1344, 1360, 1376, 1392,
    1401, 1410, 1419, 1428, 1437, 1441, 1445, 1449,
    1458, 1467, 1476, 1485, 1494, 1503, 1519, 1535,
    1551, 1567, 1583, 1599, 1615, 1631, 1647, 1663,
    1679, 1695, 1711, 1720, 1729, 1738, 1747, 1756,
    1765, 1781, 1797, 1813, 1829, 1845, 1861, 1877,
    1893, 1909, 1925, 1941, 1950, 1959, 1968, 1977,
    1981, 1985, 1989, 1993, 1997, 2006, 2015, 2024,
    2033, 2042, 2058, 2074, 2090, 2106, 2122, 2138,
    2154, 2170, 2186, 2202, 2218, 2234, 2250, 2259,
    2275, 2291, 2307, 2323, 2339, 2355, 2371, 2387,
    2403, 2419, 2435, 2451, 2467, 2483, 2492, 2501,
    2510, 2519, 2523, 2527, 2531, 2535, 2544, 2553,
    2562, 2571, 2580, 2589, 2605, 2621, 2637, 2653,
    2669, 2685, 2701, 2717, 2733, 2749, 2765, 2781,
    2797, 2813, 2829, 2845, 2861, 2877, 2893, 2909,
    2925, 2941, 2957, 2973, 2982, 2991, 3000, 3009,
    3013, 3017, 3021, 3025, 3034, 3043, 3052, 3061,
    3070, 3086, 3102, 3118, 3134, 3150, 3166, 3182,
    3198, 3214, 3230, 3246, 3262, 3278, 3294, 3310,
    3326, 3342, 3358, 3374, 3390, 3406, 3415, 3424,
    3433, 3442, 3451, 3455, 3459, 3463, 3472, 3481,
    3490, 3499, 3508, 3524, 3540, 3556, 3572, 3588,
    3604, 3620, 3636, 3652, 3668, 3684, 3700, 3716,
    3732, 3748, 3764, 3780, 3796, 3805, 3814, 3823,
    3832, 3841, 3850, 3859, 3868, 3877, 3886, 3902,
    3918, 3934, 3950, 3966, 3982, 3998, 4014, 4030,
    4046, 4062, 4078, 4094, 4110, 4126, 4135, 4144,
    4153, 4162, 4171, 4180, 4189, 4198, 4214, 4230,
    4246, 4262, 4278, 4294, 4310, 4326, 4342, 4358,
    4374, 4383, 4392, 4401, 4410, 4419, 4428, 4444,
    4460, 4476, 4492, 4508, 4524, 4540, 4549, 4558,
    4567, 4583, 4599, 4615, 4631, 4647, 4663, 4672,
    4681, 4685, 4689, 4698, 4707, 4716, 4725, 4734,
    4750, 4766, 4782, 4798, 4814, 4830, 4846, 4862,
    4878, 4894, 4903, 4912, 4921, 4930, 4939, 4948,
    4957, 4966, 4975, 4984, 5000, 5016, 5032, 5048,
    5064, 5080, 5096, 5112, 5128, 5144, 5160, 5176,
    5192, 5208, 5224, 5233, 5242, 5251, 5260, 5269,
    5273, 5277, 5281, 5285, 5294, 5303, 5312, 5321,
    5330, 5346, 5362, 5378, 5394, 5410, 5426, 5442,
    5458, 5474, 5490, 5499, 5508, 5517, 5526, 5535,
    5544, 5553, 5562, 5578, 5594, 5610, 5626, 5642,
    5658, 5674, 5690, 5706, 5722, 5738, 5754, 5770,
    5786, 5802, 5811, 5820, 5829, 5838, 5847, 5851,
    5855, 5859, 5863, 5872, 5881, 5890, 5899, 5915,
    5931, 5947, 5963, 5979, 5995, 6011, 6027, 6043,
    6059, 6075, 6084, 6093, 6102, 6111, 6120, 6136,
    6152, 6168, 6184, 6200, 6216, 6232, 6248, 6264,
    6280, 6296, 6312, 6328, 6344, 6360, 6376, 6385,
    6394, 6403, 6412, 6421, 6425, 6429, 6433, 6437,
    6446, 6455, 6464, 6473, 6489, 6505, 6521, 6537,
    6553, 6569, 6585, 6601, 6617, 6633, 6649, 6665,
    6681, 6697, 6713, 6729, 6745, 6761, 6777, 6793,
    6809, 6825, 6841, 6857, 6873, 6889, 6905, 6921,
    6937, 6953, 6962, 6971, 6980, 6989, 6998, 7002,
    7006, 7010, 7019, 7028, 7037, 7046, 7055, 7071,
    7087, 7103, 7119, 7135, 7151, 7167, 7183, 7199,
    7215, 7231, 7247, 7263, 7279, 7295, 7311, 7327,
    7343, 7359, 7375, 7391, 7407, 7423, 7439, 7455,
    7471, 7487, 7496, 7505, 7514, 7523, 7532, 7541,
    7550, 7559, 7568, 7577, 7586, 7595, 7611, 7627,
    7643, 7659, 7675, 7691, 7707, 7723, 7739, 7755,
    7771, 7787, 7803, 7819, 7835, 7851, 7867, 7883,
    7899, 7915, 7931, 7947, 7963, 7979, 7995, 8004,
    8013, 8022, 8031, 8040, 8049, 8058, 8067, 8076,
    8085, 8101, 8117, 8133, 8149, 8165, 8181, 8197,
    8213, 8229, 8245, 8261, 8277, 8293, 8309, 8325,
    8341, 8357, 8373, 8389, 8405, 8421, 8437, 8446,
    8455, 8464, 8473, 8482, 8491, 8500, 8509, 8518,
    8534, 8550, 8566, 8582, 8598, 8614, 8630, 8646,
    8662, 8678, 8694, 8710, 8726, 8742, 8758, 8774,
    8790, 8806, 8822, 8831, 8840, 8849, 8858, 8867,
    8876, 8892, 8908, 8924, 8940, 8956, 8972, 8988,
    9004, 9020, 9036, 9052, 9068, 9084, 9100, 9116,
    9125, 9134, 9143, 9152, 9161, 9177, 9193, 9209,
    9225, 9241, 9257, 9273, 9289, 9305, 9321, 9337,
    9346, 9362, 9378, 9394, 9410, 9426, 9442, 9458,
    9474, 9490, 9506, 9522
};

/* the square of NumSmlRgns is the number of small regions in the large region */

char NumSmlRgns[732] = {
     3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,
     4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,
     3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,
     3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,
     3,  3,  3,  3,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  3,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  2,
     2,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  3,  3,  3,  3,  2,  2,  2,  2,  3,  3,  3,
     3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  2,
     2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,
     3,  3,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,
     3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  4,  4,
     4,  4,  4,  4,  4,  3,  3,  3,  4,  4,  4,  4,  4,  4,  3,  3,
     2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  2,
     2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  2,  2,
     2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  2,  2,  2,  2,  3,
     3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  3,  3,  3,  3,  3,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  3,
     3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,
     3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,
     3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  3,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,
     3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,
     4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4
};

/*
    Given an ra & dec in dec. deg. GSCRegions returns the large and small
    GSC region numbers (and their boundaries) which contain the coordinate.
*/

void GSCRegions(double ra, double dec, GSCRgnInfoPtr G)
{
    int Ldecband, Lraband;
    int Sdecband, Sraband;

    /* normalize ra & dec */
    while (dec > 90.0 || dec < -90.0) {
        if (dec >  90.0) {  dec = 180.0 - dec;  ra += 180.0; }
        if (dec < -90.0) {  dec = 180.0 + dec;  ra += 180.0; }
    }
    while (ra <   0.0) ra += 360.0;
    while (ra > 360.0) ra -= 360.0;

    G->Lg.decwidth = 7.5;
    Ldecband = (int) ((dec + 90.0) / G->Lg.decwidth);
    if (Ldecband > 23) Ldecband = 23;
    G->Band = Ldecband;

    Lraband = (int)(ra / 360.0 * NumInBand[Ldecband]);
    if (Lraband >= NumInBand[Ldecband]) Lraband = NumInBand[Ldecband] - 1;
    G->LgRgn = FirstInBand[Ldecband] + Lraband;
    G->Lg.rawidth = 360.0 / NumInBand[Ldecband];
    G->Lg.lodec = Ldecband * G->Lg.decwidth - 90.0;
    G->Lg.lora  = Lraband  * G->Lg.rawidth;
    G->Lg.hidec = G->Lg.lodec + G->Lg.decwidth;
    G->Lg.hira  = G->Lg.lora  + G->Lg.rawidth;

    G->Sm.rawidth = G->Lg.rawidth / NumSmlRgns[G->LgRgn];
    Sraband = (int) ((ra - G->Lg.lora) / G->Sm.rawidth);
    if (Sraband > NumSmlRgns[G->LgRgn]-1) Sraband = NumSmlRgns[G->LgRgn] - 1;
    G->Sm.decwidth = G->Lg.decwidth / NumSmlRgns[G->LgRgn];
    if (dec >= 0.0) {
        Sdecband = (int) ((dec - G->Lg.lodec) / G->Sm.decwidth);
        if (Sdecband >= NumSmlRgns[G->LgRgn]) Sdecband = NumSmlRgns[G->LgRgn] - 1;
        G->Sm.lodec = G->Lg.lodec + Sdecband * G->Sm.decwidth;
        G->Sm.hidec = G->Sm.lodec + G->Sm.decwidth;
    } else {
        Sdecband = (int) ((G->Lg.hidec - dec) / G->Sm.decwidth);
        if (Sdecband >= NumSmlRgns[G->LgRgn]) Sdecband = NumSmlRgns[G->LgRgn] - 1;
        G->Sm.hidec = G->Lg.hidec - Sdecband * G->Sm.decwidth;
        G->Sm.lodec = G->Sm.hidec - G->Sm.decwidth;
    }
    G->Sm.lora  = G->Lg.lora  + Sraband  * G->Sm.rawidth;
    G->Sm.hira  = G->Sm.lora  + G->Sm.rawidth;

    G->SmlRgn = FirstSmlRgn[G->LgRgn] + NumSmlRgns[G->LgRgn] * Sdecband + Sraband;
}

/* LargeRegionInfo gives information on a large region given its number and dec. band */

void LargeRegionInfo(GSCRgnInfoPtr G)
{
    int Lraband;

    G->Lg.decwidth = 7.5;
    Lraband = G->LgRgn - FirstInBand[G->Band];
    G->Lg.rawidth = 360.0 / NumInBand[G->Band];
    G->Lg.lodec = G->Band * G->Lg.decwidth - 90.0;
    G->Lg.lora  = Lraband  * G->Lg.rawidth;
    G->Lg.hidec = G->Lg.lodec + G->Lg.decwidth;
    G->Lg.hira  = G->Lg.lora  + G->Lg.rawidth;
}

/* SmallRegionInfo gives information on a small region given its number and
    the large region information. (i.e. you must've called LargeRegionInfo(G); )
*/
void SmallRegionInfo(GSCRgnInfoPtr G)
{
    int Sraband, Sdecband, Scell;

    Scell = G->SmlRgn - FirstSmlRgn[G->LgRgn];
    Sdecband = Scell / NumSmlRgns[G->LgRgn];
    Sraband = Scell - Sdecband * NumSmlRgns[G->LgRgn];
    G->Sm.rawidth = G->Lg.rawidth / NumSmlRgns[G->LgRgn];
    G->Sm.decwidth = G->Lg.decwidth / NumSmlRgns[G->LgRgn];
    if (G->Band >= 12) {
        G->Sm.lodec = G->Lg.lodec + Sdecband * G->Sm.decwidth;
        G->Sm.hidec = G->Sm.lodec + G->Sm.decwidth;
    } else {
        G->Sm.hidec = G->Lg.hidec - Sdecband * G->Sm.decwidth;
        G->Sm.lodec = G->Sm.hidec - G->Sm.decwidth;
    }
    G->Sm.lora  = G->Lg.lora  + Sraband  * G->Sm.rawidth;
    G->Sm.hira  = G->Sm.lora  + G->Sm.rawidth;
}


/* find the dec band and large region that a small region is in */

int FindSmallRegion(int rgn, GSCRgnInfoPtr G)
{
    int i, band, firstlg, lastlg, lgrgn;
    if (rgn<1 || rgn>9537)
    	return 0;

    for (i=0; i<23; ++i) {
        if (rgn >= FirstSmlRgn[FirstInBand[i]]
        && rgn < FirstSmlRgn[FirstInBand[i+1]]) break;
    }
    band = i;
    firstlg = FirstInBand[band];
    lastlg = FirstInBand[band] + NumInBand[band] - 1;

    for (i=firstlg; i<= lastlg && FirstSmlRgn[i]-1<rgn; ++i) ;
    lgrgn = i;
    G->Band = band;
    G->LgRgn = lgrgn;
    G->SmlRgn = rgn;
    return 1;
}
#endif

