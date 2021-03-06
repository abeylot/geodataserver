#ifndef TILE_HPP
#define TILE_HPP
#include "../messaging/HttpEncoder.hpp"
#include "../messaging/Msg.hpp"
#include "../CompiledDataManager.hpp"
#include "ServiceInterface.hpp"
#include "../GeoBox.hpp"
//#include "ServicesFactory.hpp"

class Tile : public ServiceInterface
{
private:
    long _x, _y, _z;
    unsigned int _cachelevel;
    std::string _locale, _defaultColor;
public:
    Tile(long z, long x, long y, unsigned int cachelevel, std::string locale, std::string defaultColor):_locale(locale)
    {
        _x = x;
        _y = y;
        _z = z;
        _cachelevel = cachelevel;
        _defaultColor = defaultColor;
    }
    Msg* processRequest(Msg* request, CompiledDataManager& mger);
};
#endif
