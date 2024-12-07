#ifndef GEOBOX_HPP
#define GEOBOX_HPP
#include <cstdint>
//#include "helpers/FileIndex.hpp"
#include "helpers/Rectangle.hpp"
#define POS_MASKING_ UINT64_C(0b1111111111111111111111111111111111111111111111111111111111000000)
#define MAS_MASKING_ UINT64_C(0b0000000000000000000000000000000000000000000000000000000000111111)
class GeoBox
{
    private :
    uint64_t _data;
    //uint64_t _pos;
    //int8_t _maskLength;

    public :
    GeoBox():_data(0){};
    uint64_t get_pos() const {return _data & POS_MASKING_;}
    uint8_t get_maskLength() const {return _data & MAS_MASKING_;}
    void set_pos(uint64_t pos){_data = (_data & MAS_MASKING_) + (pos & POS_MASKING_);}
    void set_maskLength(uint8_t m){_data = m + (_data & POS_MASKING_);}
    uint64_t get_data() const{return _data;};
    uint64_t get_hash() const{return _data;};
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




#endif
