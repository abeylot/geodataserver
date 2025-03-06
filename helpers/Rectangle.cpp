#include "Rectangle.hpp"

bool Rectangle::isValid() const
{
    return (
               (x0 <= x1) &&
               (y0 <= y1) &&
               (x0 != UINT32_C(0xFFFFFFFF)) &&
               (x1 != UINT32_C(0xFFFFFFFF)) &&
               (y0 != UINT32_C(0xFFFFFFFF)) &&
               (y1 != UINT32_C(0xFFFFFFFF))
           );
}

void Rectangle::addPoint(uint32_t x, uint32_t y)
{
    if( ! isValid())
    {
        x0=x1=x;
        y0=y1=x;
    }
    else
    {
        if(x < x0) x0 = x;
        if(x > x1) x1 = x;
        if(y < y0) y0 = y;
        if(y > y1) y1 = y;
    }
}

Rectangle operator*(Rectangle const& a, double const& b)
{
    Rectangle result;


    double coef = b/2.0;

    uint64_t difx = (a.x1 - a.x0);
    uint64_t dify = (a.y1 - a.y0);

    uint32_t meanx = a.x0 + (difx / UINT32_C(2));
    uint32_t meany = a.y0 + (dify / UINT32_C(2));

    difx *= coef;
    dify *= coef;
    
    int64_t x0 = meanx;
    int64_t x1 = meanx;
    int64_t y0 = meany;
    int64_t y1 = meany;
    
    x0 -= difx;
    x1 += difx;
    y0 -= dify;
    y1 += dify;
    
    if (x0 < 0) x0 = 0; 
    if (y0 < 0) y0 = 0; 

    if (x1 > 0xFFFFFFFF) x1 = 0xFFFFFFFF; 
    if (y1 > 0xFFFFFFFF) y1 = 0xFFFFFFFF; 


    result.x0 = x0;
    result.x1 = x1;
    result.y0 = y0;
    result.y1 = y1;
    
    
    return result;
}

Rectangle operator*(Rectangle const& a, Rectangle const& b)
{
    Rectangle result;
    if(! a.isValid() || !b.isValid())
    {
        result.x0 = result.y0 = result.x1 = result.y1 = UINT32_C(0xFFFFFFFF);
        return result;
    }

    if(a.x0 > b.x0) result.x0 = a.x0;
    else result.x0 = b.x0;

    if(a.x1 < b.x1) result.x1 = a.x1;
    else result.x1 = b.x1;

    if(a.y0 > b.y0) result.y0 = a.y0;
    else result.y0 = b.y0;

    if(a.y1 < b.y1) result.y1 = a.y1;
    else result.y1 = b.y1;

    return result;
}

Rectangle operator+(Rectangle const& a, Rectangle const& b)
{
    Rectangle result;

    if (! a.isValid() ) return b;
    if (! b.isValid() ) return a;

    if(a.x0 < b.x0) result.x0 = a.x0;
    else result.x0 = b.x0;

    if(a.x1 > b.x1) result.x1 = a.x1;
    else result.x1 = b.x1;

    if(a.y0 < b.y0) result.y0 = a.y0;
    else result.y0 = b.y0;

    if(a.y1 > b.y1) result.y1 = a.y1;
    else result.y1 = b.y1;

    return result;
}

