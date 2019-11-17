#ifndef WAYDETAIL_HPP
#define WAYDETAIL_HPP
#include "../messaging/HttpEncoder.hpp"
#include "../messaging/Msg.hpp"
#include "../DataManager.hpp"
#include "ServiceInterface.hpp"
#include "../GeoBox.hpp"
//#include "ServicesFactory.hpp"

class WayDetail : public ServiceInterface
{
private:
    struct context
    {
        int level;
        long long nodeCount, wayCount, relationCount;
        double ratio;
        uint32_t maxx,maxy,minx,miny,szx,szy;
        bool isLandArea;
        GeoBoxSet gSet;
        DataManager* mger;
        //HotPoint hotPoint;
    };
private:
    std::string printBalise(Balise* b,context& ctx);
    std::string drawBalise(Balise* b,context& ctx);
    std::string drawWay(Balise* b,context& ctx);
    HttpEncoder encoder;
public:
    Msg* processRequest(Msg* request, DataManager& mger);
};
#endif
