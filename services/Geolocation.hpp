#ifndef GEOLOCATION_HPP
#define GEOLOCATION_HPP
#include "../messaging/HttpEncoder.hpp"
#include "../messaging/Msg.hpp"
#include "ServiceInterface.hpp"
//#include "ServicesFactory.hpp"

struct SearchContext
{
    std::vector<std::string> found_words;
    std::vector<std::string> missing_words;
    float score;
    Rectangle area;
};




class Geolocation : public ServiceInterface
{
private:
    HttpEncoder encoder;
public:
    Msg* processRequest(Msg* request, CompiledDataManager& mger);
};
#endif
