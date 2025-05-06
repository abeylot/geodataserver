#ifndef WAYDETAIL_HPP
#define WAYDETAIL_HPP
#include "../messaging/HttpEncoder.hpp"
#include "../messaging/Msg.hpp"
#include "../CompiledDataManager.hpp"
#include "ServiceInterface.hpp"
#include "../GeoBox.hpp"
//#include "ServicesFactory.hpp"

class WayDetail : public ServiceInterface
{

private:
    HttpEncoder encoder;
    std::string printWay(Way& w);
public:
    std::shared_ptr<Msg> processRequest(std::shared_ptr<Msg> request, CompiledDataManager& mger) override;
    const std::string get_name() override { return "WayDetail";}
};
#endif
