#include "ServicesFactory.hpp"
#include <stdlib.h>

bool ServicesFactory::_enabledPingService{false};
bool ServicesFactory::_enabledRelationListService{false};
bool ServicesFactory::_enabledIdxListService{false};
bool ServicesFactory::_enabledIdxDetailService{false};
bool ServicesFactory::_enabledRelationDetailService{false};
bool ServicesFactory::_enabledWayDetailService{false};
bool ServicesFactory::_enabledSvgService{false};
bool ServicesFactory::_enabledMapDisplayService{false};
bool ServicesFactory::_enabledTileService{false};
bool ServicesFactory::_enabledGeoLocationService{false};
bool ServicesFactory::_enabledRasterImageService{false};

int  ServicesFactory::_cacheLevel;
std::string ServicesFactory::_defaultColor;
std::string ServicesFactory::_locale;
std::vector<PngImage> ServicesFactory::_imageList;

void ServicesFactory::init(const ParmsXmlVisitor& params, const std::vector<PngImage>& imageList )
{
    _imageList = imageList;

    _locale = params.getParam("locale");
    _cacheLevel = params.getNumParam("CacheLevel", 8);
    _defaultColor = params.getParam("DefaultColor");

    _enabledPingService = (params.getParam("PingService") == "enabled");
    if(_enabledPingService) std::cout << "ping service enabled \n";

    _enabledRelationListService = (params.getParam("RelationListService") == "enabled");
    if(_enabledRelationListService) std::cout << "relation list service enabled \n";

    _enabledIdxListService = (params.getParam("IdxListService") == "enabled");
    if(_enabledIdxListService) std::cout << "index list service enabled \n";

    _enabledIdxDetailService = (params.getParam("IdxDetailService") == "enabled");
    if(_enabledIdxDetailService) std::cout << "index detail service enabled \n";

    _enabledRelationDetailService = (params.getParam("RelationDetailService") == "enabled");
    if(_enabledRelationDetailService) std::cout << "relation detail service enabled \n";

    _enabledWayDetailService = (params.getParam("WayDetailService") == "enabled");
    if(_enabledWayDetailService) std::cout << "way detail service enabled \n";

    _enabledSvgService = (params.getParam("SvgService") == "enabled");
    if(_enabledSvgService) std::cout << "svg service enabled \n";

    _enabledMapDisplayService = (params.getParam("MapDisplayService") == "enabled");
     if(_enabledMapDisplayService) std::cout << "map display service enabled \n";

   _enabledTileService = (params.getParam("TileService") == "enabled");
    if(_enabledTileService) std::cout << "tile service enabled \n";

    _enabledGeoLocationService = (params.getParam("GeolocationService") == "enabled");
    if(_enabledGeoLocationService) std::cout << "geolocation service enabled \n";

    _enabledRasterImageService = (params.getParam("RasterImageService") == "enabled");
    if(_enabledGeoLocationService) std::cout << "raster image service enabled \n";

}


std::shared_ptr<ServiceInterface>  ServicesFactory::getService(std::string service)
{
    if(_enabledPingService && service == "/ping")                                        return std::make_shared<Ping>();
    else if(_enabledGeoLocationService && service == "/geoloc")                          return std::make_shared<Geolocation>();
    else if (_enabledRelationListService && service == "/relation/list")                 return std::make_shared<RelationList>();
    else if (_enabledIdxListService && service == "/index/list")                         return std::make_shared<IdxList>();
    else if (_enabledIdxDetailService && service == "/index/get")                        return std::make_shared<IdxDetail>();
    else if (_enabledRelationDetailService && service == "/relation/get")                return std::make_shared<RelationDetail>();
    else if (_enabledWayDetailService && service == "/way/get")                          return std::make_shared<WayDetail>();
    else if (_enabledSvgService && service == "/svgMap.svg")                             return std::make_shared<Svg>();
    else if (_enabledMapDisplayService && ((service == "/MapDisplay")||(service =="/"))) return std::make_shared<MapDisplay>(_enabledRasterImageService);
    else if(_enabledTileService && service.find(".svg") != std::string::npos)
    {
        unsigned int pos = 0;
        const char* c = service.c_str();
        long x=0;
        long y=0;
        long z = atoll(c+1);
        pos=1;
        while(pos < service.length() && (*(c+pos) != '/')) pos++;
        if(pos < service.length()) x =  atoll(c+pos+1);
        pos++;
        while(pos < service.length() && (*(c+pos) != '/')) pos++;
        if(pos < service.length()) y =  atoll(c+pos+1);
        //std::cout << x <<":" << y << ";" << z << "\n";
        return std::make_shared<Tile>(z, x, y, _cacheLevel, _locale, _defaultColor);

    }
    else if(_enabledRasterImageService && service.find(".png") != std::string::npos)
    {
        unsigned int pos = 0;
        const char* c = service.c_str();
        long x=0;
        long y=0;
        long z = atoll(c+1);
        pos=1;
        while(pos < service.length() && (*(c+pos) != '/')) pos++;
        if(pos < service.length()) x =  atoll(c+pos+1);
        pos++;
        while(pos < service.length() && (*(c+pos) != '/')) pos++;
        if(pos < service.length()) y =  atoll(c+pos+1);
        //std::cout << x <<":" << y << ";" << z << "\n";
        return std::make_shared<RasterImage>(z, x, y, _cacheLevel, _imageList);

    }
    else return nullptr;
}


