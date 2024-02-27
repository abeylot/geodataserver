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
    Msg* processRequest(Msg* request, CompiledDataManager& mger) override;
};
#endif
