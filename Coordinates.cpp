#include "Coordinates.hpp"
#include <iostream>
#include <stdlib.h>
uint32_t Coordinates::toNormalizedLon(const std::string& coord)
{
    size_t pos = coord.find(".");
    int signe = 1;
    long long signedInt = atoll(coord.substr(0,pos).c_str());
    if (coord.at(0) == '-') signe = -1;
    uint32_t integerPart = 180 + signedInt;
    std::string sDecimalPart = coord.substr(pos+1);
    while(sDecimalPart.size() < 7) sDecimalPart += '0';
    uint32_t decimalPart = atol(sDecimalPart.c_str());
    uint64_t intCoord;
    if(signe == -1)
        intCoord = (integerPart) * 10000000 - decimalPart;
    else
        intCoord = ( integerPart) * 10000000 + decimalPart;

    uint32_t normalized = 0;
    //if(intCoord == 3600000000ULL) intCoord = 3599999999ULL;
    //if(intCoord == 4500000000ULL) intCoord = 4499999999ULL;
    uint32_t toCompare = 3600000000ULL;
    //uint32_t toCompare = 2250000000ULL;
    normalized = 1;
    for (int i = 0; i < 31; i++ )
    {
        normalized <<= 1;
        if (intCoord > toCompare)
        {
            normalized |= 1;
            intCoord = intCoord - toCompare;
        }
        toCompare >>= 1;
    }
    return normalized;
}


uint32_t Coordinates::toNormalizedLat(const std::string& coord)
{
    size_t pos = coord.find(".");
    int signe = 1;
    long long signedInt = atoll(coord.substr(0,pos).c_str());
    if (coord.at(0) == '-') signe = -1;
    uint32_t integerPart = 90 - signedInt;
    std::string sDecimalPart = coord.substr(pos+1);
    while(sDecimalPart.size() < 7) sDecimalPart += '0';
    uint32_t decimalPart = atol(sDecimalPart.c_str());
    uint32_t intCoord;
    if(signe == 1)
        intCoord = (integerPart) * 10000000 - decimalPart;
    else
        intCoord = (integerPart)* 10000000 + decimalPart;

    uint32_t normalized = 0;
    //if(intCoord == 2700000000ULL) intCoord = 2699999999ULL;
    uint32_t toCompare = 1800000000ULL;
    normalized = 1;
    for (int i = 0; i < 31; i++ )
    {
        normalized <<= 1;
        if (intCoord > toCompare)
        {
            normalized |= 1;
            intCoord = intCoord - toCompare;
        }
        toCompare >>= 1;
    }
    return normalized;
}



std::string Coordinates::toHex(const uint32_t number)
{
    static const char* digits = "0123456789ABCDEF";
    std::string rc(8,'0');
    for (size_t i=0, j=(7)*4 ; i<8; ++i,j-=4)
        rc[i] = digits[(number>>j) & 0x0f];
    return rc;
}

uint32_t Coordinates::fromHex(const std::string& s)
{
    int len = s.size();
    const char* b = s.c_str();
    uint32_t result = 0;
    for(int i = 0; i < len ; i++ )
    {
        result <<= 4;
        switch(b[i])
        {
        case '0':
            break;
        case '1':
            result |= 1;
            break;
        case '2':
            result |= 2;
            break;
        case '3':
            result |= 3;
            break;
        case '4':
            result |= 4;
            break;
        case '5':
            result |= 5;
            break;
        case '6':
            result |= 6;
            break;
        case '7':
            result |= 7;
            break;
        case '8':
            result |= 8;
            break;
        case '9':
            result |= 9;
            break;
        case 'A':
            result |= 10;
            break;
        case 'B':
            result |= 11;
            break;
        case 'C':
            result |= 12;
            break;
        case 'D':
            result |= 13;
            break;
        case 'E':
            result |= 14;
            break;
        case 'F':
            result |= 15;
            break;
        }
    }
    return result;
}

/*int main() {
    uint32_t test = Coordinates::toNormalizedCoords("90.0000000");
    std::cout << test <<"\n";
    test = Coordinates::toNormalizedCoords("45.0000000");
    std::cout << test << "\n" << Coordinates::toHex(test) << "\n" << Coordinates::fromHex(Coordinates::toHex(1234567)) << "\n";
    return 0;
}*/
