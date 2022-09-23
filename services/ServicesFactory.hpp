#ifndef SERVICESFACTORY_HPP
#define SERVICESFACTORY_HPP
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
class ServicesFactory
{
private:
public:
    static ServiceInterface* getService(std::string service,ParmsXmlVisitor& params);
    static void releaseService(ServiceInterface* service);
};
#endif
