#ifndef PING_HPP
#define PING_HPP
#include "../messaging/HttpEncoder.hpp"
#include "../messaging/Msg.hpp"
#include "ServiceInterface.hpp"
#include "../CompiledDataManager.hpp"
class Ping : public ServiceInterface
{
private:
    HttpEncoder encoder;
public:
    Msg* processRequest(Msg* request, CompiledDataManager& mger) override;
};
#endif
