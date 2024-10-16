#include "Ping.hpp"
std::shared_ptr<Msg> Ping::processRequest([[maybe_unused]] std::shared_ptr<Msg> request, [[maybe_unused]] CompiledDataManager& mger)
{
    std::string resp = "<!DOCTYPE html><html> <head>  <meta charset=\"UTF-8\"></head> <body>Ping!</body></html>";
    auto rep = std::make_shared<Msg>();
    encoder.build200Header(rep, "text/html");
    encoder.addContent(rep,resp);
    return rep;
}
