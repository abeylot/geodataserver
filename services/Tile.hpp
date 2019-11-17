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
public:
    Tile(long z, long x, long y)
    {
        _x = x;
        _y = y;
        _z = z;
    }
    Msg* processRequest(Msg* request, CompiledDataManager& mger);
};
#endif
