#include "GeoBox.hpp"
#include <string.h>

// 32 BITS CONSTANTS

#define UINT32_BIT0  UINT32_C(0x00000001)
#define UINT32_BIT1  UINT32_C(0x00000002)
#define UINT32_BIT2  UINT32_C(0x00000004)
#define UINT32_BIT3  UINT32_C(0x00000008)

#define UINT32_BIT4  UINT32_C(0x00000010)
#define UINT32_BIT5  UINT32_C(0x00000020)
#define UINT32_BIT6  UINT32_C(0x00000040)
#define UINT32_BIT7  UINT32_C(0x00000080)

#define UINT32_BIT8  UINT32_C(0x00000100)
#define UINT32_BIT9  UINT32_C(0x00000200)
#define UINT32_BIT10 UINT32_C(0x00000400)
#define UINT32_BIT11 UINT32_C(0x00000800)

#define UINT32_BIT12 UINT32_C(0x00001000)
#define UINT32_BIT13 UINT32_C(0x00002000)
#define UINT32_BIT14 UINT32_C(0x00004000)
#define UINT32_BIT15 UINT32_C(0x00008000)

#define UINT32_BIT16 UINT32_C(0x00010000)
#define UINT32_BIT17 UINT32_C(0x00020000)
#define UINT32_BIT18 UINT32_C(0x00040000)
#define UINT32_BIT19 UINT32_C(0x00080000)

#define UINT32_BIT20 UINT32_C(0x00100000)
#define UINT32_BIT21 UINT32_C(0x00200000)
#define UINT32_BIT22 UINT32_C(0x00400000)
#define UINT32_BIT23 UINT32_C(0x00800000)

#define UINT32_BIT24 UINT32_C(0x01000000)
#define UINT32_BIT25 UINT32_C(0x02000000)
#define UINT32_BIT26 UINT32_C(0x04000000)
#define UINT32_BIT27 UINT32_C(0x08000000)

#define UINT32_BIT28 UINT32_C(0x10000000)
#define UINT32_BIT29 UINT32_C(0x20000000)
#define UINT32_BIT30 UINT32_C(0x40000000)
#define UINT32_BIT31 UINT32_C(0x80000000)


// 64 BITS CONSTANTS

#define UINT64_BIT0  UINT64_C(0x0000000000000001)
#define UINT64_BIT1  UINT64_C(0x0000000000000002)
#define UINT64_BIT2  UINT64_C(0x0000000000000004)
#define UINT64_BIT3  UINT64_C(0x0000000000000008)

#define UINT64_BIT4  UINT64_C(0x0000000000000010)
#define UINT64_BIT5  UINT64_C(0x0000000000000020)
#define UINT64_BIT6  UINT64_C(0x0000000000000040)
#define UINT64_BIT7  UINT64_C(0x0000000000000080)

#define UINT64_BIT8  UINT64_C(0x0000000000000100)
#define UINT64_BIT9  UINT64_C(0x0000000000000200)
#define UINT64_BIT10 UINT64_C(0x0000000000000400)
#define UINT64_BIT11 UINT64_C(0x0000000000000800)

#define UINT64_BIT12 UINT64_C(0x0000000000001000)
#define UINT64_BIT13 UINT64_C(0x0000000000002000)
#define UINT64_BIT14 UINT64_C(0x0000000000004000)
#define UINT64_BIT15 UINT64_C(0x0000000000008000)

#define UINT64_BIT16 UINT64_C(0x0000000000010000)
#define UINT64_BIT17 UINT64_C(0x0000000000020000)
#define UINT64_BIT18 UINT64_C(0x0000000000040000)
#define UINT64_BIT19 UINT64_C(0x0000000000080000)

#define UINT64_BIT20 UINT64_C(0x0000000000100000)
#define UINT64_BIT21 UINT64_C(0x0000000000200000)
#define UINT64_BIT22 UINT64_C(0x0000000000400000)
#define UINT64_BIT23 UINT64_C(0x0000000000800000)

#define UINT64_BIT24 UINT64_C(0x0000000001000000)
#define UINT64_BIT25 UINT64_C(0x0000000002000000)
#define UINT64_BIT26 UINT64_C(0x0000000004000000)
#define UINT64_BIT27 UINT64_C(0x0000000008000000)

#define UINT64_BIT28 UINT64_C(0x0000000010000000)
#define UINT64_BIT29 UINT64_C(0x0000000020000000)
#define UINT64_BIT30 UINT64_C(0x0000000040000000)
#define UINT64_BIT31 UINT64_C(0x0000000080000000)


#define UINT64_BIT32 UINT64_C(0x0000000100000000)
#define UINT64_BIT33 UINT64_C(0x0000000200000000)
#define UINT64_BIT34 UINT64_C(0x0000000400000000)
#define UINT64_BIT35 UINT64_C(0x0000000800000000)

#define UINT64_BIT36 UINT64_C(0x0000001000000000)
#define UINT64_BIT37 UINT64_C(0x0000002000000000)
#define UINT64_BIT38 UINT64_C(0x0000004000000000)
#define UINT64_BIT39 UINT64_C(0x0000008000000000)

#define UINT64_BIT40 UINT64_C(0x0000010000000000)
#define UINT64_BIT41 UINT64_C(0x0000020000000000)
#define UINT64_BIT42 UINT64_C(0x0000040000000000)
#define UINT64_BIT43 UINT64_C(0x0000080000000000)

#define UINT64_BIT44 UINT64_C(0x0000100000000000)
#define UINT64_BIT45 UINT64_C(0x0000200000000000)
#define UINT64_BIT46 UINT64_C(0x0000400000000000)
#define UINT64_BIT47 UINT64_C(0x0000800000000000)

#define UINT64_BIT48 UINT64_C(0x0001000000000000)
#define UINT64_BIT49 UINT64_C(0x0002000000000000)
#define UINT64_BIT50 UINT64_C(0x0004000000000000)
#define UINT64_BIT51 UINT64_C(0x0008000000000000)

#define UINT64_BIT52 UINT64_C(0x0010000000000000)
#define UINT64_BIT53 UINT64_C(0x0020000000000000)
#define UINT64_BIT54 UINT64_C(0x0040000000000000)
#define UINT64_BIT55 UINT64_C(0x0080000000000000)

#define UINT64_BIT56 UINT64_C(0x0100000000000000)
#define UINT64_BIT57 UINT64_C(0x0200000000000000)
#define UINT64_BIT58 UINT64_C(0x0400000000000000)
#define UINT64_BIT59 UINT64_C(0x0800000000000000)

#define UINT64_BIT60 UINT64_C(0x1000000000000000)
#define UINT64_BIT61 UINT64_C(0x2000000000000000)
#define UINT64_BIT62 UINT64_C(0x4000000000000000)
#define UINT64_BIT63 UINT64_C(0x8000000000000000)


uint64_t mergeBits (uint32_t a, uint32_t b)
{

    uint64_t result = 0;

    //if( a & UINT32_BIT0) result |= UINT64_BIT0;
    //if( a & UINT32_BIT1) result |= UINT64_BIT2;
    //if( a & UINT32_BIT2) result |= UINT64_BIT4;
    if( a & UINT32_BIT3) result |= UINT64_BIT6;
    if( a & UINT32_BIT4) result |= UINT64_BIT8;
    if( a & UINT32_BIT5) result |= UINT64_BIT10;
    if( a & UINT32_BIT6) result |= UINT64_BIT12;
    if( a & UINT32_BIT7) result |= UINT64_BIT14;
    if( a & UINT32_BIT8) result |= UINT64_BIT16;
    if( a & UINT32_BIT9) result |= UINT64_BIT18;
    if( a & UINT32_BIT10) result |= UINT64_BIT20;
    if( a & UINT32_BIT11) result |= UINT64_BIT22;
    if( a & UINT32_BIT12) result |= UINT64_BIT24;
    if( a & UINT32_BIT13) result |= UINT64_BIT26;
    if( a & UINT32_BIT14) result |= UINT64_BIT28;
    if( a & UINT32_BIT15) result |= UINT64_BIT30;
    if( a & UINT32_BIT16) result |= UINT64_BIT32;
    if( a & UINT32_BIT17) result |= UINT64_BIT34;
    if( a & UINT32_BIT18) result |= UINT64_BIT36;
    if( a & UINT32_BIT19) result |= UINT64_BIT38;
    if( a & UINT32_BIT20) result |= UINT64_BIT40;
    if( a & UINT32_BIT21) result |= UINT64_BIT42;
    if( a & UINT32_BIT22) result |= UINT64_BIT44;
    if( a & UINT32_BIT23) result |= UINT64_BIT46;
    if( a & UINT32_BIT24) result |= UINT64_BIT48;
    if( a & UINT32_BIT25) result |= UINT64_BIT50;
    if( a & UINT32_BIT26) result |= UINT64_BIT52;
    if( a & UINT32_BIT27) result |= UINT64_BIT54;
    if( a & UINT32_BIT28) result |= UINT64_BIT56;
    if( a & UINT32_BIT29) result |= UINT64_BIT58;
    if( a & UINT32_BIT30) result |= UINT64_BIT60;
    if( a & UINT32_BIT31) result |= UINT64_BIT62;

    //if( b & UINT32_BIT0) result |= UINT64_BIT1;
    //if( b & UINT32_BIT1) result |= UINT64_BIT3;
    //if( b & UINT32_BIT2) result |= UINT64_BIT5;
    if( b & UINT32_BIT3) result |= UINT64_BIT7;
    if( b & UINT32_BIT4) result |= UINT64_BIT9;
    if( b & UINT32_BIT5) result |= UINT64_BIT11;
    if( b & UINT32_BIT6) result |= UINT64_BIT13;
    if( b & UINT32_BIT7) result |= UINT64_BIT15;
    if( b & UINT32_BIT8) result |= UINT64_BIT17;
    if( b & UINT32_BIT9) result |= UINT64_BIT19;
    if( b & UINT32_BIT10) result |= UINT64_BIT21;
    if( b & UINT32_BIT11) result |= UINT64_BIT23;
    if( b & UINT32_BIT12) result |= UINT64_BIT25;
    if( b & UINT32_BIT13) result |= UINT64_BIT27;
    if( b & UINT32_BIT14) result |= UINT64_BIT29;
    if( b & UINT32_BIT15) result |= UINT64_BIT31;
    if( b & UINT32_BIT16) result |= UINT64_BIT33;
    if( b & UINT32_BIT17) result |= UINT64_BIT35;
    if( b & UINT32_BIT18) result |= UINT64_BIT37;
    if( b & UINT32_BIT19) result |= UINT64_BIT39;
    if( b & UINT32_BIT20) result |= UINT64_BIT41;
    if( b & UINT32_BIT21) result |= UINT64_BIT43;
    if( b & UINT32_BIT22) result |= UINT64_BIT45;
    if( b & UINT32_BIT23) result |= UINT64_BIT47;
    if( b & UINT32_BIT24) result |= UINT64_BIT49;
    if( b & UINT32_BIT25) result |= UINT64_BIT51;
    if( b & UINT32_BIT26) result |= UINT64_BIT53;
    if( b & UINT32_BIT27) result |= UINT64_BIT55;
    if( b & UINT32_BIT28) result |= UINT64_BIT57;
    if( b & UINT32_BIT29) result |= UINT64_BIT59;
    if( b & UINT32_BIT30) result |= UINT64_BIT61;
    if( b & UINT32_BIT31) result |= UINT64_BIT63;

    return result;
}



uint32_t getX (uint64_t pos)
{
    uint32_t result = 0;
    //if( pos & UINT64_BIT0) result |= UINT32_BIT0;
    //if( pos & UINT64_BIT2) result |= UINT32_BIT1;
    //if( pos & UINT64_BIT4) result |= UINT32_BIT2;
    if( pos & UINT64_BIT6) result |= UINT32_BIT3;
    if( pos & UINT64_BIT8) result |= UINT32_BIT4;
    if( pos & UINT64_BIT10) result |= UINT32_BIT5;
    if( pos & UINT64_BIT12) result |= UINT32_BIT6;
    if( pos & UINT64_BIT14) result |= UINT32_BIT7;
    if( pos & UINT64_BIT16) result |= UINT32_BIT8;
    if( pos & UINT64_BIT18) result |= UINT32_BIT9;
    if( pos & UINT64_BIT20) result |= UINT32_BIT10;
    if( pos & UINT64_BIT22) result |= UINT32_BIT11;
    if( pos & UINT64_BIT24) result |= UINT32_BIT12;
    if( pos & UINT64_BIT26) result |= UINT32_BIT13;
    if( pos & UINT64_BIT28) result |= UINT32_BIT14;
    if( pos & UINT64_BIT30) result |= UINT32_BIT15;
    if( pos & UINT64_BIT32) result |= UINT32_BIT16;
    if( pos & UINT64_BIT34) result |= UINT32_BIT17;
    if( pos & UINT64_BIT36) result |= UINT32_BIT18;
    if( pos & UINT64_BIT38) result |= UINT32_BIT19;
    if( pos & UINT64_BIT40) result |= UINT32_BIT20;
    if( pos & UINT64_BIT42) result |= UINT32_BIT21;
    if( pos & UINT64_BIT44) result |= UINT32_BIT22;
    if( pos & UINT64_BIT46) result |= UINT32_BIT23;
    if( pos & UINT64_BIT48) result |= UINT32_BIT24;
    if( pos & UINT64_BIT50) result |= UINT32_BIT25;
    if( pos & UINT64_BIT52) result |= UINT32_BIT26;
    if( pos & UINT64_BIT54) result |= UINT32_BIT27;
    if( pos & UINT64_BIT56) result |= UINT32_BIT28;
    if( pos & UINT64_BIT58) result |= UINT32_BIT29;
    if( pos & UINT64_BIT60) result |= UINT32_BIT30;
    if( pos & UINT64_BIT62) result |= UINT32_BIT31;
    return result;
}
uint32_t getY (uint64_t pos)
{
    uint32_t result = 0;
    //if( pos & UINT64_BIT1) result |= UINT32_BIT0;
    //if( pos & UINT64_BIT3) result |= UINT32_BIT1;
    //if( pos & UINT64_BIT5) result |= UINT32_BIT2;
    if( pos & UINT64_BIT7) result |= UINT32_BIT3;
    if( pos & UINT64_BIT9) result |= UINT32_BIT4;
    if( pos & UINT64_BIT11) result |= UINT32_BIT5;
    if( pos & UINT64_BIT13) result |= UINT32_BIT6;
    if( pos & UINT64_BIT15) result |= UINT32_BIT7;
    if( pos & UINT64_BIT17) result |= UINT32_BIT8;
    if( pos & UINT64_BIT19) result |= UINT32_BIT9;
    if( pos & UINT64_BIT21) result |= UINT32_BIT10;
    if( pos & UINT64_BIT23) result |= UINT32_BIT11;
    if( pos & UINT64_BIT25) result |= UINT32_BIT12;
    if( pos & UINT64_BIT27) result |= UINT32_BIT13;
    if( pos & UINT64_BIT29) result |= UINT32_BIT14;
    if( pos & UINT64_BIT31) result |= UINT32_BIT15;
    if( pos & UINT64_BIT33) result |= UINT32_BIT16;
    if( pos & UINT64_BIT35) result |= UINT32_BIT17;
    if( pos & UINT64_BIT37) result |= UINT32_BIT18;
    if( pos & UINT64_BIT39) result |= UINT32_BIT19;
    if( pos & UINT64_BIT41) result |= UINT32_BIT20;
    if( pos & UINT64_BIT43) result |= UINT32_BIT21;
    if( pos & UINT64_BIT45) result |= UINT32_BIT22;
    if( pos & UINT64_BIT47) result |= UINT32_BIT23;
    if( pos & UINT64_BIT49) result |= UINT32_BIT24;
    if( pos & UINT64_BIT51) result |= UINT32_BIT25;
    if( pos & UINT64_BIT53) result |= UINT32_BIT26;
    if( pos & UINT64_BIT55) result |= UINT32_BIT27;
    if( pos & UINT64_BIT57) result |= UINT32_BIT28;
    if( pos & UINT64_BIT59) result |= UINT32_BIT29;
    if( pos & UINT64_BIT61) result |= UINT32_BIT30;
    if( pos & UINT64_BIT63) result |= UINT32_BIT31;
    return result;
}

uint32_t getXmin(GeoBox& a)
{
    return getX(a.get_pos());
}

uint32_t getYmin(GeoBox& a)
{
    return getY(a.get_pos());
}

uint32_t getXmax(GeoBox& a)
{
    uint64_t pos = a.get_pos();
    uint64_t mask = UINT64_C(0xFFFFFFFFFFFFFFFF);
    mask = mask >> (64 - (a.get_maskLength()));
    pos |= mask;
    return getX(pos);
}

uint32_t getYmax(GeoBox& a)
{
    uint64_t pos = a.get_pos();
    uint64_t mask = UINT64_C(0xFFFFFFFFFFFFFFFF);
    mask = mask >> (64 - (a.get_maskLength()));
    pos |= mask;
    return getY(pos);
}

bool geoBoxContains(GeoBox* a, GeoBox* b)
{
    if(a->get_maskLength() < b->get_maskLength()) return false;
    uint64_t mask = UINT64_C(0xFFFFFFFFFFFFFFFF) << a->get_maskLength();
    if((a->get_pos() & mask) == (b->get_pos() & mask)) return true;
    return false;
}

bool operator<(GeoBox const& a, GeoBox const& b)
{
    return (compareGeoBox(&a, &b) < 0);
}

bool operator>(GeoBox const& a, GeoBox const& b)
{
    return (compareGeoBox(&a, &b) >0);
}

bool operator == (GeoBox const& a, GeoBox const& b)
{
    return (compareGeoBox(&a, &b)  == 0);
}

bool operator>=(GeoBox const& a, GeoBox const& b)
{
    return (compareGeoBox(&a, &b) >=0);
}

bool operator<=(GeoBox const& a, GeoBox const& b)
{
    return (compareGeoBox(&a, &b) <=0);
}

short compareGeoBox(GeoBox const* a, GeoBox const* b)
{
    if(a->get_pos() > b->get_pos()) return 1;
    if(a->get_pos() < b->get_pos()) return -1;
    if(a->get_maskLength() > b->get_maskLength()) return -1;
    if(a->get_maskLength() < b->get_maskLength()) return 1;
    return 0;
}

bool dividex(Rectangle r, Rectangle& left, Rectangle& right)
{
    uint32_t delta = r.x0 ^ r.x1;
    short mask_length = 3;
    //if(delta & UINT32_BIT0) mask_length = 1;
    //if(delta & UINT32_BIT1) mask_length = 2;
    //if(delta & UINT32_BIT2) mask_length = 3;
    if(delta & UINT32_BIT3) mask_length = 4;
    if(delta & UINT32_BIT4) mask_length = 5;
    if(delta & UINT32_BIT5) mask_length = 6;
    if(delta & UINT32_BIT6) mask_length = 7;
    if(delta & UINT32_BIT7) mask_length = 8;
    if(delta & UINT32_BIT8) mask_length = 9;
    if(delta & UINT32_BIT9) mask_length = 10;
    if(delta & UINT32_BIT10) mask_length = 11;
    if(delta & UINT32_BIT11) mask_length = 12;
    if(delta & UINT32_BIT12) mask_length = 13;
    if(delta & UINT32_BIT13) mask_length = 14;
    if(delta & UINT32_BIT14) mask_length = 15;
    if(delta & UINT32_BIT15) mask_length = 16;
    if(delta & UINT32_BIT16) mask_length = 17;
    if(delta & UINT32_BIT17) mask_length = 18;
    if(delta & UINT32_BIT18) mask_length = 19;
    if(delta & UINT32_BIT19) mask_length = 20;
    if(delta & UINT32_BIT20) mask_length = 21;
    if(delta & UINT32_BIT21) mask_length = 22;
    if(delta & UINT32_BIT22) mask_length = 23;
    if(delta & UINT32_BIT23) mask_length = 24;
    if(delta & UINT32_BIT24) mask_length = 25;
    if(delta & UINT32_BIT25) mask_length = 26;
    if(delta & UINT32_BIT26) mask_length = 27;
    if(delta & UINT32_BIT27) mask_length = 28;
    if(delta & UINT32_BIT28) mask_length = 29;
    if(delta & UINT32_BIT29) mask_length = 30;
    if(delta & UINT32_BIT30) mask_length = 31;
    if(delta & UINT32_BIT31) mask_length = 32;

    uint32_t maxx;
    uint32_t minx;
    if(r.x0 > r.x1)
    {
        maxx = r.x0;
        minx = r.x1;
    }
    else if (r.x0 < r.x1)
    {
        maxx = r.x1;
        minx = r.x0;
    }
    else
    {
        return false;
    }
    uint32_t mask;
    mask = UINT32_C(0XFFFFFFFF) << (mask_length - 1);
    uint32_t pivotx =maxx & mask;
    left.x0 = minx;
    left.x1 = pivotx - 1;
    left.y0 = r.y0;
    left.y1 = r.y1;

    right.x0 = pivotx;
    right.x1 = maxx;
    right.y0 = r.y0;
    right.y1 = r.y1;
    return true;
}
bool dividey(Rectangle r, Rectangle& bottom, Rectangle& top)
{
    uint32_t delta = r.y0 ^ r.y1;
    short mask_length = 3;

    //if(delta & UINT32_BIT0) mask_length = 1;
    //if(delta & UINT32_BIT1) mask_length = 2;
    //if(delta & UINT32_BIT2) mask_length = 3;
    if(delta & UINT32_BIT3) mask_length = 4;
    if(delta & UINT32_BIT4) mask_length = 5;
    if(delta & UINT32_BIT5) mask_length = 6;
    if(delta & UINT32_BIT6) mask_length = 7;
    if(delta & UINT32_BIT7) mask_length = 8;
    if(delta & UINT32_BIT8) mask_length = 9;
    if(delta & UINT32_BIT9) mask_length = 10;
    if(delta & UINT32_BIT10) mask_length = 11;
    if(delta & UINT32_BIT11) mask_length = 12;
    if(delta & UINT32_BIT12) mask_length = 13;
    if(delta & UINT32_BIT13) mask_length = 14;
    if(delta & UINT32_BIT14) mask_length = 15;
    if(delta & UINT32_BIT15) mask_length = 16;
    if(delta & UINT32_BIT16) mask_length = 17;
    if(delta & UINT32_BIT17) mask_length = 18;
    if(delta & UINT32_BIT18) mask_length = 19;
    if(delta & UINT32_BIT19) mask_length = 20;
    if(delta & UINT32_BIT20) mask_length = 21;
    if(delta & UINT32_BIT21) mask_length = 22;
    if(delta & UINT32_BIT22) mask_length = 23;
    if(delta & UINT32_BIT23) mask_length = 24;
    if(delta & UINT32_BIT24) mask_length = 25;
    if(delta & UINT32_BIT25) mask_length = 26;
    if(delta & UINT32_BIT26) mask_length = 27;
    if(delta & UINT32_BIT27) mask_length = 28;
    if(delta & UINT32_BIT28) mask_length = 29;
    if(delta & UINT32_BIT29) mask_length = 30;
    if(delta & UINT32_BIT30) mask_length = 31;
    if(delta & UINT32_BIT31) mask_length = 32;

    uint32_t maxy;
    uint32_t miny;
    if(r.y0 > r.y1)
    {
        maxy = r.y0;
        miny = r.y1;
    }
    else if (r.y0 < r.y1)
    {
        maxy = r.y1;
        miny = r.y0;
    }
    else
    {
        return false;
    }
    uint32_t mask;
    mask = UINT32_C(0XFFFFFFFF) << (mask_length - 1);
    uint32_t pivoty =maxy & mask;
    bottom.y0 = miny;
    bottom.y1 = pivoty - 1;
    bottom.x0 = r.x0;
    bottom.x1 = r.x1;

    top.y0 = pivoty;
    top.y1 = maxy;
    top.x0 = r.x0;
    top.x1 = r.x1;
    return true;
}


GeoBox makeGeoBox(Rectangle rect)
{
    GeoBox result;
    result.set_maskLength(0);
    uint32_t minx,maxx,miny,maxy;
    if(rect.x0 < rect.x1)
    {
        minx = rect.x0;
        maxx = rect.x1;
    } else {
        minx = rect.x1;
        maxx = rect.x0;
    }

    if(rect.y0 < rect.y1)
    {
        miny = rect.y0;
        maxy = rect.y1;
    } else {
        miny = rect.y1;
        maxy = rect.y0;
    }

    uint64_t minpos = mergeBits(minx, miny);
    uint64_t maxpos = mergeBits(maxx, maxy);
    uint64_t delta = minpos ^ maxpos;
    short mask_length = 3;
    //if(delta & UINT64_BIT0) mask_length = 1;
    //if(delta & UINT64_BIT1) mask_length = 2;
    //if(delta & UINT64_BIT2) mask_length = 3;
    if(delta & UINT64_BIT3) mask_length = 4;
    if(delta & UINT64_BIT4) mask_length = 5;
    if(delta & UINT64_BIT5) mask_length = 6;
    if(delta & UINT64_BIT6) mask_length = 7;
    if(delta & UINT64_BIT7) mask_length = 8;
    if(delta & UINT64_BIT8) mask_length = 9;
    if(delta & UINT64_BIT9) mask_length = 10;
    if(delta & UINT64_BIT10) mask_length = 11;
    if(delta & UINT64_BIT11) mask_length = 12;
    if(delta & UINT64_BIT12) mask_length = 13;
    if(delta & UINT64_BIT13) mask_length = 14;
    if(delta & UINT64_BIT14) mask_length = 15;
    if(delta & UINT64_BIT15) mask_length = 16;
    if(delta & UINT64_BIT16) mask_length = 17;
    if(delta & UINT64_BIT17) mask_length = 18;
    if(delta & UINT64_BIT18) mask_length = 19;
    if(delta & UINT64_BIT19) mask_length = 20;
    if(delta & UINT64_BIT20) mask_length = 21;
    if(delta & UINT64_BIT21) mask_length = 22;
    if(delta & UINT64_BIT22) mask_length = 23;
    if(delta & UINT64_BIT23) mask_length = 24;
    if(delta & UINT64_BIT24) mask_length = 25;
    if(delta & UINT64_BIT25) mask_length = 26;
    if(delta & UINT64_BIT26) mask_length = 27;
    if(delta & UINT64_BIT27) mask_length = 28;
    if(delta & UINT64_BIT28) mask_length = 29;
    if(delta & UINT64_BIT29) mask_length = 30;
    if(delta & UINT64_BIT30) mask_length = 31;
    if(delta & UINT64_BIT31) mask_length = 32;
    if(delta & UINT64_BIT32) mask_length = 33;
    if(delta & UINT64_BIT33) mask_length = 34;
    if(delta & UINT64_BIT34) mask_length = 35;
    if(delta & UINT64_BIT35) mask_length = 36;
    if(delta & UINT64_BIT36) mask_length = 37;
    if(delta & UINT64_BIT37) mask_length = 38;
    if(delta & UINT64_BIT38) mask_length = 39;
    if(delta & UINT64_BIT39) mask_length = 40;
    if(delta & UINT64_BIT40) mask_length = 41;
    if(delta & UINT64_BIT41) mask_length = 42;
    if(delta & UINT64_BIT42) mask_length = 43;
    if(delta & UINT64_BIT43) mask_length = 44;
    if(delta & UINT64_BIT44) mask_length = 45;
    if(delta & UINT64_BIT45) mask_length = 46;
    if(delta & UINT64_BIT46) mask_length = 47;
    if(delta & UINT64_BIT47) mask_length = 48;
    if(delta & UINT64_BIT48) mask_length = 49;
    if(delta & UINT64_BIT49) mask_length = 50;
    if(delta & UINT64_BIT50) mask_length = 51;
    if(delta & UINT64_BIT51) mask_length = 52;
    if(delta & UINT64_BIT52) mask_length = 53;
    if(delta & UINT64_BIT53) mask_length = 54;
    if(delta & UINT64_BIT54) mask_length = 55;
    if(delta & UINT64_BIT55) mask_length = 56;
    if(delta & UINT64_BIT56) mask_length = 57;
    if(delta & UINT64_BIT57) mask_length = 58;
    if(delta & UINT64_BIT58) mask_length = 59;
    if(delta & UINT64_BIT59) mask_length = 60;
    if(delta & UINT64_BIT60) mask_length = 61;
    if(delta & UINT64_BIT61) mask_length = 62;
    if(delta & UINT64_BIT62) mask_length = 63;
    if(delta & UINT64_BIT63) mask_length = 64;
    uint64_t mask64 = UINT64_C(0XFFFFFFFFFFFFFFFF) << mask_length;
    result.set_pos (minpos & mask64);
    result.set_maskLength (mask_length);
    return result;
}

GeoBox makeGeoBox(uint32_t x1, uint32_t y1)
{
    GeoBox result;
    result.set_maskLength(3);
    result.set_pos(mergeBits(x1, y1));
    return result;
}

Rectangle getRectangle(GeoBox& g)
{
    Rectangle result;
    result.x0 = getXmin(g);
    result.y0 = getYmin(g);
    result.x1 = getXmax(g);
    result.y1 = getYmax(g);
    return result;
}

GeoBoxSet makeGeoBoxSet(Rectangle rect)
{

    GeoBoxSet result;
    result.count = 0;

    /*if(hasgoodMask(rect))
    {
        result.count = 1;
        result.boxes[0] = makeGeoBox(rect);
        return result;
    }*/

    Rectangle a,b,c,d,r[4];
    short rcount = 0;

    if(dividex(rect,a,b))
    {
        if(dividey(a,c,d))
        {
            r[rcount++] = c;
            r[rcount++] = d;
        }
        else
        {
            r[rcount++] = a;
        }
        if(dividey(b,c,d))
        {
            r[rcount++] = c;
            r[rcount++] = d;
        }
        else
        {
            r[rcount++] = b;
        }
    }
    else
    {
        if(dividey(rect,a,b))
        {
            r[rcount++] = a;
            r[rcount++] = b;
        }
        else
        {
            r[rcount++] = rect;
        }
    }

    for(short i = 0; i < rcount; i++)
    {
        GeoBox g = makeGeoBox(r[i]);
        bool alreadyExists = false;
        for(short j = 0; j < result.count; j++)
        {
            if(geoBoxContains(&(result.boxes[j]), &g)) alreadyExists = true;
            else if(geoBoxContains(&g, &(result.boxes[j])))
            {
                result.boxes[j] = g;
                alreadyExists = true;
            }
        }
        if(!alreadyExists) result.boxes[result.count++] = g;
    }
    return result;
}

