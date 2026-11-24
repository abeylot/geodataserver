#ifndef TILE_HPP
#define TILE_HPP
#include "../messaging/HttpEncoder.hpp"
#include "../messaging/Msg.hpp"
#include "../CompiledDataManager.hpp"
#include "ServiceInterface.hpp"
#include "../GeoBox.hpp"
//#include "ServicesFactory.hpp"

enum class TileFormat { Svg, Png };

class Tile : public ServiceInterface
{
private:
    long _x, _y, _z;
    unsigned int _cachelevel;
    std::string _locale, _defaultColor;
    TileFormat _format;
public:
    Tile(long z, long x, long y, unsigned int cachelevel, const std::string& locale, const std::string& defaultColor,
         TileFormat format = TileFormat::Svg):_locale(locale),
    _defaultColor(defaultColor), _format(format)
    {
        _x = x;
        _y = y;
        _z = z;
        _cachelevel = cachelevel;
    }
    std::shared_ptr<Msg> processRequest(std::shared_ptr<Msg> request, CompiledDataManager& mger) override;
    const std::string get_name() override { return "Tile";}
};
#endif
