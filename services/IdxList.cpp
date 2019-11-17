#include "IdxList.hpp"
Msg* IdxList::processRequest(Msg* request, CompiledDataManager& mger)
{
    std::string resp = "<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body>";
    Msg* rep = new Msg;
    encoder.build200Header(rep, "text/html");
    for(IndexDesc* desc : *(mger.indexes))
    {
        resp += "<a href=\"/index/get?name=";
        resp +=desc->name;
        resp +="\"> ";
        resp +=desc->name+":"+desc->type;
        resp +="</a><br/>";

    }
    resp += "</body></html>";
    encoder.addContent(rep,resp);
    return rep;
}
