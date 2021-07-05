#ifndef GEOBOX_HPP
#define GEOBOX_HPP
#include <cstdint>
//#include "helpers/FileIndex.hpp"
#include "helpers/Rectangle.hpp"

struct GeoBox
{
    uint64_t pos;
    int8_t maskLength;
    uint32_t zmMask;
};

struct GeoBoxSet
{
    GeoBox boxes[4];
    unsigned char count;
};

uint64_t mergeBits (uint32_t a, uint32_t b);

uint32_t getX (uint64_t pos);

uint32_t getY (uint64_t pos);

uint32_t getXmin(GeoBox& a);
uint32_t getXmax(GeoBox& a);
uint32_t getYmin(GeoBox& a);
uint32_t getYmax(GeoBox& a);

short compareGeoBox(GeoBox const* a, GeoBox const* b);

GeoBox makeGeoBox(Rectangle rect);
GeoBox makeGeoBox(uint32_t x1, uint32_t y1);
GeoBoxSet makeGeoBoxSet(Rectangle rect);
Rectangle getRectangle(GeoBox& g);

bool operator<(GeoBox const& a, GeoBox const& b);
bool operator>(GeoBox const& a, GeoBox const& b);
bool operator==(GeoBox const& a, GeoBox const& b);
bool operator>=(GeoBox const& a, GeoBox const& b);
bool operator<=(GeoBox const& a, GeoBox const& b);


bool hasgoodMask(Rectangle r);


#endif
