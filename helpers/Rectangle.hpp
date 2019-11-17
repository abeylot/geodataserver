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
};
Rectangle operator*(Rectangle const& a, double const& b);
Rectangle operator*(Rectangle const& a, Rectangle const& b);
Rectangle operator+(Rectangle const& a, Rectangle const& b);
#endif

