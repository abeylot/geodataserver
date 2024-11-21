#include "IdxDetail.hpp"
#include "../common/constants.hpp"

std::shared_ptr<Msg> IdxDetail::processRequest(std::shared_ptr<Msg> request, CompiledDataManager& mger)
{
    std::string resp = "<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body>";
    auto rep = std::make_shared<Msg>();
    encoder.build200Header(rep, "text/html");
    std::string rq_name  = request->getRecord(2)->getNamedValue("name");
    std::string rq_start  = request->getRecord(2)->getNamedValue("start");
    uint64_t istart = 0;
    if (rq_start != "") istart = atoll(rq_start.c_str());
    if(istart >= 1000)
    {
        resp += "<a href=\"get?name=" +rq_name+ "&start="+std::to_string(istart - 1000)+"\"/> page up </a>\n";
    }
    uint64_t i = istart;
    for(auto desc : *(mger.indexes))
    {
        if(desc->name == rq_name )
        {
            fidx::FileIndex<IndexEntryMasked,GeoBox>* idx = desc->idx;
            fidx::Record<IndexEntryMasked, GeoBox> record;
            //GeoIndex p;
            if(desc->type == "relation")
            {
                while (idx->get(i, &record) &&  (i < (istart + 1000)))
                {
                    //GeoIndex p;
                    //mger.relationIndex->get(record.value, &p);
                    auto r = mger.loadRelation(record.value.id);
                    resp += "<a href=\"/relation/get?id=";
                    resp += std::to_string(record.value.id);
                    resp +="\"> ";
                    resp += std::to_string(i);
                    resp +="</a>";
                    std::string_view name = r->tags["name"];
                    std::string_view type = r->tags["type"];
                    resp += "name:" + std::string(name) + " type:" + std::string(type);
                    resp += "<a href=\"/relation/get?id=" + std::to_string(r->id) + "\">"+ std::to_string(i) +"</a>:: pos :" + std::to_string(record.key.get_pos())+ ":: mask :" + std::to_string(record.key.get_maskLength())+"::"+std::to_string(record.value.zmMask)+":: id :" + std::to_string(record.value.id);
                    resp += "<br/>";
                    i++;
                }
            }
            else if(desc->type == "node")
            {
                while (idx->get(i, &record) &&  (i < (istart + 1000)))
                {
                    auto p = mger.loadPoint(record.value.id);
                    std::string_view name = p->tags["name"];
                    resp += std::to_string(i) + ":: pos :" + std::to_string(record.key.get_pos())+ "::"+std::string(name)+":: mask :" + std::to_string(record.key.get_maskLength())+"::"+std::to_string(record.value.zmMask)+":: id :" + std::to_string(record.value.id);
                    resp += "::" + std::string(name) +"<br/>";

                    uint64_t used = 0;

                    while( used < p->tags.data_size)
                    {

                        char* tag = nullptr;
                        char* value = nullptr;

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
                }
            }
            else
            {
                while (idx->get(i, &record) &&  (i < (istart + 1000)))
                {
                    auto r = mger.loadWay(record.value.id);
                    std::string_view name = r->tags["name"];
                    resp += "name:" + std::string(name) ;
                    resp += " <a href=\"/way/get?id=" + std::to_string(r->id) + "\">"+ std::to_string(i) +"</a>:: pos :" + std::to_string(record.key.get_pos())+ ":: mask :" + std::to_string(record.key.get_maskLength())+"::"+std::to_string(record.value.zmMask)+ ":: id :" + std::to_string(record.value.id);
                    resp +="<br/>";
                    i++;
                }
            }
        }

    }
    if(i == istart + 1000)
    {
        resp += "<a href=\"get?name=" +rq_name+ "&start="+std::to_string(i)+"\"/> page down! </a>\n";
    }
    resp += "</body></html>";
    encoder.addContent(rep,resp);
    return rep;
}
