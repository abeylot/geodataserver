#include "IdxDetail.hpp"
#include "../common/constants.hpp"

Msg* IdxDetail::processRequest(Msg* request, CompiledDataManager& mger)
{
    std::string resp = "<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body>";
    Msg* rep = new Msg;
    encoder.build200Header(rep, "text/html");
    std::string name  = request->getRecord(2)->getNamedValue("name");
    std::string start  = request->getRecord(2)->getNamedValue("start");
    uint64_t istart = 0;
    if (start != "") istart = atoll(start.c_str());
    if(istart >= 1000)
    {
        resp += "<a href=\"get?name=" +name+ "&start="+std::to_string(istart - 1000)+"\"/> page up </a>\n";
    }
    uint64_t i = istart;
    for(IndexDesc* desc : *(mger.indexes))
    {
        if(desc->name == name )
        {
            fidx::FileIndex<IndexEntry,GeoBox>* idx = desc->idx;
            fidx::Record<IndexEntry, GeoBox> record;
            //GeoIndex p;
            if(desc->type == "relation")
            {
                while (idx->get(i, &record) &&  (i < (istart + 1000)))
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
                    resp += "<a href=\"/relation/get?id=" + std::to_string(r->id) + "\">"+ std::to_string(i) +"</a>:: pos :" + std::to_string(record.key.pos)+ ":: mask :" + std::to_string(record.key.maskLength)+"::"+std::to_string(record.key.zmMask)+":: id :" + std::to_string(record.value.id);
                    resp += "<br/>";
                    i++;
                    delete r;
                }
            }
            else if(desc->type == "node")
            {
                while (idx->get(i, &record) &&  (i < (istart + 1000)))
                {
                    Point* p = mger.loadPoint(record.value.id);
                    std::string name = p->tags["name"];
                    resp += std::to_string(i) + ":: pos :" + std::to_string(record.key.pos)+ "::"+name+":: mask :" + std::to_string(record.key.maskLength)+"::"+std::to_string(record.key.zmMask)+":: id :" + std::to_string(record.value.id);
                    resp += "::" + name +"<br/>";
        
                    uint64_t used = 0;

                    while( used < p->tags.data_size)
                    {

                        char* tag = NULL;
                        char* value = NULL;

                        unsigned char tag_size = 0;
                        unsigned char value_size = 0;

                        tag_size = p->tags.data[used];
                        used++;
                        tag = p->tags.data + used;
                        used += tag_size;
                        value_size =  p->tags.data[used];
                        used++;
                        value = p->tags.data+used;
                        used += value_size;
                        resp += std::string(tag, tag_size);
                        resp += "=";
                        resp += std::string(value, value_size);
                        resp += "<br/>";
                    }
                    i++;
                    delete p;
                }
            }
            else
            {
                while (idx->get(i, &record) &&  (i < (istart + 1000)))
                {
                    Way* r = mger.loadWay(record.value.id);
                    std::string name = r->tags["name"];
                    resp += "name:" + name ;
                    resp += " <a href=\"/way/get?id=" + std::to_string(r->id) + "\">"+ std::to_string(i) +"</a>:: pos :" + std::to_string(record.key.pos)+ ":: mask :" + std::to_string(record.key.maskLength)+"::"+std::to_string(record.key.zmMask)+ ":: id :" + std::to_string(record.value.id);
                    resp +="<br/>";
                    i++;
                }
            }
        }

    }
    if(i == istart + 1000)
    {
        resp += "<a href=\"get?name=" +name+ "&start="+std::to_string(i)+"\"/> page down! </a>\n";
    }
    resp += "</body></html>";
    encoder.addContent(rep,resp);
    return rep;
}
