#include "RelationDetail.hpp"
//#include "RelationList.hpp"
#include "ServicesFactory.hpp"
//#include "../datamanager.hpp"
#include "../Coordinates.hpp"
#include "../common/constants.hpp"
#include <string>
#include <math.h>
#include <iostream>

std::string  RelationDetail::printRelation(Relation& r)
{
    std::string res ="";
    if(r.tags.data != NULL)
    {
        uint64_t used = 0;
        char* tag;
        char* value;

        unsigned char tag_size;
        unsigned char value_size;

        while( used < r.tags.data_size)
        {
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

Msg* RelationDetail::processRequest(Msg* request, CompiledDataManager& mger)
{
    //GeoIndex g;
    std::string resp ="";
    uint64_t id = atoll(request->getRecord(2)->getNamedValue("id").c_str());

    Relation* r = mger.loadRelation(id);
    if(r != NULL)
    {
        resp = "<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body style=\"background-color:#AAAAAA; color:#3399CC; padding:20px;\">";
        resp += "<div style=\"position:absolute;top:10px; left:1100px;\">" + printRelation(*r) +"</div>";


        resp += "<object type=\"image/svg+xml\" data=\"../svgMap.svg?longitude1="+ std::to_string(r->rect.x0)
                +"&lattitude1="+ std::to_string(r->rect.y0)
                +"&longitude2="+ std::to_string(r->rect.x1)
                +"&lattitude2="+ std::to_string(r->rect.y1)
                +"\" style=\"width:1000px\"/>";
        resp += "</body></html>";
        Msg* rep = new Msg;
        encoder.build200Header(rep, "text/html");
        encoder.addContent(rep,resp);
        return rep;

    }
    else
    {
        resp = "<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body>Relation wasn't found.</body></html>";
        Msg* rep = new Msg;
        encoder.build200Header(rep, "text/html");
        encoder.addContent(rep,resp);
        return rep;
    }
    //return rep;
}
