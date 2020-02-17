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

    if( a & UINT32_BIT0) result |= UINT64_BIT0;
    if( a & UINT32_BIT1) result |= UINT64_BIT2;
    if( a & UINT32_BIT2) result |= UINT64_BIT4;
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

    if( b & UINT32_BIT0) result |= UINT64_BIT1;
    if( b & UINT32_BIT1) result |= UINT64_BIT3;
    if( b & UINT32_BIT2) result |= UINT64_BIT5;
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
    if( pos & UINT64_BIT0) result |= UINT32_BIT0;
    if( pos & UINT64_BIT2) result |= UINT32_BIT1;
    if( pos & UINT64_BIT4) result |= UINT32_BIT2;
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
    if( pos & UINT64_BIT1) result |= UINT32_BIT0;
    if( pos & UINT64_BIT3) result |= UINT32_BIT1;
    if( pos & UINT64_BIT5) result |= UINT32_BIT2;
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
    return getX(a.pos);
}

uint32_t getYmin(GeoBox& a)
{
    return getY(a.pos);
}

uint32_t getXmax(GeoBox& a)
{
    uint64_t pos = a.pos;
    uint64_t mask = UINT64_C(0xFFFFFFFFFFFFFFFF);
    mask = mask >> (64 - (a.maskLength));
    pos |= mask;
    return getX(pos);
}

uint32_t getYmax(GeoBox& a)
{
    uint64_t pos = a.pos;
    uint64_t mask = UINT64_C(0xFFFFFFFFFFFFFFFF);
    mask = mask >> (64 - (a.maskLength));
    pos |= mask;
    return getY(pos);
}

bool geoBoxContains(GeoBox* a, GeoBox* b)
{
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
    if(a->pos > b->pos) return 1;
    if(a->pos < b->pos) return -1;
    if(a->maskLength > b->maskLength) return -1;
    if(a->maskLength < b->maskLength) return 1;
    return 0;
}

GeoBox makeGeoBox(Rectangle rect)
{
    GeoBox result;
    result.maskLength = 0;
    uint32_t szx,szy,sz;
    if(rect.x0 > rect.x1) szx = rect.x0 - rect.x1;
    else szx = rect.x1 - rect.x0;
    if(rect.y0 > rect.y1) szy = rect.y0 - rect.y1;
    else szy = rect.y1 - rect.y0;
    if(szx > szy) sz = szx  ;
    else sz = szy ;

    short pos;
    for(pos = 0; pos < 32; pos++)
    {
        if((sz >> pos) == UINT32_C(0)) break;
    }

    uint64_t mask64 = UINT64_C(0XFFFFFFFFFFFFFFFF) << pos << pos;


    result.maskLength = 2*pos;
    result.pos =  mergeBits(rect.x0,rect.y0) & mask64;
    return result;
}

GeoBox makeGeoBox(uint32_t x1, uint32_t y1)
{
    GeoBox result;
    result.maskLength = 0;
    result.pos = mergeBits(x1, y1);
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
    memset(&result, 0, sizeof(result));
    result.count = 1;
    result.boxes[0] = makeGeoBox (rect);
    Rectangle rect2;
    rect2.x0 = rect.x0;
    rect2.x1 = rect.x1;
    rect2.y0 = rect.y1;
    rect2.y1 = rect.y0;
    GeoBox g = makeGeoBox (rect2);
    if(g.pos != result.boxes[0].pos )
    {
        result.boxes[1] = g;
        result.count++;
    }
    rect2.x0 = rect.x1;
    rect2.x1 = rect.x0;
    rect2.y0 = rect.y0;
    rect2.y1 = rect.y1;
    g = makeGeoBox (rect2);
    if((g.pos != result.boxes[0].pos) && (g.pos!= result.boxes[1].pos))
    {
        result.boxes[result.count] = g;
        result.count++;
    }
    rect2.x0 = rect.x1;
    rect2.x1 = rect.x0;
    rect2.y0 = rect.y1;
    rect2.y1 = rect.y0;
    g = makeGeoBox (rect2);
    if((g.pos != result.boxes[0].pos) && (g.pos!= result.boxes[1].pos)&& (g.pos!= result.boxes[2].pos))
    {
        result.boxes[result.count] = g;
        result.count++;
    }
    return result;
}

