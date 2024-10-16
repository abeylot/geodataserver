#ifndef SERVICEINTERFACE_HPP
#define SERVICEINTERFACE_HPP
#include "../messaging/Msg.hpp"
#include "../CompiledDataManager.hpp"
#include <memory>
class ServiceInterface
{

public:
    virtual std::shared_ptr<Msg> processRequest(std::shared_ptr<Msg> request,CompiledDataManager& mger) = 0;
    virtual ~ServiceInterface() {};
};
#endif
