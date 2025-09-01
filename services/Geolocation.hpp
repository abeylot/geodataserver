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
    std::string _locale;
    char _locales[32][2];
    unsigned char _nb_locales = 0;
    std::list<weightedArea> findExpression(std::string expr, CompiledDataManager& mger);
    template <class ITEM> int64_t calcMatchScore(const ITEM& item, const std::vector<uint64_t>& searched_words, CompiledDataManager& mger);
public:
    Geolocation(const std::string locale):_locale(locale){
        while(3*_nb_locales < _locale.size())
        {
            _locales[_nb_locales][0] = _locale[3*_nb_locales];
            _locales[_nb_locales][1] = _locale[3*_nb_locales + 1];
            _nb_locales ++ ;
        }
    };
    std::shared_ptr<Msg> processRequest(std::shared_ptr<Msg> request, CompiledDataManager& mger) override;
    const std::string get_name() override { return "Geolocation";}
};
#endif
