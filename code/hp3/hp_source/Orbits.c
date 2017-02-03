/* N3EMO Orbit Simulator routines  v3.7 */

/* Copyright (c) 1986,1987,1988,1989,1990 Robert W. Berger N3EMO
   May be freely distributed, provided this notice remains intact. */

#include <stdio.h>
#include <string.h>
#include <math.h>

#define SSPELLIPSE 0            /* If non zero, use ellipsoidal earth model
                                   when calculating longitude, latitude, and
                                   height */

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

#ifdef PI2
#undef PI2
#endif

#ifdef E
#undef E
#endif

typedef double mat3x3[3][3];

#define PI2 (M_PI * 2)
#define MinutesPerDay (24 * 60.0)
#define SecondsPerDay (60 * MinutesPerDay)
#define HalfSecond (0.5 / SecondsPerDay)
#define JulianCentury		36525.0		// Days in Julian century
#define EarthRadius 6378.16           /* Kilometers, at equator */

#define EarthFlat (1 / 298.25)        /* Earth Flattening Coeff. */
#define SiderealSolar 1.0027379093
#define SidRate (PI2 * SiderealSolar / SecondsPerDay)    /* radians/second */
#define GM 398600                     /* Kilometers^3/seconds^2 */
#define DegreesPerRadian (180 / M_PI)
#define RadiansPerDegree (M_PI / 180)
#define ABS(x) ((x) < 0 ? (-(x)) : (x))
#define SQR(x) ((x) * (x))

#define Epsilon (RadiansPerDegree / 3600)     /* 1 arc second */
#define SunRadius 695000
#define SunSemiMajorAxis 149598845.0       /* Kilometers */

static double SidDay, SidReference;          /* Date and sidereal time */

/* Keplerian elements for the sun */

static double SunEpochTime, SunInclination, SunRAAN, SunEccentricity,
       SunArgPerigee, SunMeanAnomaly, SunMeanMotion;

/* values for shadow geometry */

static double SinPenumbra, CosPenumbra;


/* Solve Kepler's equation                                      */
/* Inputs:                                                      */
/*      MeanAnomaly     Time Since last perigee, in radians.    */
/*                      PI2 = one complete orbit.               */
/*      Eccentricity    Eccentricity of orbit's ellipse.        */
/* Output:                                                      */
/*      TrueAnomaly     Angle between perigee, geocenter, and   */
/*                      current position.                       */

double Kepler(double MeanAnomaly, double Eccentricity)
{
    double E;                         /* Eccentric Anomaly */
    double Error;
    double TrueAnomaly;

    E = MeanAnomaly; /*+ Eccentricity*sin(MeanAnomaly);  Initial guess */
    do {
        Error = (E - Eccentricity * sin(E) - MeanAnomaly)
                / (1 - Eccentricity * cos(E));
        E -= Error;
    } while (ABS(Error) >= Epsilon);

    if (ABS(E - M_PI) < Epsilon)
        TrueAnomaly = M_PI;
      else
        TrueAnomaly = 2 * atan(sqrt((1 + Eccentricity)/(1 - Eccentricity))
                                * tan(E / 2));
    if (TrueAnomaly < 0)
        TrueAnomaly += PI2;

    return TrueAnomaly;
}

void GetSubSatPoint(double SatX, double SatY, double SatZ,
				    double Time,
				    double *Latitude, double *Longitude, double *Height)
{
    double r;
    long i;

    r = sqrt(SQR(SatX) + SQR(SatY) + SQR(SatZ));

    *Longitude = PI2 * ((Time - SidDay) * SiderealSolar + SidReference)
                    - atan2(SatY, SatX);

    /* i = floor(Longitude/2*M_PI)        */
    i = (long) (*Longitude / PI2);
    if (i < 0)
        i--;

    *Longitude -= i * PI2;

    *Latitude = atan(SatZ / sqrt(SQR(SatX) + SQR(SatY)));

#if SSPELLIPSE
#else
    *Height = r - EarthRadius;
#endif
}


void GetPrecession(double SemiMajorAxis, double Eccentricity, double Inclination,
                   double *RAANPrecession, double *PerigeePrecession)
{
  *RAANPrecession = 9.95 * pow(EarthRadius / SemiMajorAxis, 3.5) *
                    cos(Inclination) /
                    SQR(1 - SQR(Eccentricity)) * RadiansPerDegree;
  *PerigeePrecession = 4.97 * pow(EarthRadius / SemiMajorAxis, 3.5)
         * (5 * SQR(cos(Inclination)) - 1)
                 / SQR(1 - SQR(Eccentricity)) * RadiansPerDegree;
}

/* Compute the satellite postion and velocity in the RA based coordinate
   system */

void GetSatPosition(double EpochTime, double EpochRAAN, double EpochArgPerigee,
					double SemiMajorAxis, double Inclination, double Eccentricity,
					double RAANPrecession, double PerigeePrecession, double Time,
					double TrueAnomaly,
					double *X, double *Y, double *Z,
					double *Radius, double *VX, double *VY, double *VZ)
{
    double RAAN, ArgPerigee;

    double Xw, Yw, VXw, VYw;          /* In orbital plane */
    double Tmp;
    double Px, Qx, Py, Qy, Pz, Qz;    /* Escobal transformation 31 */
    double CosArgPerigee, SinArgPerigee;
    double CosRAAN, SinRAAN, CoSinclination, SinInclination;

    *Radius = SemiMajorAxis * (1 - SQR(Eccentricity))
                        / (1 + Eccentricity * cos(TrueAnomaly));

    Xw = *Radius * cos(TrueAnomaly);
    Yw = *Radius * sin(TrueAnomaly);

    Tmp = sqrt(GM / (SemiMajorAxis * (1 - SQR(Eccentricity))));

    VXw = -Tmp * sin(TrueAnomaly);
    VYw =  Tmp * (cos(TrueAnomaly) + Eccentricity);

    ArgPerigee = EpochArgPerigee + (Time - EpochTime) * PerigeePrecession;
    RAAN = EpochRAAN - (Time - EpochTime) * RAANPrecession;

    CosRAAN = cos(RAAN);
    SinRAAN = sin(RAAN);
    CosArgPerigee = cos(ArgPerigee);
    SinArgPerigee = sin(ArgPerigee);
    CoSinclination = cos(Inclination);
    SinInclination = sin(Inclination);

    Px = CosArgPerigee * CosRAAN - SinArgPerigee * SinRAAN * CoSinclination;
    Py = CosArgPerigee * SinRAAN + SinArgPerigee * CosRAAN * CoSinclination;
    Pz = SinArgPerigee * SinInclination;
    Qx = -SinArgPerigee * CosRAAN - CosArgPerigee * SinRAAN * CoSinclination;
    Qy = -SinArgPerigee * SinRAAN + CosArgPerigee * CosRAAN * CoSinclination;
    Qz = CosArgPerigee * SinInclination;

    *X = Px * Xw + Qx * Yw;           /* Escobal, transformation #31 */
    *Y = Py * Xw + Qy * Yw;
    *Z = Pz * Xw + Qz * Yw;

    *VX = Px * VXw + Qx * VYw;
    *VY = Py * VXw + Qy * VYw;
    *VZ = Pz * VXw + Qz * VYw;
}

/* Compute the site postion and velocity in the RA based coordinate
   system. SiteMatrix is set to a matrix which is used by GetTopoCentric
   to convert geocentric coordinates to topocentric (observer-centered)
   coordinates. */

void GetSitPosition(double SiteLat, double SiteLong, double SiteElevation,
					double CurrentTime,
                    double *SiteX, double *SiteY, double *SiteZ,
                    double *SiteVX, double *SiteVY, mat3x3 SiteMatrix)
{
    static double G1, G2; /* Used to correct for flattening of the Earth */
    static double CosLat, SinLat;
    static double OldSiteLat = -100000; /* To avoid unneccesary recomputation */
    static double OldSiteElevation = -100000;
    double Lat;
    double SiteRA;                    /* Right Ascension of site */
    double CosRA, SinRA;

    if ((SiteLat != OldSiteLat) || (SiteElevation != OldSiteElevation)) {
        OldSiteLat = SiteLat;
        OldSiteElevation = SiteElevation;
        Lat = atan(1 / (1 - SQR(EarthFlat)) * tan(SiteLat));

        CosLat = cos(Lat);
        SinLat = sin(Lat);

        G1 = EarthRadius / (sqrt(1 - (2 * EarthFlat - SQR(EarthFlat)) *
                                 SQR(SinLat)));
        G2 = G1 * SQR(1 - EarthFlat);
        G1 += SiteElevation;
        G2 += SiteElevation;
    }

    SiteRA = PI2 * ((CurrentTime - SidDay) * SiderealSolar + SidReference)
                 - SiteLong;
    CosRA = cos(SiteRA);
    SinRA = sin(SiteRA);


    *SiteX = G1 * CosLat * CosRA;
    *SiteY = G1 * CosLat * SinRA;
    *SiteZ = G2 * SinLat;
    *SiteVX = -SidRate * *SiteY;
    *SiteVY = SidRate * *SiteX;

    SiteMatrix[0][0] = SinLat * CosRA;
    SiteMatrix[0][1] = SinLat * SinRA;
    SiteMatrix[0][2] = -CosLat;
    SiteMatrix[1][0] = -SinRA;
    SiteMatrix[1][1] = CosRA;
    SiteMatrix[1][2] = 0.0;
    SiteMatrix[2][0] = CosRA * CosLat;
    SiteMatrix[2][1] = SinRA * CosLat;
    SiteMatrix[2][2] = SinLat;
}

#ifdef NEEDED
void GetRange(double SiteX, double SiteY, double SiteZ,
			  double SiteVX, double SiteVY,
              double SatX, double SatY, double SatZ,
              double SatVX, double SatVY, double SatVZ,
              double *Range, double *RangeRate)
{
    double DX, DY, DZ;

    DX = SatX - SiteX;
    DY = SatY - SiteY;
    DZ = SatZ - SiteZ;

    *Range = sqrt(SQR(DX) + SQR(DY) + SQR(DZ));

    *RangeRate = ((SatVX - SiteVX) * DX + (SatVY - SiteVY) * DY + SatVZ * DZ)
                        / *Range;
}
#endif

/* Convert from geocentric RA based coordinates to topocentric
   (observer centered) coordinates */

static void GetTopocentric(double SatX, double SatY, double SatZ,
						   double SiteX, double SiteY, double SiteZ,
						   mat3x3 SiteMatrix, double *X, double *Y, double *Z)
{
    SatX -= SiteX;
    SatY -= SiteY;
    SatZ -= SiteZ;

    *X = SiteMatrix[0][0] * SatX + SiteMatrix[0][1] * SatY +
         SiteMatrix[0][2] * SatZ;
    *Y = SiteMatrix[1][0] * SatX + SiteMatrix[1][1] * SatY +
         SiteMatrix[1][2] * SatZ;
    *Z = SiteMatrix[2][0] * SatX + SiteMatrix[2][1] * SatY +
         SiteMatrix[2][2] * SatZ;
}

void GetBearings(double SatX, double SatY, double SatZ,
				 double SiteX, double SiteY, double SiteZ,
                 mat3x3 SiteMatrix, double *Azimuth, double *Elevation)
{
    double x, y, z;

    GetTopocentric(SatX, SatY, SatZ, SiteX, SiteY, SiteZ, SiteMatrix,
                   &x, &y, &z);
    *Elevation = atan(z / sqrt(SQR(x) + SQR(y)));
    *Azimuth = M_PI - atan2(y, x);
    if (*Azimuth < 0)
        *Azimuth += M_PI;
}

int Eclipsed(double SatX, double SatY, double SatZ,
			 double SatRadius, double CurrentTime)
{
    double MeanAnomaly, TrueAnomaly;
    double SunX, SunY, SunZ, SunRad;
    double vx, vy, vz;
    double CosTheta;

    MeanAnomaly = SunMeanAnomaly + (CurrentTime - SunEpochTime) *
                  SunMeanMotion * PI2;
    TrueAnomaly = Kepler(MeanAnomaly, SunEccentricity);
    GetSatPosition(SunEpochTime, SunRAAN, SunArgPerigee, SunSemiMajorAxis,
                SunInclination, SunEccentricity, 0.0, 0.0, CurrentTime,
                TrueAnomaly, &SunX, &SunY, &SunZ, &SunRad, &vx, &vy, &vz);
    CosTheta = (SunX * SatX + SunY * SatY + SunZ * SatZ) / (SunRad * SatRadius)
                 * CosPenumbra + (SatRadius / EarthRadius) * SinPenumbra;
    if (CosTheta < 0)
        if (CosTheta < -sqrt(SQR(SatRadius) - SQR(EarthRadius)) / SatRadius
            * CosPenumbra + (SatRadius / EarthRadius) * SinPenumbra)
            return 1;
    return 0;
}

/* Initialize the Sun's Keplerian elements for a given epoch.
   Formulas are from "Explanatory Supplement to the Astronomical Ephemeris".
   Also init the sidereal reference. */

void InitOrbitRoutines(double EpochDay)
{
    double T, T2, T3, Omega;
    int n;
    double SunTrueAnomaly, SunDistance;

    T = (floor(EpochDay) - 0.5) / JulianCentury;
    T2 = T * T;
    T3 = T2 * T;

    SidDay = floor(EpochDay);
    SidReference = (6.6460656 + 2400.051262 * T + 0.00002581 * T2) / 24;
    SidReference -= floor(SidReference);

    /* Omega is used to correct for the nutation and the aberration */

    Omega = (259.18 - 1934.142 * T) * RadiansPerDegree;
    n = (int) (Omega / PI2);
    Omega -= n * PI2;

    SunEpochTime = EpochDay;
    SunRAAN = 0;

    SunInclination = (23.452294 - 0.0130125 * T - 0.00000164 * T2 +
                      0.000000503 * T3 +
                      0.00256 * cos(Omega)) * RadiansPerDegree;
    SunEccentricity = (0.01675104 - 0.00004180 * T - 0.000000126 * T2);
    SunArgPerigee = (281.220833 + 1.719175 * T + 0.0004527 * T2
                        + 0.0000033 * T3) * RadiansPerDegree;
    SunMeanAnomaly = (358.475845 + 35999.04975 * T - 0.00015 * T2
                        - 0.00000333333 * T3) * RadiansPerDegree;
    n = (int) (SunMeanAnomaly / PI2);
    SunMeanAnomaly -= n * PI2;

    SunMeanMotion = 1 / (365.24219879 - 0.00000614 * T);

    SunTrueAnomaly = Kepler(SunMeanAnomaly, SunEccentricity);
    SunDistance = SunSemiMajorAxis * (1 - SQR(SunEccentricity))
                        / (1 + SunEccentricity * cos(SunTrueAnomaly));

    SinPenumbra = (SunRadius - EarthRadius) / SunDistance;
    CosPenumbra = sqrt(1 - SQR(SinPenumbra));
}

static int MonthDays[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

long GetDayNum(int Year, int Month, int Day)
{
    long Result;

    /* Heuristic to allow 4 or 2 digit year specifications */

    if (Year < 50)
        Year += 2000;
    else if (Year < 100)
        Year += 1900;

    Result = ((((long) Year - 1901) * 1461) >> 2) +
             MonthDays[Month - 1] + Day + 365;
    if (Year % 4 == 0 && Month > 2)
        Result++;

    return Result;
}
