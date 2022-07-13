#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP
#include  <cstdint>
struct Rectangle
{
    uint32_t x0;
    uint32_t y0;
    uint32_t x1;
    uint32_t y1;
    bool isValid()const;
    void addPoint(uint32_t x, uint32_t y);
    inline bool isIn(Rectangle const& b)
    {
        if (x0 < b.x0) return false;
        if (y0 < b.y0) return false;
        if (x1 > b.x1) return false;
        if (y1 > b.y1) return false;
	    return true;
    }
};
Rectangle operator*(Rectangle const& a, double const& b);
Rectangle operator*(Rectangle const& a, Rectangle const& b);
Rectangle operator+(Rectangle const& a, Rectangle const& b);
inline bool operator < (Rectangle const& a, Rectangle const& b)
{
    if((a.x1 - a.x0)*(a.y1 - a.y0) > (b.x1 - b.x0)*(b.y1 - b.y0)) return true;
    else if ((a.x1 - a.x0)*(a.y1 - a.y0) != (b.x1 - b.x0)*(b.y1 - b.y0)) return false;
    if (a.x0 < b.x0) return true;
    else if (a.x0 != b.x0) return false;
    if (a.y0 < b.y0) return true;
    else if (a.y0 != b.y0) return false;
    if (a.x1 < b.x1) return true;
    else if (a.x1 != b.x1) return false;
    if (a.y1 < b.y1) return true;
	return false;
}
inline bool operator == (Rectangle const& a, Rectangle const& b)
{
    if (a.x0 != b.x0) return false;
    if (a.y0 != b.y0) return false;
    if (a.x1 != b.x1) return false;
    if (a.y1 != b.y1) return false;
	return true;
}

#endif

