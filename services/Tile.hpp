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
public:
    Tile(long z, long x, long y, unsigned int cachelevel)
    {
        _x = x;
        _y = y;
        _z = z;
        _cachelevel = cachelevel;
    }
    Msg* processRequest(Msg* request, CompiledDataManager& mger);
};
#endif
