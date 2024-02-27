#ifndef RELATIONDETAIL_HPP
#define RELATIONDETAIL_HPP
#include "../messaging/HttpEncoder.hpp"
#include "../messaging/Msg.hpp"
#include "../CompiledDataManager.hpp"
#include "ServiceInterface.hpp"
//#include "../GeoBox.hpp"
//#include "ServicesFactory.hpp"

class RelationDetail : public ServiceInterface
{
private:
    HttpEncoder encoder;
    std::string printRelation(Relation& r);
public:
    Msg* processRequest(Msg* request, CompiledDataManager& mger) override;
};
#endif
