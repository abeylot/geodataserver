#ifndef GEOLOCATION_HPP
#define GEOLOCATION_HPP
#include "../messaging/HttpEncoder.hpp"
#include "../messaging/Msg.hpp"
#include "ServiceInterface.hpp"
#include <list>
#include <vector>
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
    int64_t score;
    std::string found;
    std::vector<uint64_t> nodes;
    std::vector<uint64_t> ways;
    std::vector<uint64_t> relations;
    GeoPoint pin;
    bool checkIntersect(CompiledDataManager& mger);
};



class Geolocation : public ServiceInterface
{
private:
    HttpEncoder encoder;
    std::list<weightedArea> findExpression(std::string expr, CompiledDataManager& mger);
public:
    std::shared_ptr<Msg> processRequest(std::shared_ptr<Msg> request, CompiledDataManager& mger) override;
    const std::string get_name() override { return "Geolocation";}
};
#endif
