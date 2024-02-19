#include "Ping.hpp"
Msg* Ping::processRequest([[maybe_unused]] Msg* request, [[maybe_unused]] CompiledDataManager& mger)
{
    std::string resp = "<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body>Ping!</body></html>";
    Msg* rep = new Msg;
    encoder.build200Header(rep, "text/html");
    encoder.addContent(rep,resp);
    return rep;
}
