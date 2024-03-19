#ifndef SERVICESFACTORY_HPP
#define SERVICESFACTORY_HPP
#include <string>
#include "../CompiledDataManager.hpp"
#include "Ping.hpp"
#include "RelationList.hpp"
#include "Geolocation.hpp"
#include "RelationDetail.hpp"
#include "WayDetail.hpp"
#include "IdxList.hpp"
#include "IdxDetail.hpp"
#include "Svg.hpp"
#include "Tile.hpp"
#include "MapDisplay.hpp"
#include "Geolocation.hpp"
#include "RasterImage.hpp"
class ServicesFactory
{
private:
static bool _enabledPingService;
static bool _enabledRelationListService;
static bool _enabledIdxListService;
static bool _enabledIdxDetailService;
static bool _enabledRelationDetailService;
static bool _enabledWayDetailService;
static bool _enabledSvgService;
static bool _enabledMapDisplayService;
static bool _enabledTileService;
static bool _enabledGeoLocationService;
static bool _enabledRasterImageService;
static int _cacheLevel;
static std::string _defaultColor;
static std::string _locale;
static std::vector<PngImage> _imageList;


public:
    static void init(const ParmsXmlVisitor& params, const std::vector<PngImage>& imageList);
    static ServiceInterface* getService(std::string service);
    static void releaseService(ServiceInterface* service);
};
#endif
