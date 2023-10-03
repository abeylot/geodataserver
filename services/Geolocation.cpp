#include "Geolocation.hpp"
#include "../Coordinates.hpp"
#include "ServicesFactory.hpp"
#include "../common/constants.hpp"
#include <stdlib.h>

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
        for(auto i : a) if(i != UNDEFINED_ID) new_a.push_back(i);
        for(auto i : b) if(i != UNDEFINED_ID) new_b.push_back(i);
        
        a = new_a;
        b = new_b;
        
        new_a.clear();
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
        
        std::vector<uint64_t> new_a, new_b;
    }

        for(auto i : a) if(i != UNDEFINED_ID) new_a.push_back(i);
        for(auto i : b) if(i != UNDEFINED_ID) new_b.push_back(i);
        
        a = new_a;
        b = new_b;
        
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
              for(auto i : a) if(i != UNDEFINED_ID) new_a.push_back(i);
            for(auto i : b) if(i != UNDEFINED_ID) new_b.push_back(i);
        
            a = new_a;
            b = new_b;
        
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
        sWord = "";
        node_start_id = UNDEFINED_ID;
        node_stop_id  = UNDEFINED_ID;

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


bool compare_weight (const weightedArea first, const weightedArea second)
{
  return ( first.score > second.score );
}

std::list<weightedArea> Geolocation::findExpression(std::string expr, CompiledDataManager& mger)
{
    std::cout << "Search : [" << expr << "]\n";
    std::stringstream my_stream(expr);
    
    std::list<textSearchIds> foundWords;
    std::vector<uint64_t> queryWordsVector;
    std::list<uint64_t> words;
    std::list<uint64_t> wordsToMatch;
    std::string word;
    while(std::getline(my_stream,word,' '))
    {
        if(!word.empty())
        {
            uint64_t k = fidx::makeLexicalKey(word.c_str(), word.length(), *mger.charconvs);
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
                for(uint64_t i = searchIds.node_start_id; i <= searchIds.node_stop_id; i++)
                {
                    weightedArea area;
                    area.pin.x = area.pin.y = 0;
                    fidx::Record<IndexEntry, uint64_t> record;
                    std::vector<uint64_t> nameWordVector;
                    mger.textIndexNode->get(i, &record);
                    Point* item;
                    mger.load(item, record.value.id, true);            
                    area.r.x0 = area.r.x1 = item->x;
                    area.r.y0 = area.r.y1 = item->y;
                    std::string my_string = item->tags["name"];
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
                    if(area.r.isValid()) area.score = calcMatchScore(queryWordsVector, nameWordVector);
                    else area.score = WORST_SCORE;
                    std::string sPlace = item->tags["admin_level"];
                    if(!sPlace.empty())
                    {
                        int level = 10 - atoi(sPlace.c_str());
                        if(level) area.score *= (1.0 + 0.1 / level);
                    }
                    area.found = "Node_"+std::to_string(record.value.id)+":"+my_string;
                    areas.push_back(area);
                    delete item;            
                }
            }
            if (searchIds.relation_start_id != UNDEFINED_ID)
            {
                for(uint64_t i = searchIds.relation_start_id; i <= searchIds.relation_stop_id; i++)
                {
                    weightedArea area;
                    area.pin.x = area.pin.y = 0;
                    fidx::Record<IndexEntry, uint64_t> record;
                    std::vector<uint64_t> nameWordVector;
                    mger.textIndexRelation->get(i, &record);
                    Relation* item;
                    item = mger.loadRelationFast(record.value.id);            
                    area.r =  record.value.r;
                    std::string my_string = item->tags["name"];
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

                    if(area.r.isValid()) area.score = calcMatchScore(queryWordsVector, nameWordVector) + 1;
                    else area.score = WORST_SCORE;

                    std::string sPlace = item->tags["admin_level"];
                    if(!sPlace.empty())
                    {
                        int level = 10 - atoi(sPlace.c_str());
                        if(level) area.score *= (1.0 + 0.1 / level);
                    }
                    area.found = "Relation_"+std::to_string(record.value.id)+":"+my_string;
                    area.relations.push_back(record.value.id);
                                
                    areas.push_back(area);
                    delete item;            
                }
            } 
            if (searchIds.way_start_id != UNDEFINED_ID)
            {
                for(uint64_t i = searchIds.way_start_id; i <= searchIds.way_stop_id; i++)
                {
                    weightedArea area;
                    area.pin.x = area.pin.y = 0;
                    fidx::Record<IndexEntry, uint64_t> record;
                    std::vector<uint64_t> nameWordVector;
                    mger.textIndexWay->get(i, &record);
                    Way* item;
                    mger.load(item, record.value.id, true);            
                    area.r =  record.value.r;
                    std::string my_string = item->tags["name"];
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
                    if(area.r.isValid()) area.score = calcMatchScore(queryWordsVector, nameWordVector);
                    else area.score = WORST_SCORE;
                    std::string sPlace = item->tags["admin_level"];
                    if(!sPlace.empty())
                    {
                        int level = 10 - atoi(sPlace.c_str());
                        if(level) area.score *= (1.0 + 0.1 / level);
                    }
                    area.found = "Way_"+std::to_string(record.value.id)+":"+my_string;
                    areas.push_back(area);
                    delete item;            
                }
            }
        }
        if(best_areas.size() == 0)
        {
            areas.sort(compare_weight);
            for(auto a : areas)
            {
                unsigned int i = 0;
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
        Relation* item;
        mger.load(item, relid, true);
        GeoPoint pin = {0,0};

        std::string sType = item->tags["type"];
        if(street_number && sType == "street")
        {
            for(auto p : item->points)
            {
                std::string sNr = p->tags["addr:housenumber"];
                if(atoi(sNr.c_str()) == street_number)
                {
                    a.r.x0 = p->x;
                    a.r.x1 = p->x;
                    a.r.y0 = p->y;
                    a.r.y1 = p->y;
                }
            }            
            for(auto w : item->ways)
            {
                std::string sNr = w->tags["addr:housenumber"];
                if(atoi(sNr.c_str()) == street_number)
                {
                    a.r = w->rect;
                }
            }            
            for(auto r : item->relations)
            {
                std::string sNr = r->tags["addr:housenumber"];
                if(atoi(sNr.c_str()) == street_number)
                {
                    a.r = r->rect;
                }
            }            
        }
        else if (street_number && sType == "associatedStreet")
        {
            for(auto p : item->points)
            {
                std::string sNr = p->tags["addr:housenumber"];
                if(atoi(sNr.c_str()) == street_number)
                {
                    a.r.x0 = p->x;
                    a.r.x1 = p->x;
                    a.r.y0 = p->y;
                    a.r.y1 = p->y;
                }
            }            
            for(auto w : item->ways)
            {
                std::string sNr = w->tags["addr:housenumber"];
                if(atoi(sNr.c_str()) == street_number)
                {
                    a.r = w->rect;

                }
            }            
            for(auto r : item->relations)
            {
                std::string sNr = r->tags["addr:housenumber"];
                if(atoi(sNr.c_str()) == street_number)
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
        else if (item->shape.lines.size() == 1)
        {
            Line* l =  item->shape.lines[0];
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


Msg* Geolocation::processRequest(Msg* request, CompiledDataManager& mger)
{
    std::string name = HttpEncoder::urlDecode(request->getRecord(1)->getNamedValue("name"));
    std::string mode = HttpEncoder::urlDecode(request->getRecord(1)->getNamedValue("mode"));
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

        if(best_areas.size() == 0)
        {
            best_areas = findExpression(word, mger);
        }
        else
        {
            areas = findExpression(word, mger);
            new_areas.clear();
            if(areas.size())
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
            }
        }
              
    }

    
      weightedArea result;
      best_areas.sort(compare_weight);
      
      
      if(best_areas.size()) result = *(best_areas.begin()); 

//    for (auto a: best_areas)
//    {
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
    
        if(mode == "xml")    
        {
            std::string resp = "<root>\n";
            int i = 0;
            for(auto a : best_areas)
            {    
                if(i > MAX_RESULTS) break;
                if(! a.r.isValid()) continue;
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
           Msg* rep = new Msg;
           encoder.build200Header(rep, "application/xml");
           encoder.addContent(rep,resp);
           return rep;
       } else if (mode =="json"){
            std::string resp = "[";
            int i = 0;
            for(auto a : best_areas)
            {
                if(i > MAX_RESULTS) break;
                if(! a.r.isValid()) continue;
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
           Msg* rep = new Msg;
           encoder.build200Header(rep, "application/json");
           encoder.addContent(rep,resp);
           return rep;
       } else {
       
            Msg* rep = new Msg;
            std::string URL;
            if(result.pin.x){
                URL = "/MapDisplay?pin=true&longitude=" + std::to_string(Coordinates::fromNormalizedLon(result.pin.x)) + "&lattitude=" + std::to_string(Coordinates::fromNormalizedLat(result.pin.y))+"&zoom="+std::to_string(zlevel)+"";
            } else { 
                URL = "/MapDisplay?pin=true&longitude=" + std::to_string(Coordinates::fromNormalizedLon(r.x0/2 + r.x1/2)) + "&lattitude=" + std::to_string(Coordinates::fromNormalizedLat(r.y0/2 + r.y1/2))+"&zoom="+std::to_string(zlevel)+"";
            }
            encoder.build303Header(rep,URL);
            encoder.addContent(rep,"redirect!!!");
            return rep;
        }
}
