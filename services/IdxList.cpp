#include "IdxList.hpp"
std::shared_ptr<Msg> IdxList::processRequest([[maybe_unused]] std::shared_ptr<Msg> request, CompiledDataManager& mger)
{
    std::string resp = "<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body>";
    auto rep = std::make_shared<Msg>();
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
