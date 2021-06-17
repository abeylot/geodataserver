#include "Svg.hpp"
#include "renderers/SvgRenderer.hpp"
#include "../Coordinates.hpp"
#include "../common/constants.hpp"
#include <string>
#include <math.h>
#include <iostream>
//#include "renderers/ClcArea.hpp"


Msg* Svg::processRequest(Msg* request, CompiledDataManager& mger)
{
    std::string sLon1 = request->getRecord(2)->getNamedValue("longitude1");
    std::string sLat1 = request->getRecord(2)->getNamedValue("lattitude1");

    std::string sLon2 = request->getRecord(2)->getNamedValue("longitude2");
    std::string sLat2 = request->getRecord(2)->getNamedValue("lattitude2");

    Rectangle rect;

    rect.x0 = atoll(sLon1.c_str());
    rect.y0 = atoll(sLat1.c_str());

    rect.x1 = atoll(sLon2.c_str());
    rect.y1 = atoll(sLat2.c_str());

    uint32_t meany = (rect.y0 >> 1) + (rect.y1  >> 1);
    double angle = PI * ((meany *1.0) / (1.0 * UINT32_MAX)) - PI/2;
    double ratio = 	(sin(angle)*(rect.x1 - rect.x0))/(1.0*(rect.y1 - rect.y0));
    if (ratio < 0) ratio *= -1;

    uint32_t szx, szy;

    if(ratio > 1)
    {
        szx = 2000;
        szy = 2000.0 / (ratio*0.5);
    }
    else
    {
        szy = 2000;
        szx = 2000.0 * ratio*0.5;
    }


    //std::string resp = "";

    Msg* rep = new Msg;
    HttpEncoder encoder;
    encoder.build200Header(rep, "image/svg+xml");
    SvgRenderer rdr(&mger,std::string(""),"#888888");

    std::string tag ="";
    for(std::string val : (request->getRecord(2)->getBlocks()))
    {
        tag += "["+val+"]";
    }
    encoder.addContent(rep,rdr.renderItems(rect,szx,szy,tag));
    return rep;

}
