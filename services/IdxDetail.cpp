#include "IdxDetail.hpp"
#include "../common/constants.hpp"

Msg* IdxDetail::processRequest(Msg* request, CompiledDataManager& mger)
{
    std::string resp = "<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body>";
    Msg* rep = new Msg;
    encoder.build200Header(rep, "text/html");
    std::string name  = request->getRecord(2)->getNamedValue("name");
    for(IndexDesc* desc : *(mger.indexes))
    {
        if(desc->name == name )
        {
            fidx::FileIndex<IndexEntry,GeoBox>* idx = desc->idx;
            fidx::Record<IndexEntry, GeoBox> record;
            uint64_t i = 0;
            //GeoIndex p;
            if(desc->type == "relation")
            {
                while (idx->get(i, &record))
                {
                    //GeoIndex p;
                    //mger.relationIndex->get(record.value, &p);
                    Relation* r = mger.loadRelation(record.value.id);
                    resp += "<a href=\"/relation/get?id=";
                    resp += std::to_string(record.value.id);
                    resp +="\"> ";
                    resp += std::to_string(i);
                    resp +="</a>";
                    std::string name = r->tags["name"];
                    std::string type = r->tags["type"];
                    resp += "name:" + name + " type:" + type;
                    resp += std::to_string(i) + ":: pos :" + std::to_string(record.key.pos)+ ":: mask :" + std::to_string(record.key.maskLength)+"::"+std::to_string(record.key.zmMask)+":: id :" + std::to_string(record.value.id);
                    resp += "<br/>";
                    i++;
                    delete r;
                }
            }
            else if(desc->type == "node")
            {
                while (idx->get(i, &record))
                {
                    Point* p = mger.loadPoint(record.value.id);
                    resp += std::to_string(i) + ":: pos :" + std::to_string(record.key.pos)+ ":: mask :" + std::to_string(record.key.maskLength)+"::"+std::to_string(record.key.zmMask)+":: id :" + std::to_string(record.value.id);
                    resp += "<br/>";
                    /*for(std::pair<std::string, std::string> pair : p->tags)
                    {
                        resp += pair.first + ":" + pair.second + "<br/>";
                    }*/
                    i++;
                    delete p;
                }
            }
            else
            {
                while (idx->get(i, &record))
                {
                    resp += std::to_string(i) + ":: pos :" + std::to_string(record.key.pos)+ ":: mask :" + std::to_string(record.key.maskLength)+"::"+std::to_string(record.key.zmMask)+ ":: id :" + std::to_string(record.value.id);
                    resp +="<br/>";
                    i++;
                }
            }
        }

    }
    resp += "</body></html>";
    encoder.addContent(rep,resp);
    return rep;
}
