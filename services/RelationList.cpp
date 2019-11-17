#include "RelationList.hpp"
#include "ServicesFactory.hpp"
#include "../common/constants.hpp"
#include <stdlib.h>

Msg* RelationList::processRequest(Msg* request, CompiledDataManager& mger)
{
    //GeoBox geoBox;
    // DataManager mger;
    //bool dump;
    std::string resp = "<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body>";
    //size_t i = 0;
    //GeoIndex p;
    for(uint64_t i=0; i < mger.relationIndex->getSize(); i++)
    {
        Relation* r = mger.loadRelationFast(i);
        //dump = false;
        /*for(Balise* c : b->childs) {
          if(c->baliseName == BALISENAME_TAGSHORT) {
        	  //if((c->keyValues["k"] =="admin_level") && (c->keyValues["v"] == "2")) {
        		  dump = true;
        	  //}

          }
        }*/
        if(true)
        {
            resp += "<a href=\"/relation/get?id=";
            resp += std::to_string(i);
            resp +="\"> ";
            resp += std::to_string(i);
            resp +="</a>";
            /*for(Balise* c : b->childs) {
              if((c->baliseName == BALISENAME_TAGSHORT) &&
              (
              (c->keyValues["k"] == "name") ||
              (c->keyValues["k"] == "type")
              )
              )
              {
            	  resp += c->keyValues["k"] +"="+ c->keyValues["v"]; resp +=" ";
              }
            }*/
            std::string name = r->tags["name"];
            std::string type = r->tags["type"];
            resp += "name:" + name + " type:" + type;
            resp += "<br/>";
        }
        delete r;
        //i++;
    }
    resp += "</body></html>";
    Msg* rep = new Msg;
    encoder.build200Header(rep, "text/html");
    encoder.addContent(rep,resp);
    return rep;
}
