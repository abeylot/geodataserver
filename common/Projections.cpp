#include "Projections.hpp"
#include <math.h>
#include <iostream>

double WebMercatorProj::lat2y(const double lat) const
{
    double angle = lat;

    double angleRadians  =  angle * M_PI / 180.0;
    bool signe = false;
    if(angleRadians < 0) {
        angleRadians  = -1.0 * angleRadians;
        signe = true;
    }

    double dist = log(tan((angleRadians/2.0 + M_PI/4.0)));

    double normalized = 0;

    if(signe) normalized = 0b1000000000000000000000000000000  * (1 + dist/(2.5 * M_PI));
    else normalized =       0b1000000000000000000000000000000 * (1 - dist/(2.5 * M_PI));
    //std::cout << normalized << "\n";
    return normalized;

}

double WebMercatorProj::lon2x(const double lon) const
{
   return lon;
}

/*
double::MercatorProj::lat2y(const double lat) const
{
   return lat;
}

double MercatorProj::lon2x(const double lon) const
{
   return lon;
}*/
