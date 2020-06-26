#include "ServicesFactory.hpp"
#include <stdlib.h>
ServiceInterface* ServicesFactory::getService(std::string service, ParmsXmlVisitor& params)
{
    if(service == "/ping")                return new Ping;
    else if (service == "/relation/list") return new RelationList;
    else if (service == "/index/list")    return new IdxList;
    else if (service == "/index/get")     return new IdxDetail;
    else if (service == "/relation/get")  return new RelationDetail;
    else if (service == "/way/get")       return new WayDetail;
    else if (service == "/svgMap.svg")        return new Svg;
    else if(service.find(".svg") != std::string::npos)
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
        std::cout << x <<":" << y << ";" << z << "\n";
        return new Tile(z, x, y, params.getNumParam("CacheLevel", 8), params.getParam("locale"));

    }
    else return NULL;
}

void ServicesFactory::releaseService(ServiceInterface* service)
{
    if(service != NULL) delete service;
}

