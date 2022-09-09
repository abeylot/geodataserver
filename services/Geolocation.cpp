#include "Geolocation.hpp"
#include "../Coordinates.hpp"
#include "ServicesFactory.hpp"
#include "../common/constants.hpp"
#include <stdlib.h>

const uint64_t UNDEFINED_ID = 0xFFFFFFFFFFFFFFFF;


uint64_t calcMatchScore(std::vector<uint64_t> a, std::vector<uint64_t> b)
{
	uint64_t result = 1000000;
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
					if(i!=0) result -=10;
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




std::list<weightedArea> Geolocation::findExpression(std::string expr, CompiledDataManager& mger)
{
	std::cout << "Search : " << expr << "\n";
    std::stringstream my_stream(expr);
    
    std::list<textSearchIds> foundWords;
    std::vector<uint64_t> queryWordsVector;
    std::list<uint64_t> words;
    std::list<uint64_t> wordsToMatch;
    std::string word;
    while(std::getline(my_stream,word,' '))
    {
        uint64_t k = fidx::makeLexicalKey(word.c_str(), word.length());
		words.push_back(k);
        queryWordsVector.push_back(k);
    }
    
    words.sort();
    words.unique();
    
    for(auto myword : words)
    {
        fidx::Record<IndexRange, uint64_t> eN;
        fidx::Record<IndexRange, uint64_t> eW;
        fidx::Record<IndexRange, uint64_t> eR;



        bool foundN = mger.textIndexNodeRange->find(myword, &eN);
        bool foundW = mger.textIndexWayRange->find(myword, &eW);
        bool foundR = mger.textIndexRelationRange->find(myword, &eR);
        
        uint64_t nstart, nstop, wstart, wstop, rstart, rstop;
        nstart = nstop = wstart = wstop = rstart = rstop = UNDEFINED_ID;
        
        if (foundN)
        {
			nstart = eN.value.first;
			nstop  = eN.value.last;
		}
        if (foundW)
        {
			wstart = eW.value.first;
			wstop  = eW.value.last;
		}
        if (foundR)
        {
			rstart = eR.value.first;
			rstop  = eR.value.last;
		}
		if(foundN || foundW || foundR)
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
			        fidx::Record<IndexEntry, uint64_t> record;
			        std::vector<uint64_t> nameWordVector;
			        mger.textIndexNode->get(i, &record);
			        Point* item;
                    mger.load(item, record.value.id, true);		    
			        area.r.x0 = area.r.x1 = item->x;
			        area.r.y0 = area.r.y1 = item->y;
                    std::stringstream my_stream(item->tags["name"]);
                    std::string word;
                    while(std::getline(my_stream,word,' '))
                    {
                        uint64_t k = fidx::makeLexicalKey(word.c_str(), word.length());
		                nameWordVector.push_back(k);
                    }
			        area.score = calcMatchScore(queryWordsVector, nameWordVector);
			        areas.push_back(area);
			        delete item;		    
			    }
            }
		    if (searchIds.relation_start_id != UNDEFINED_ID)
		    {
			    for(uint64_t i = searchIds.relation_start_id; i <= searchIds.relation_stop_id; i++)
			    {
			        weightedArea area;
			        fidx::Record<IndexEntry, uint64_t> record;
			        std::vector<uint64_t> nameWordVector;
			        mger.textIndexRelation->get(i, &record);
			        Relation* item;
                    mger.load(item, record.value.id, true);		    
			        area.r =  record.value.r;
                    std::stringstream my_stream(item->tags["name"]);
                    std::string word;
                    while(std::getline(my_stream,word,' '))
                    {
                        uint64_t k = fidx::makeLexicalKey(word.c_str(), word.length());
		                nameWordVector.push_back(k);
                    }
			        area.score = calcMatchScore(queryWordsVector, nameWordVector);
			        areas.push_back(area);
			        delete item;		    
			    }
            } 
		    if (searchIds.way_start_id != UNDEFINED_ID)
		    {
			    for(uint64_t i = searchIds.way_start_id; i <= searchIds.way_stop_id; i++)
			    {
			        weightedArea area;
			        fidx::Record<IndexEntry, uint64_t> record;
			        std::vector<uint64_t> nameWordVector;
			        mger.textIndexWay->get(i, &record);
			        Way* item;
                    mger.load(item, record.value.id, true);		    
			        area.r =  record.value.r;
                    std::stringstream my_stream(item->tags["name"]);
                    std::string word;
                    while(std::getline(my_stream,word,' '))
                    {
                        uint64_t k = fidx::makeLexicalKey(word.c_str(), word.length());
		                nameWordVector.push_back(k);
                    }
			        area.score = calcMatchScore(queryWordsVector, nameWordVector);
			        areas.push_back(area);
			        delete item;		    
			    }
		    }
		    // keep only best score(s)
		}
		if(best_areas.size() == 0)
		{
		    uint64_t best_score = 0;
		    for(auto a : areas) {if (a.score > best_score) best_score = a.score;} 
		    for(auto a : areas)
		    {
		        if (a.score == best_score) best_areas.push_back(a);
			}
			break;
		}
		/*else
		{
			for(auto b: best_areas)
			{
				auto it = std::find(b.words.begin(), b.words.end(), searchIds.word);
				if(it ==  b.words.end())
				{
				    for(auto a:areas)
				    {
						if ((b.r * a.r).isValid())
						{
							b.r = b.r + a.r;
							b.score += a.score;
							for(auto w : a.words) b.words.push_back(w);
						}
				    }
				}
			}
		}*/
		
	}
    return best_areas;
}


Msg* Geolocation::processRequest(Msg* request, CompiledDataManager& mger)
{
    std::string name = HttpEncoder::urlDecode(request->getRecord(1)->getNamedValue("name"));

    std::string resp = "<root>";
    std::string word;
    
    std::stringstream my_stream(name);
    
    std::list<weightedArea> areas;
    std::list<weightedArea> best_areas;
    std::list<weightedArea> new_areas;
    
    areas.clear();
    best_areas.clear();
    new_areas.clear();
    
    while(std::getline(my_stream,word,','))
    {
		if(best_areas.size() == 0)
		{
			best_areas = findExpression(word, mger);
		}
		else
		{
			areas = findExpression(word, mger);
			if(areas.size())
			{
				for(auto a : areas) for(auto b : best_areas)
				{
					if ((b.r * a.r).isValid())
					{
						weightedArea c;
						c.r = b.r * a.r;
						c.score = a.score + b.score;
						new_areas.push_back(c);
					}
				}
				best_areas = new_areas;
			}
		}
		      
	}

    uint64_t best_size = 0;
    weightedArea result;
    for (auto a: best_areas)
    {
		if(((a.r.x1 - a.r.x0)*(a.r.y1 - a.r.y0)) > best_size)
		{
			result = a;
			best_size = (a.r.x1 - a.r.x0)*(a.r.y1 - a.r.y0);
		} 
	}

//    for (auto a: best_areas)
//    {
		Rectangle r = result.r;
		resp += "<score value=\"" + std::to_string(result.score) + "\"/>"; 
        resp += "<url u=\"http://127.0.0.1:8081/svgMap.svg?longitude1=" + std::to_string(r.x0) + "&amp;lattitude1=" + std::to_string(r.y0) + "&amp;longitude2=" + std::to_string(r.x1) + "&amp;lattitude2=" + std::to_string(r.y1) + "\" />";
        resp += "<url u=\"http://127.0.0.1:8081/MapDisplay?longitude=" + std::to_string(Coordinates::fromNormalizedLon(r.x0/2 + r.x1/2)) + "&amp;lattitude=" + std::to_string(Coordinates::fromNormalizedLat(r.y0/2 + r.y1/2))+"&amp;zoom=17\"/>";
        resp += "<rectangle xo=\"" + std::to_string(Coordinates::fromNormalizedLon(r.x0))
             + "\" y0=\"" + std::to_string(Coordinates::fromNormalizedLat(r.y0))
             + "\" x1=\"" + std::to_string(Coordinates::fromNormalizedLon(r.x1))
             + "\" y1=\"" + std::to_string(Coordinates::fromNormalizedLat(r.y1))
            	              + "\" />";
       resp += "<rectangle xo=\"" + std::to_string(Coordinates::toNormalizedLon(std::to_string(Coordinates::fromNormalizedLon(r.x0))))
            	              + "\" y0=\"" + std::to_string(Coordinates::toNormalizedLat(std::to_string(Coordinates::fromNormalizedLat(r.y0))))
            	              + "\" x1=\"" + std::to_string(Coordinates::toNormalizedLon(std::to_string(Coordinates::fromNormalizedLon(r.x1))))
            	              + "\" y1=\"" + std::to_string(Coordinates::toNormalizedLat(std::to_string(Coordinates::fromNormalizedLat(r.y1))))
            	              + "\" />";
//	}		
    
    resp +=            "</root>";
    Msg* rep = new Msg;
    encoder.build200Header(rep, "application/xml");
    encoder.addContent(rep,resp);
    return rep;
}
