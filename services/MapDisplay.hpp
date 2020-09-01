#include "ServiceInterface.hpp"
#include "../CompiledDataManager.hpp"
#include "../messaging/HttpEncoder.hpp"

#ifndef MAPDISPLAY_HPP
#define MAPDISPLAY_HPP


class MapDisplay : public ServiceInterface
{
private:
    HttpEncoder encoder;    
    
public:
    MapDisplay();
    ~MapDisplay();
    Msg* processRequest(Msg* request, CompiledDataManager& mger);

};

#endif // MAPDISPLAY_HPP
