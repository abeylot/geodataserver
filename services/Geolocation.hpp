#ifndef GEOLOCATION_HPP
#define GEOLOCATION_HPP
#include "../messaging/HttpEncoder.hpp"
#include "../messaging/Msg.hpp"
#include "ServiceInterface.hpp"
#include <list>
//#include "ServicesFactory.hpp"

struct SearchContext
{
    std::vector<std::string> found_words;
    std::vector<std::string> missing_words;
    float score;
    Rectangle area;
};


struct weightedArea
{
	Rectangle r;
	uint64_t score;
	std::list<uint64_t> words;
};

class Geolocation : public ServiceInterface
{
private:
    HttpEncoder encoder;
    std::list<weightedArea> findExpression(std::string expr, CompiledDataManager& mger);
public:
    Msg* processRequest(Msg* request, CompiledDataManager& mger);
};
#endif
