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
    std::shared_ptr<Msg> processRequest(std::shared_ptr<Msg> request, CompiledDataManager& mger) override;
};
#endif
