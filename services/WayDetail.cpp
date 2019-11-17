#include "WayDetail.hpp"
//#include "RelationList.hpp"
//#include "ServicesFactory.hpp"
//#include "../datamanager.hpp"
#include "../Coordinates.hpp"
#include "../common/constants.hpp"
#include <string>
#include <math.h>
#include <iostream>

std::string WayDetail::printBalise(Balise* b, WayDetail::context& ctx)
{

    std::string result = "";
    ctx.level++;
    if(b->baliseName == BALISENAME_NODESHORT)
    {
        //uint32_t x = Coordinates::fromHex(b->keyValues["lon"]);
        //uint32_t y = Coordinates::fromHex(b->keyValues["lat"]);
        uint32_t x = b->rect.x0;
        uint32_t y = b->rect.y0;
        if (x > ctx.maxx) ctx.maxx = x;
        if (x < ctx.minx) ctx.minx = x;
        if (y > ctx.maxy) ctx.maxy = y;
        if (y < ctx.miny) ctx.miny = y;
        ctx.nodeCount++;
    }
    else if (b->baliseName == BALISENAME_WAYSHORT)
    {
        ctx.wayCount++;
    }
    else if (b->baliseName == BALISENAME_RELATIONSHORT)
    {
        ctx.relationCount++;
    }
    else if ((ctx.level == 2) && (b->baliseName == BALISENAME_TAGSHORT))
    {
        result += b->keyValues["k"] + ":" + b->keyValues["v"] + "<br/>";
    }
    for(Balise* c : b->childs)
    {
        result += printBalise(c, ctx);
    }
    ctx.level--;
    if(ctx.level == 0)
    {

        //ctx.gSet = makeGeoBoxSet(ctx.minx, ctx.miny, ctx.maxx,ctx.maxy );

        uint32_t meany = (ctx.miny >> 1) + (ctx.maxy  >> 1);
        double angle = PI * ((meany *1.0) / (1.0 * UINT32_MAX));
        std::cout << "Angle : " << angle << " rad\n";
        ctx.ratio = 	( 2.0* sin(angle)*(ctx.maxx - ctx.minx))/(1.0*(ctx.maxy - ctx.miny));
        if(ctx.ratio < 0 ) ctx.ratio *= -1;
        if(ctx.ratio > 1)
        {
            ctx.szx = 1000;
            ctx.szy = 1000.0 / ctx.ratio;
        }
        else
        {
            ctx.szy = 1000;
            ctx.szx = 1000.0 * ctx.ratio;
        }
        result += " nodes : " + std::to_string(ctx.nodeCount) +" ways : " + std::to_string(ctx.wayCount) +" relations : " + std::to_string(ctx.relationCount) + "<br/>";
        result += " closed : " + std::to_string(b->isClosed) + "<br/>";
    }
    return result;
}



std::string WayDetail::drawWay(Balise* b, WayDetail::context& ctx)
{
    std::string result = "";
    std::string color="grey";
    result += "<polyline points=\"";
    int oldx = -1;
    int oldy = -1;
    for(Balise* c : b->childs)
    {
        if(c->baliseName == BALISENAME_TAGSHORT)
        {
            if((c->keyValues["k"] == "natural")
                    && (
                        (c->keyValues["v"] == "coastline")||
                        (c->keyValues["v"] == "water")
                    ))
                color="lightblue";
            else if(c->keyValues["k"] == "admin_level")
            {
                if (c->keyValues["v"] == "2") color = "red";
                else if (c->keyValues["v"] == "3") color = "red";
                else if (c->keyValues["v"] == "4") color = "#AAAAAA";
                else if (c->keyValues["v"] == "5") color = "#AA9999";
                else if (c->keyValues["v"] == "6") color = "#889988";
                else if (c->keyValues["v"] == "7") color = "#AA7777";
                else if (c->keyValues["v"] == "8") color = "#666AA6";
                else if (c->keyValues["v"] == "9") color = "#55BB55";
            }
        }
    }
    for(Balise* c : b->childs)
    {
        if(c->baliseName == BALISENAME_NODESHORT)
        {
            //int x = (Coordinates::fromHex(c->keyValues["lon"]) - ctx.minx)*(ctx.szx*1.0) /(1.0*(ctx.maxx -ctx.minx));
            //int y = (Coordinates::fromHex(c->keyValues["lat"]) - ctx.miny)*(ctx.szy*1.0) /(1.0*(ctx.maxy -ctx.miny));
            int x = (c->rect.x0 - ctx.minx)*(ctx.szx*1.0) /(1.0*(ctx.maxx -ctx.minx));
            int y = (c->rect.y0 - ctx.miny)*(ctx.szy*1.0) /(1.0*(ctx.maxy -ctx.miny));
            if((x != oldx) || (y != oldy))
                result += std::to_string(x) + "," + std::to_string(y) + " ";
            oldx=x;
            oldy=y;
        }

    }
    result += " \" style=\"fill:none;stroke:"+color+";stroke-width:3\" />";
    return result;
}


std::string WayDetail::drawBalise(Balise* b, WayDetail::context& ctx)
{

    std::string result = "";
    if(ctx.level == 0)
    {
        for(short i = 0; i < ctx.gSet.count; i++)
        {
            std::cout << ctx.gSet.boxes[i].pos << ":-:" << std::to_string(ctx.gSet.boxes[i].maskLength) << "\n";
        }
    }
    ctx.level++;
    if (b->baliseName == BALISENAME_WAYSHORT)
    {
        result +=  drawWay(b,ctx);
    }

    ctx.level--;

    return result;
}

Msg* WayDetail::processRequest(Msg* request, DataManager& mger)
{
    context ctx;
    ctx.level = ctx.nodeCount =ctx.wayCount = ctx.relationCount = 0;
    ctx.maxx = ctx.maxy = 0;
    ctx.minx = ctx.miny = UINT32_MAX;
    ctx.mger = &mger;
    std::string resp;
    GeoWayIndex g;
    uint64_t res;
    uint64_t id = atoll(request->getRecord(2)->getNamedValue("id").c_str());

    if(mger.wayIndex->get(id, &g))
    {
        Balise* b = mger.loadBaliseFull(g.start, g.size);
        resp = "<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body style=\"background-color:#AAAAAA; color:#3399CC; padding:20px;\">";
        resp += "<div style=\"position:absolute;top:10px; left:1100px;\">" + printBalise(b,ctx) +"</div>";


        resp += "<svg height=\""+std::to_string(ctx.szy)+"\" width=\""+std::to_string(ctx.szx)+"\"  >";
        resp += drawBalise(b,ctx);
        resp +="</svg>";
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
