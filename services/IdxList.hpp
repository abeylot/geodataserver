#ifndef IDXLIST_HPP
#define IDXLIST_HPP
#include "../messaging/HttpEncoder.hpp"
#include "../messaging/Msg.hpp"
#include "ServiceInterface.hpp"
#include "../CompiledDataManager.hpp"
class IdxList : public ServiceInterface
{
private:
    HttpEncoder encoder;
public:
    Msg* processRequest(Msg* request, CompiledDataManager& mger) override;
};
#endif
