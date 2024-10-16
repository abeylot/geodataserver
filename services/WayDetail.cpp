#include "WayDetail.hpp"
#include "../Coordinates.hpp"
#include "../common/constants.hpp"
#include <string>
#include <math.h>
#include <iostream>

std::string WayDetail::printWay(Way& r)
{
    std::string res ="";
    if(r.tags.data != nullptr)
    {
        uint64_t used = 0;

        while( used < r.tags.data_size)
        {

            char* tag = nullptr;
            char* value = nullptr;

            unsigned char tag_size = 0;
            unsigned char value_size = 0;

            tag_size = r.tags.data[used];
            used++;
            tag = r.tags.data + used;
            used += tag_size;
            value_size =  r.tags.data[used];
            used++;
            value = r.tags.data+used;
            used += value_size;
            res += std::string(tag, tag_size);
            res += "=";
            res += std::string(value, value_size);
            res += "<br/>";
        }
    }

    return res;
}




std::shared_ptr<Msg> WayDetail::processRequest(std::shared_ptr<Msg> request, CompiledDataManager& mger)
{
    //GeoIndex g;
    std::string resp ="";
    uint64_t id = atoll(request->getRecord(2)->getNamedValue("id").c_str());

    auto r = mger.loadWay(id);
    if(r != nullptr)
    {
        resp = "<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body style=\"background-color:#AAAAAA; color:#3399CC; padding:20px;\">";
        resp += "<div style=\"position:absolute;top:10px; left:1100px;\">" + printWay(*r) +"</div>";


        resp += "<object type=\"image/svg+xml\" data=\"../svgMap.svg?longitude1="+ std::to_string(r->rect.x0)
                +"&lattitude1="+ std::to_string(r->rect.y0)
                +"&longitude2="+ std::to_string(r->rect.x1)
                +"&lattitude2="+ std::to_string(r->rect.y1)
                +"\" style=\"width:1000px\"/>";
        resp += "</body></html>";
        auto rep = std::make_shared<Msg>();
        encoder.build200Header(rep, "text/html");
        encoder.addContent(rep,resp);
        return rep;

    }
    else
    {
        resp = "<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body>Way wasn't found.</body></html>";
        auto rep = std::make_shared<Msg>();
        encoder.build200Header(rep, "text/html");
        encoder.addContent(rep,resp);
        return rep;
    }
    //return rep;
}
