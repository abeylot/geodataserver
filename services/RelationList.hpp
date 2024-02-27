#ifndef RELATIONLIST_HPP
#define RELATIONLIST_HPP
#include "../messaging/HttpEncoder.hpp"
#include "../messaging/Msg.hpp"
#include "ServiceInterface.hpp"
//#include "ServicesFactory.hpp"

class RelationList : public ServiceInterface
{
private:
    HttpEncoder encoder;
public:
    Msg* processRequest(Msg* request, CompiledDataManager& mger) override;
};
#endif
