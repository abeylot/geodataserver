#ifndef SERVICEINTERFACE_HPP
#define SERVICEINTERFACE_HPP
#include "../messaging/Msg.hpp"
#include "../CompiledDataManager.hpp"
class ServiceInterface
{

public:
    virtual Msg* processRequest(Msg* request,CompiledDataManager& mger) = 0;
    virtual ~ServiceInterface() {};
};
#endif
