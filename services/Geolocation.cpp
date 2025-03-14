#include "Geolocation.hpp"
#include "../Coordinates.hpp"
#include "ServicesFactory.hpp"
#include "../common/constants.hpp"
#include <stdlib.h>
#include <algorithm>

const uint64_t UNDEFINED_ID = 0xFFFFFFFFFFFFFFFF;
const int64_t  WORST_SCORE  = -999999999;
const int64_t  MAX_RESULTS  = 50;

int64_t calcMatchScore(std::vector<uint64_t> a, std::vector<uint64_t> b)
{
    int64_t result = 0;
    std::vector<uint64_t> new_a, new_b;
    for(uint64_t i = 0; i < a.size(); i++)
    {
        uint64_t counta = 0;
        uint64_t countb = 0;
        for(uint64_t j = 0; j < a.size(); j++)
        {
            if(a[j] == a[i]) counta++;
        }
        for(uint64_t j = 0; j < b.size(); j++)
        {
            if(b[j] == a[i]) countb++;
        }

        while(counta > countb)
        {
            for(uint64_t j = 0; j < a.size(); j++)
            {
                if(a[i] == a[j])
                {
                    a[j] = UNDEFINED_ID;
                    counta--;
                    result -= 40;
                    break;
                }
            }
        }
        while(countb > counta)
        {
            for(uint64_t j = 0; j < b.size(); j++)
            {
                if(a[i] == b[j])
                {
                    b[j] = UNDEFINED_ID;
                    countb--;
                    result -= 40;
                    break;
                }
            }
        }

    }

    std::copy_if(a.begin(), a.end(), std::back_inserter(new_a), [](uint64_t i){return i != UNDEFINED_ID;});
    std::copy_if(b.begin(), b.end(), std::back_inserter(new_b), [](uint64_t i){return i != UNDEFINED_ID;});
    a = std::move(new_a);
    b = std::move(new_b);
    new_a.clear(); //just to be sure
    new_b.clear();

    for(uint64_t i = 0; i < b.size(); i++)
    {
        uint64_t counta = 0;
        uint64_t countb = 0;
        for(uint64_t j = 0; j < a.size(); j++)
        {
            if(b[i] == a[j]) counta++;
        }
        for(uint64_t j = 0; j < b.size(); j++)
        {
            if(b[j] == b[i]) countb++;
        }

        while(counta > countb)
        {
            for(uint64_t j = 0; j < a.size(); j++)
            {
                if(b[i] == a[j])
                {
                    a[j] = UNDEFINED_ID;
                    counta--;
                    result -= 40;
                    break;
                }
            }
        }
        while(countb > counta)
        {
            for(uint64_t j = 0; j < b.size(); j++)
            {
                if(b[i] == b[j])
                {
                    b[j] = UNDEFINED_ID;
                    countb--;
                    result -= 40;
                    break;
                }
            }
        }

    }

    std::copy_if(a.begin(), a.end(), std::back_inserter(new_a), [](uint64_t i){return i != UNDEFINED_ID;});
    std::copy_if(b.begin(), b.end(), std::back_inserter(new_b), [](uint64_t i){return i != UNDEFINED_ID;});
    a = std::move(new_a);
    b = std::move(new_b);
    new_a.clear();
    new_b.clear();

    while(a.size())
    {
        for(uint64_t i = 0; i < b.size(); i++)
        {
            if(a[0] == b[i])
            {
                if(i!=0) result += 300;
                else result += 500;
                a[0] = b[i] = UNDEFINED_ID;
                break;
            }
        }
        std::copy_if(a.begin(), a.end(), std::back_inserter(new_a), [](uint64_t i){return i != UNDEFINED_ID;});
        std::copy_if(b.begin(), b.end(), std::back_inserter(new_b), [](uint64_t i){return i != UNDEFINED_ID;});
        a = std::move(new_a);
        b = std::move(new_b);
        new_a.clear();
        new_b.clear();
    }
    return result;
}

struct textSearchIds
{
    std::string sWord;
    uint64_t word;

    uint64_t node_start_id;
    uint64_t node_stop_id;

    uint64_t way_start_id;
    uint64_t way_stop_id;

    uint64_t relation_start_id;
    uint64_t relation_stop_id;

    textSearchIds()
    {
        word = 0;
        node_start_id = UNDEFINED_ID;
        node_stop_id  = UNDEFINED_ID;

        way_start_id = UNDEFINED_ID;
        way_stop_id  = UNDEFINED_ID;

        relation_start_id = UNDEFINED_ID;
        relation_stop_id  = UNDEFINED_ID;
    }

};

inline bool operator < (textSearchIds const& a, textSearchIds const& b)
{
    uint64_t count_a, count_b;
    count_a = count_b = 0;

    if(a.node_start_id != UNDEFINED_ID)     count_a += 1 + a.node_stop_id     - a.node_start_id;
    if(a.way_start_id != UNDEFINED_ID)      count_a += 1 + a.way_stop_id      - a.way_start_id;
    if(a.relation_start_id != UNDEFINED_ID) count_a += 1 + a.relation_stop_id - a.relation_start_id;

    if(b.node_start_id != UNDEFINED_ID)     count_b += 1 + b.node_stop_id     - b.node_start_id;
    if(b.way_start_id != UNDEFINED_ID)      count_b += 1 + b.way_stop_id      - b.way_start_id;
    if(b.relation_start_id != UNDEFINED_ID) count_b += 1 + b.relation_stop_id - b.relation_start_id;

    return count_a < count_b;
}


bool compare_weight (const weightedArea& first, const weightedArea& second)
{
  return ( first.score > second.score || ((first.score == second.score )&&(first.r.area() > second.r.area())));
}


template <class ITEM> int64_t calcMatchScore(const ITEM& item, const std::vector<uint64_t>& searched_words, CompiledDataManager& mger)
{
    std::vector<uint64_t> nameWordVector;
    std::string my_string = std::string(item->tags["name"]);
    std::replace( my_string.begin(), my_string.end(), '-', ' ');
    std::stringstream my_stream(my_string);
    std::string word;
    while(std::getline(my_stream,word,' '))
    {
        size_t found = word.find("&apos;");
        while(found != std::string::npos)
        {
            std::string word2 = word.substr(0,found) + "'" + word.substr(found + 6,word.length());
            word = word2;
            found = word.find("&apos;");
        }
        uint64_t k = fidx::makeLexicalKey(word.c_str(), word.length(), *mger.charconvs);
        nameWordVector.push_back(k);
    }
    return calcMatchScore(searched_words, nameWordVector);
}

std::list<weightedArea> Geolocation::findExpression(std::string expr, CompiledDataManager& mger)
{
    std::cout << "Search : [" << expr << "]\n";
    std::stringstream my_strm(expr);

    std::list<textSearchIds> foundWords;
    std::vector<uint64_t> queryWordsVector;
    std::list<uint64_t> words;
    std::list<uint64_t> wordsToMatch;
    std::string wrd;
    while(std::getline(my_strm,wrd,' '))
    {
        if(!wrd.empty())
        {
            uint64_t k = fidx::makeLexicalKey(wrd.c_str(), wrd.length(), *mger.charconvs);
            words.push_back(k);
            queryWordsVector.push_back(k);
         }
    }

    words.sort();
    words.unique();


    for(auto myword : words)
    {
        IndexRange eN;
        IndexRange eW;
        IndexRange eR;



        bool foundN = mger.textIndexNodeRange->find(myword, &eN);
        bool foundW = mger.textIndexWayRange->find(myword, &eW);
        bool foundR = mger.textIndexRelationRange->find(myword, &eR);

        uint64_t nstart, nstop, wstart, wstop, rstart, rstop;
        nstart = nstop = wstart = wstop = rstart = rstop = UNDEFINED_ID;

        if (foundN)
        {
            nstart = eN.first;
            nstop  = eN.last;
        }
        if (foundW)
        {
            wstart = eW.first;
            wstop  = eW.last;
        }
        if (foundR)
        {
            rstart = eR.first;
            rstop  = eR.last;
        }

        std::cout << (nstop - nstart) << "," <<  (wstop - wstart) << "," << (rstop -rstart) << "\n";

        if((foundN || foundW || foundR) &&
        ((nstop - nstart) + (wstop - wstart) + (rstop -rstart)) < 100000
         )
        {
            textSearchIds ids;
            ids.word = myword;
            ids.node_start_id = nstart;
            ids.node_stop_id  = nstop;
            ids.way_start_id  = wstart;
            ids.way_stop_id   = wstop;
            ids.relation_start_id = rstart;
            ids.relation_stop_id = rstop;

            foundWords.push_back(ids);
            wordsToMatch.push_back(myword);
        }
    }
    foundWords.sort();


    std::list<weightedArea> areas;
    std::list<weightedArea> best_areas;

    for(auto searchIds : foundWords)
    {
        areas.clear();
        {
            if (searchIds.node_start_id != UNDEFINED_ID)
            {
                IndexEntry* indexEntry = new IndexEntry[1 + searchIds.node_stop_id - searchIds.node_start_id];
                if( mger.textIndexNode->getItems(searchIds.node_start_id, searchIds.node_stop_id, indexEntry))
                for(uint64_t i = 0; i <= searchIds.node_stop_id - searchIds.node_start_id; i++)
                {
                    weightedArea area;
                    area.pin.x = area.pin.y = 0;
                    std::shared_ptr<Point> item;
                    item = mger.load<Point>(indexEntry[i].id);
                    area.r.x0 = area.r.x1 = item->x;
                    area.r.y0 = area.r.y1 = item->y;

                    // make real area not point.
                    area.r.x0 -= 1;
                    area.r.x1 += 1;
                    area.r.y0 -=1;
                    area.r.y1 +=1;

                    if(area.r.isValid()) area.score = calcMatchScore(item, queryWordsVector, mger);
                    else area.score = WORST_SCORE;
                    std::string_view sPlace = item->tags["admin_level"];
                    if(!sPlace.empty())
                    {
                        int level = 10 - atoiSW(sPlace);
                        if(level) area.score *= (1.0 + 0.1 / level);
                    }
                    area.found = "Node_"+std::to_string(indexEntry[i].id)+":"+ std::string(item->tags["name"]);
                    areas.push_back(area);
                    area.nodes.push_back(indexEntry[i].id);
                }
                delete[] indexEntry;
            }
            if (searchIds.relation_start_id != UNDEFINED_ID)
            {
                IndexEntry* indexEntry = new IndexEntry[1 + searchIds.relation_stop_id - searchIds.relation_start_id];
                if( mger.textIndexRelation->getItems(searchIds.relation_start_id, searchIds.relation_stop_id, indexEntry))
                for(uint64_t i = 0; i <= searchIds.relation_stop_id - searchIds.relation_start_id; i++)
                {
                    weightedArea area;
                    area.pin.x = area.pin.y = 0;
                    std::shared_ptr<Relation> item;
                    item = mger.loadRelationFast(indexEntry[i].id);
                    area.r =  indexEntry[i].r;
                    if(area.r.isValid()) area.score = calcMatchScore(item, queryWordsVector, mger);
                    else area.score = WORST_SCORE;

                    std::string_view sPlace = item->tags["admin_level"];
                    if(!sPlace.empty())
                    {
                        int level = 10 - atoiSW(sPlace);
                        if(level) area.score *= (1.0 + 0.1 / level);
                    }
                    area.found = "Relation_"+std::to_string(indexEntry[i].id)+":" + std::string(item->tags["name"]);
                    area.relations.push_back(indexEntry[i].id);

                    areas.push_back(area);
                }
                delete[] indexEntry;
            }
            if (searchIds.way_start_id != UNDEFINED_ID)
            {
                IndexEntry* indexEntry = new IndexEntry[1 + searchIds.way_stop_id - searchIds.way_start_id];
                if( mger.textIndexWay->getItems(searchIds.way_start_id, searchIds.way_stop_id, indexEntry))
                for(uint64_t i = 0; i <= searchIds.way_stop_id - searchIds.way_start_id; i++)
                {
                    weightedArea area;
                    area.pin.x = area.pin.y = 0;
                    std::shared_ptr<Way> item;
                    item = mger.load<Way>(indexEntry[i].id);
                    area.r =  indexEntry[i].r;
                    if(area.r.isValid()) area.score = calcMatchScore(item, queryWordsVector, mger);
                    else area.score = WORST_SCORE;
                    std::string_view sPlace = item->tags["admin_level"];
                    if(!sPlace.empty())
                    {
                        int level = 10 - atoiSW(sPlace);
                        if(level) area.score *= (1.0 + 0.1 / level);
                    }
                    area.found = "Way_"+std::to_string(indexEntry[i].id)+":" + std::string(item->tags["name"]);
                    areas.push_back(area);
                    area.ways.push_back(indexEntry[i].id);
                }
                delete[] indexEntry;
            }
        }
        if(best_areas.empty())
        {
            areas.sort(compare_weight);
            unsigned int i = 0;
            for(auto a : areas)
            {
                i++;
                best_areas.push_back(a);
                if (i >= 100000)  break;
            }
            break;
        }

    }
    return best_areas;
}


void fillPin(CompiledDataManager& mger, weightedArea& a, int street_number)
{
    for( auto relid : a.relations)
    {
        std::shared_ptr<Relation> item;
        item = mger.load<Relation>(relid);
        GeoPoint pin = {0,0};

        std::string_view sType = item->tags["type"];
        if(street_number && sType == "street")
        {
            for(auto p : item->points)
            {
                std::string_view sNr = p->tags["addr:housenumber"];
                if(atoiSW(sNr) == street_number)
                {
                    a.r.x0 = p->x;
                    a.r.x1 = p->x;
                    a.r.y0 = p->y;
                    a.r.y1 = p->y;
                }
            }
            for(auto w : item->ways)
            {
                std::string_view sNr = w->tags["addr:housenumber"];
                if(atoiSW(sNr) == street_number)
                {
                    a.r = w->rect;
                }
            }
            for(auto r : item->relations)
            {
                std::string_view sNr = r->tags["addr:housenumber"];
                if(atoiSW(sNr) == street_number)
                {
                    a.r = r->rect;
                }
            }
        }
        else if (street_number && sType == "associatedStreet")
        {
            for(auto p : item->points)
            {
                std::string_view sNr = p->tags["addr:housenumber"];
                if(atoiSW(sNr) == street_number)
                {
                    a.r.x0 = p->x;
                    a.r.x1 = p->x;
                    a.r.y0 = p->y;
                    a.r.y1 = p->y;
                }
            }
            for(auto w : item->ways)
            {
                std::string_view sNr = w->tags["addr:housenumber"];
                if(atoiSW(sNr) == street_number)
                {
                    a.r = w->rect;

                }
            }
            for(auto r : item->relations)
            {
                std::string_view sNr = r->tags["addr:housenumber"];
                if(atoiSW(sNr) == street_number)
                {
                    a.r = r->rect;
                }
            }
        }
        if(item->isClosed)
        {
            // search in relation point to set pin
            for(auto p : item->points)
            {
                if ((p->tags["place"] != "")
                    && (p->tags["name"] == item->tags["name"]))
                    {
                    if(
                      (p->x >= a.r.x0) &&
                      (p->x <= a.r.x1) &&
                      (p->y >= a.r.y0) &&
                      (p->y <= a.r.y1)
                      )
                    {
                        pin.x = p->x;
                        pin.y = p->y;
                        break;
                    }
                }
            }
        }
        else if (item->shape.openedLines.size() == 1)
        {
            Line* l =  item->shape.openedLines[0];
             // set pin to middle of shape points
            if(
              (l->points[l->pointsCount/2].x >= a.r.x0) &&
              (l->points[l->pointsCount/2].x <= a.r.x1) &&
              (l->points[l->pointsCount/2].y >= a.r.y0) &&
              (l->points[l->pointsCount/2].y <= a.r.y1)
              )
            {
                pin.x = l->points[l->pointsCount/2].x;
                pin.y =  l->points[l->pointsCount/2].y;
            }
        }
        else
        {
            pin = {0,0};
        }
        if((pin.x)
           && (pin.x >= a.r.x0)
           && (pin.x <= a.r.x1)
           && (pin.y >= a.r.y0)
           && (pin.y <= a.r.y1))
        {
            a.pin = pin;
        }

    }
}


std::shared_ptr<Msg> Geolocation::processRequest(std::shared_ptr<Msg> request, CompiledDataManager& mger)
{
    std::string name = HttpEncoder::urlDecode(request->getRecord(1)->getNamedValue("name"));
    std::string mode = HttpEncoder::urlDecode(request->getRecord(1)->getNamedValue("mode"));
    std::string mag = HttpEncoder::urlDecode(request->getRecord(1)->getNamedValue("mag"));
    std::string word;

    std::stringstream my_stream(name);

    std::list<weightedArea> areas;
    std::list<weightedArea> best_areas;
    std::list<weightedArea> new_areas;

    areas.clear();
    best_areas.clear();
    new_areas.clear();

    int street_number = 0;
    while(std::getline(my_stream,word,','))
    {
        //trim word:
        size_t left;
        size_t right;

        left = 0;
        right = word.size() - 1;

        while(left < right && isspace(word.c_str()[left])) ++left;
        while(left <= right && isspace(word.c_str()[right])) --right;
        word = word.substr(left, (right - left + 1));
        std::cout << "search trimmed expr : [" << word << "]\n";

        bool is_number = (!word.empty());
        if(!(word[0] >= '0' && word[0] <='9')){
            is_number = false;
        }
           if(is_number){
             street_number = atoi(word.c_str());
             continue;
        }

        if(best_areas.empty())
        {
            best_areas = findExpression(word, mger);
        }
        else
        {
            areas = findExpression(word, mger);
            new_areas.clear();
            areas.sort(compare_weight);
            if(areas.size() > 250) areas.resize(250);
            if(!areas.empty())
            {
                for(auto a : areas)
                {
                    for(auto b : best_areas)
                    {
                        if ((b.r * a.r).isValid())
                        {
                            weightedArea c;
                            c.r = b.r * a.r;
                            c.score = a.score + b.score;
                            c.found = a.found + ", " + b.found;
                            c.pin = {0,0};
                            for(auto r : a.relations) c.relations.push_back(r);
                            for(auto r : b.relations) c.relations.push_back(r);
                            new_areas.push_back(c);
                        }
                    }
                }
                best_areas = new_areas;
                best_areas.sort(compare_weight);
                if(best_areas.size() > 250) best_areas.resize(250);
            }
        }

    }


    best_areas.sort(compare_weight);
    if(best_areas.size() > 250) best_areas.resize(250);



    if(mode == "xml")
    {
        std::string resp = "<root>\n";
        int i = 0;
        for(auto a : best_areas)
        {
            if(i > MAX_RESULTS) break;
            if(! a.r.isValid() || !a.checkIntersect(mger)) continue;
            fillPin(mger, a, street_number);
            std::string sPin = "";
            if(a.pin.x) sPin =
                "pin_lon=\""+std::to_string(Coordinates::fromNormalizedLon(a.pin.x))+"\" "
                "pin_lat=\""+std::to_string(Coordinates::fromNormalizedLat(a.pin.y))+"\" ";
            resp += "<area "
                "lon_min=\""+std::to_string(Coordinates::fromNormalizedLon(a.r.x0))+"\" "
                "lon_max=\""+std::to_string(Coordinates::fromNormalizedLon(a.r.x1))+"\" "
                "lat_min=\""+std::to_string(Coordinates::fromNormalizedLat(a.r.y1))+"\" "
                "lat_max=\""+std::to_string(Coordinates::fromNormalizedLat(a.r.y0))+"\" "
                + sPin +
                "found=\""+a.found+"\" "
                "score=\""+std::to_string(a.score)+"\" />\n";
            i++;
        }
       resp += "</root>\n";
       auto rep = std::make_shared<Msg>();
       encoder.build200Header(rep, "application/xml");
       encoder.addContent(rep,resp);
       return rep;
   } else if (mode =="json"){
        std::string resp = "[";
        int i = 0;
        for(auto a : best_areas)
        {
            if(i > MAX_RESULTS) break;
            if(! a.r.isValid() || !a.checkIntersect(mger)) continue;
            fillPin(mger, a, street_number);
            if(i) resp += ", ";
            std::string sPin = "";
            if(a.pin.x) sPin =
                ", \"pin_lon\":"+std::to_string(Coordinates::fromNormalizedLon(a.pin.x))+
                ", \"pin_lat\":"+std::to_string(Coordinates::fromNormalizedLat(a.pin.y));
             resp+="{"
                "\"lon_min\":"+std::to_string((Coordinates::fromNormalizedLon(a.r.x0)))+
                ", \"lon_max\":"+std::to_string(Coordinates::fromNormalizedLon(a.r.x1))+
                ", \"lat_min\":"+std::to_string(Coordinates::fromNormalizedLat(a.r.y1))+
                sPin +
                ", \"lat_max\":"+std::to_string(Coordinates::fromNormalizedLat(a.r.y0))+
                ", \"found\":\""+a.found+"\""+
                ", \"score\":"+std::to_string(a.score);
            resp+="}\n";
            i++;
        }
       resp += "]\n";
       auto rep = std::make_shared<Msg>();
       encoder.build200Header(rep, "application/json");
       encoder.addContent(rep,resp);
       return rep;
   } else {
        weightedArea result;
        for(auto a : best_areas)
        {
            if(a.checkIntersect(mger))
            {
                result = a;
                break;
            }
        }
        fillPin(mger, result, street_number);
        Rectangle r = result.r;
        int zlevel = 32;
        //best_size *= 1.5;
        uint64_t best_size = (r.x1 - r.x0);
        if ((r.y1 - r.y0) > (r.x1 - r.x0)) best_size =(r.y1 - r.y0);
        while(best_size)
        {
            zlevel --;
            best_size >>= 1;
        }
        //std::cout << zlevel << "\n";
        if(zlevel > 17) zlevel = 17;
        if(zlevel < 0) zlevel = 0;

        auto rep = std::make_shared<Msg>();
        std::string URL;
        if(result.pin.x){
            URL = "/MapDisplay?pin=true&longitude=" + std::to_string(Coordinates::fromNormalizedLon(result.pin.x)) + "&lattitude=" + std::to_string(Coordinates::fromNormalizedLat(result.pin.y))+"&zoom="+std::to_string(zlevel)+"";
        } else {
            URL = "/MapDisplay?pin=true&longitude=" + std::to_string(Coordinates::fromNormalizedLon(r.x0/2 + r.x1/2)) + "&lattitude=" + std::to_string(Coordinates::fromNormalizedLat(r.y0/2 + r.y1/2))+"&zoom="+std::to_string(zlevel)+"";
        }
        if(mag == "X2") URL += "&mag=X2";
        else if (mag == "X05") URL += "&mag=X05";
        encoder.build303Header(rep,URL);
        encoder.addContent(rep,"redirect!!!");
        return rep;
    }
}
bool weightedArea::checkIntersect(CompiledDataManager& mger)
{
    for(uint64_t id : relations)
    {
        bool result = true;
        std::shared_ptr<Relation> item = nullptr;
        item = mger.load<Relation>(id);
        if(item != nullptr)
        {
            result = false;
            for(auto l : item->shape.openedLines)
            {
                l->crop(r);
                if(l->pointsCount > 0) result = true;
            }
            for(auto l : item->shape.closedLines)
            {
                l->crop(r);
                if(l->pointsCount > 0) result = true;
            }
        }
        else return false;
        if (result== false) return false;
    }
    for(uint64_t id : ways)
    {
        bool result = true;
        std::shared_ptr<Way> item = nullptr;
        item = mger.load<Way>(id);
        if(item != nullptr){
            item->crop(r);
            if(item->pointsCount == 0) result = false;
        }
        else return false;
        if (result== false) return false;
    }
    for(uint64_t id : nodes)
    {
        bool result =  true;
        std::shared_ptr<Point> item = nullptr;
        item = mger.load<Point>(id);
        if(item != nullptr)
        {
            if(item->x < r.x0) result = false;
            if(item->x > r.x1) result = false;
            if(item->y < r.y0) result = false;
            if(item->y > r.y1) result = false;
        }
        else return false;
        if (result== false) return false;
    }
    return true;
}
