#ifndef __SUNRISE_H__
#define __SUNRISE_H__

#include <QDateTime>
#include <math.h>
#include <stdlib.h>

class Sunrise
{
public:  
    
    // constractor (Sunrise with S en majus)
    Sunrise(double latitude_, double longitude_, double elevation_ = 0);
    
    //convert to julian date
    int julian(const QDate &d);


    /*
     * calculation functions
     * param d is local date
     * returns local time format
     */

    QTime sunrise(const QDate &d);
    QTime noon(const QDate &d);
    QTime sunset(const QDate &d);

    //convert angle from .°.'."" to double
    static double angle(int degrees, int minutes, int seconds)
    {
	double a;
	a = seconds;
	a /= 60.0;
	a += minutes;
	a /= 60.0;
	a += degrees;
	return a;
    }



private:   

   //parametres
   const double latitude;
   const double longitude;
   const double elevation;

 
    //  Convert radian angle to degrees
    double dRadToDeg(double dAngleRad)
    {
        return (180.0 * dAngleRad / M_PI);
    }
    //  Convert degree angle to radians
    double dDegToRad(double dAngleDeg)
    {
        return (M_PI * dAngleDeg / 180.0);
    }
    // normlise angle
    double normalizedAngle(double a)
    {
        while (!(a < 360.0))
            a -= 360.0;
        while (!(a > -360.0))
            a += 360.0;
        return a;
    }
    
    //First, start by calculating the number of days since January 1, 2000. 
    //Add that number to 2451545 (the Julian day of January 1, 2000). 
    //This will be variable Jdate.

    //The next step is to calculate the Julian cycle. This is not equal to the 
    //days since Jan 1, 2000. Depending on your longitude, this may be a different
    //number.
    double julianCycle(int iJulianDay)
    {
        double n = double(iJulianDay) - 2451545.0009 - longitude / 360.0;
        return floor(n + 0.5);
    }

    //Now, it is time to approximate the Julian date of solar noon. This is just 
    //an approximation so that we can make some intermediate calculations before 
    //we calculate the actual Julian date of solar noon.
    double solarNoonApprox(int iJulianDay)
    {
        return 2451545.0009  + longitude / 360.0 + julianCycle(iJulianDay);
    }

    //Using the approximate value, calculate the mean solar anomaly. This will get
    //a very close value to the actual mean solar anomaly.
    double solarMean(double noon)
    {
        double m = 357.5291 + 0.98560028 * (noon - 2451545.0);
        return normalizedAngle(m);
    }
    
    //Calculate the equation of center
    double center(double mean)
    {
        double m1 = normalizedAngle(mean * 1.0);  //mean is mean solar anomaly
        double m2 = normalizedAngle(mean * 2.0);
        double m3 = normalizedAngle(mean * 3.0);
        m1 = sin(dDegToRad(m1));
        m2 = sin(dDegToRad(m2));
        m3 = sin(dDegToRad(m3));
        return 1.9148*m1 + 0.0200*m2 + 0.0003*m3;
    }

    //Now, using c(center) and m(mean), calculate gamma (the ecliptical longitude of the sun)
    double gamma(double m)
    {
        double c = center(m);
        return normalizedAngle(m + 102.9372 + c + 180.0);
    }

    //Now there is enough data to calculate an accurate Julian date for solar noon.
    double solarTransit(double noon)
    {
        double m = solarMean(noon);
        double g = gamma(m);
        m = sin(dDegToRad(m));
        g = normalizedAngle(g * 2.0);
        g = sin(dDegToRad(g));
        return noon + 0.0053*m - 0.0069*g;
    }
    
    //To calculate the hour angle we need to find the declination of the sun(sd)
    //Now, calculate the hour angle(o), which corresponds to half of the arc length 
    //of the sun at this latitude at this declination of the sun
    bool omega(double &o, double noon)
    {
        double m = solarMean(noon);
        double g = gamma(m);
        double l = dDegToRad(latitude);
        double sd = sin(dDegToRad(g)) * sin(dDegToRad(23.45));
        double cd = cos(asin(sd));//declination of the sun (asin(sd))

        double el = 0;
        if (elevation > 0.0) {
          el = -2.076;
          el *= sqrt(elevation) / 60.0;
        }
        o = sin(dDegToRad(-0.83 + el)) - sin(l)*sd;
        o /= cos(l) * cd;
        if (o > 1.0 || o < -1.0)
          return false; // If o (hour angle) is undefined, then there is either no sunrise 
                 //(in winter) or no sunset (in summer) for the supplied latitude.
        o = dRadToDeg(acos(o));
        return true;
    }

    //Okay, time to go back through the approximation again, this time we use H in the calculation
    double sunset(int iJulianDay)
     {
        double noon = solarNoonApprox(iJulianDay);
        double m = solarMean(noon);
        double g = gamma(m);
        m = sin(dDegToRad(m));
        g = normalizedAngle(g * 2.0);
        g = sin(dDegToRad(g));
        double o;
        if (!omega(o, noon))
          return -1.0;
        o += longitude;
        o /= 360.0;
        return 2451545.0009 + o + julianCycle(iJulianDay) + 0.0053*m - 0.0069*g;
        //The values of m and gamma from above don't really change from solar noon to 
        //sunset, so there is no need to recalculate them before calculating sunset.
    }


    double noon(int iJulianDay)
    {
        double noon = solarNoonApprox(iJulianDay);
        return solarTransit(noon);
    }

    //Instead of going through that mess again, assume that solar noon is half-way
    //between sunrise and sunset (valid for latitudes < 60) and approximate sunrise.
    double sunrise(int iJulianDay)
    {
        double transit = noon(iJulianDay);
        double ss = sunset(iJulianDay);
        return ss < 0.0 ? -1.0 : transit - (ss - transit);
        
    }
    
    //At this point you should convert the Julian dates to something more readable like a regular date. 
    //This will be left as an exercise to the reader.
    
     QDateTime date(double julian);
};

#endif