#ifndef SVG_HPP
#define SVG_HPP
#include "../messaging/HttpEncoder.hpp"
#include "../messaging/Msg.hpp"
#include "../CompiledDataManager.hpp"
#include "ServiceInterface.hpp"
#include "../GeoBox.hpp"
//#include "ServicesFactory.hpp"

class Svg : public ServiceInterface
{
public:
    std::shared_ptr<Msg> processRequest(std::shared_ptr<Msg> request, CompiledDataManager& mger) override;
};
#endif
