#include "Geolocation.hpp"
#include "../Coordinates.hpp"
#include "ServicesFactory.hpp"
#include "../common/constants.hpp"
#include <stdlib.h>

const uint64_t UNDEFINED_ID = 0xFFFFFFFFFFFFFFFF;
const int64_t  WORST_SCORE  = -999999999;

bool weightedAreaCompare (const weightedArea& first, const weightedArea& second)
{
  if ( first.score < second.score ) return false;
  if ( first.score > second.score ) return true;
  if ( (first.r.x1 - first.r.x0)*(first.r.y1 - first.r.y0) < (second.r.x1 - second.r.x0)*(second.r.y1 - second.r.y0) ) return false;
  return true;
}

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




std::list<weightedArea> Geolocation::findExpression(std::string expr, CompiledDataManager& mger, int street_number)
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
	        //std::cout << "word : [" << word << "]\n";
    		//else {
			    uint64_t k = fidx::makeLexicalKey(word.c_str(), word.length(), *mger.charconvs);
			    words.push_back(k);
			    queryWordsVector.push_back(k);
			//}
            //find number if exist
            //std::cout << "number " << street_number << "\n";
 		}
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
		
		std::cout << ((nstop - nstart) + (wstop - wstart) + (rstop -rstart)) << "\n";
		
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
		//std::cout << ((searchIds.node_stop_id - searchIds.node_start_id) + (searchIds.way_stop_id - searchIds.way_start_id) + (searchIds.relation_stop_id - searchIds.relation_start_id)) << "xxx\n";
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
			        //std::cout << my_string << " " << area.score << "\n";
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
                    //mger.load(item, record.value.id, true);
                    //item = mger.loadRelationFast(record.value.id);		    
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
			        std::string sType = item->tags["type"];
			        if(street_number && sType == "street")
			        {
						//need to reload with full datas in this case
						//delete item;
                        //mger.load(item, record.value.id, true);
                        for(auto p : item->points)
                        {
							std::string sNr = p->tags["addr:housenumber"];
							if(atoi(sNr.c_str()) == street_number)
							{
								area.r.x0 = p->x;
								area.r.x1 = p->x;
								area.r.y0 = p->y;
								area.r.y1 = p->y;
							}
						}		    
                        for(auto w : item->ways)
                        {
							std::string sNr = w->tags["addr:housenumber"];
							if(atoi(sNr.c_str()) == street_number)
							{
								area.r = w->rect;

                            }
						}		    
                        for(auto r : item->relations)
                        {
							std::string sNr = r->tags["addr:housenumber"];
							if(atoi(sNr.c_str()) == street_number)
							{
								area.r = r->rect;
                            }
						}		    
				    }
				    else if (street_number && sType == "associatedStreet")
				    {
						//need to reload with full datas in this case
						//delete item;
                        //mger.load(item, record.value.id, true);
                        for(auto p : item->points)
                        {
							std::string sNr = p->tags["addr:housenumber"];
							if(atoi(sNr.c_str()) == street_number)
							{
								area.r.x0 = p->x;
								area.r.x1 = p->x;
								area.r.y0 = p->y;
								area.r.y1 = p->y;
							}
						}		    
                        for(auto w : item->ways)
                        {
							std::string sNr = w->tags["addr:housenumber"];
							if(atoi(sNr.c_str()) == street_number)
							{
								area.r = w->rect;

                            }
						}		    
                        for(auto r : item->relations)
                        {
							std::string sNr = r->tags["addr:housenumber"];
							if(atoi(sNr.c_str()) == street_number)
							{
								area.r = r->rect;
                            }
						}		    
					} 	
			        std::string sPlace = item->tags["admin_level"];
			        if(!sPlace.empty())
			        {
						int level = 10 - atoi(sPlace.c_str());
						if(level) area.score *= (1.0 + 0.1 / level);
					}
			        //std::cout << my_string << " " << area.score << "\n";
			        area.found = "Relation_"+std::to_string(record.value.id)+":"+my_string;
			        
			        if(item->isClosed)
			        {
						// search in relation point to set pin
						for(auto p : item->points)
						{
							if ((p->tags["place"] != "")
							    && (p->tags["name"] == item->tags["name"])) 
							{
								if(
								  (p->x >= area.r.x0) &&
								  (p->x <= area.r.x1) &&
								  (p->y >= area.r.y0) &&
								  (p->y <= area.r.y1)
								  )
								{ 
								    area.pin.x = p->x;
								    area.pin.y = p->y;
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
						  (l->points[l->pointsCount/2].x >= area.r.x0) &&
						  (l->points[l->pointsCount/2].x <= area.r.x1) &&
						  (l->points[l->pointsCount/2].y >= area.r.y0) &&
						  (l->points[l->pointsCount/2].y <= area.r.y1)
						  )
						{ 
					        area.pin.x = l->points[l->pointsCount/2].x;
					        area.pin.y =  l->points[l->pointsCount/2].y;
						}
					}
					else
					{
						area.pin = {0,0};
					}
			        
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
			        //std::cout << my_string << " " << area.score << "\n";
			        area.found = "Way_"+std::to_string(record.value.id)+":"+my_string;
			        areas.push_back(area);
			        delete item;		    
			    }
		    }
		    // keep only best score(s)
		}
		if(best_areas.size() == 0)
		{
		    int64_t best_score = WORST_SCORE;
		    int64_t best_score2 = WORST_SCORE;
		    int64_t best_score3 = WORST_SCORE;
		    for(auto a : areas) {if (a.score > best_score) best_score = a.score;} 
		    for(auto a : areas) {if (a.score > best_score2 && a.score != best_score) best_score2 = a.score;} 
		    for(auto a : areas) {if (a.score > best_score3 && a.score != best_score2 && a.score != best_score) best_score3 = a.score;} 

		    for(auto a : areas)
		    {
		        if (
		        (a.score == best_score)
		        ||(a.score == best_score2)
		        ||(a.score == best_score3)
		        ) best_areas.push_back(a);
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
	//std::cout << best_areas.size() << " results\n";
    return best_areas;
}


Msg* Geolocation::processRequest(Msg* request, CompiledDataManager& mger)
{
    std::string name = HttpEncoder::urlDecode(request->getRecord(1)->getNamedValue("name"));
    std::string mode = HttpEncoder::urlDecode(request->getRecord(1)->getNamedValue("mode"));
    //std::cout << "search expr::" << name << "\n";
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
        bool is_number = (!word.empty());
	    //for( unsigned int k = 0; k < word.length(); k++)
	    //{
		    if(!(word[0] >= '0' && word[0] <='9')){
		    	is_number = false;
	     		//break;
			}
    	//}
   		if(is_number){
			 street_number = atoi(word.c_str());
			 continue;
		 }

		if(best_areas.size() == 0)
		{
			best_areas = findExpression(word, mger, street_number);
		}
		else
		{
			areas = findExpression(word, mger, street_number);
			new_areas.clear();
			if(areas.size())
			{
				for(auto a : areas)
				{
					for(auto b : best_areas)
				    {
					    if ((b.r * a.r).isValid())
					    {
						    //std::cout << "match !!" << "\n";
						    weightedArea c;
						    c.r = b.r * a.r;
						    c.score = a.score + b.score;
						    c.found = a.found + ", " + b.found;
						    if((a.pin.x)
						       && (a.pin.x >= b.r.x0)
						       && (a.pin.x <= b.r.x1)
						       && (a.pin.y >= b.r.y0)
						       && (a.pin.y <= b.r.y1))
						    {
								c.pin = a.pin;
						    }
						    else
						    if((b.pin.x)
						       && (b.pin.x >= a.r.x0)
						       && (b.pin.x <= a.r.x1)
						       && (b.pin.y >= a.r.y0)
						       && (b.pin.y <= a.r.y1))
						    {
								c.pin = b.pin;
						    }
						    else
						    {
								c.pin = {0,0};
							}
						    new_areas.push_back(c);
					    } else {
						    /*std::cout << "no match !!" << "\n";						
						    std::cout << a.r.x0 <<","<<  a.r.y0<<","<< a.r.x1<<","<< a.r.y1 << " * ";						
						    std::cout << b.r.x0 <<","<< b.r.y0<<","<< b.r.x1<<","<< b.r.y1 << " = ";
						    Rectangle c = a.r * b.r;						
						    std::cout << c.x0<<","<< c.y0<<","<< c.x1<<","<< c.y1 << "\n";*/
					    }
				    }
			    }
				best_areas = new_areas;
			}
		}
		      
	}

    /*uint64_t best_size = -1;
    int64_t best_score = -999999999;
    weightedArea result;
    for (auto a: best_areas)
    {
		if(a.r.isValid() && (a.score > best_score)) best_score = a.score;
	}
	std::cout << "best score : "<< best_score << "\n";
    for (auto a: best_areas)
    {
		if(a.score == best_score)
		if(a.r.isValid() && (((a.r.x1 - a.r.x0)*(a.r.y1 - a.r.y0)) >= best_size))
		{
			result = a;
			best_size = (a.r.x1 - a.r.x0)*(a.r.y1 - a.r.y0);
		} 
	}*/
	
	  weightedArea result;
	  best_areas.sort(weightedAreaCompare);
	  if(best_areas.size()) result = *(best_areas.begin()); 

//    for (auto a: best_areas)
//    {
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
				if(i > 50) break;
				if(! a.r.isValid()) continue;
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
		    /*resp += "<score value=\"" + std::to_string(result.score) + "\"/>"; 
            resp += "<url u=\"http://127.0.0.1:8081/svgMap.svg?longitude1=" + std::to_string(r.x0) + "&amp;lattitude1=" + std::to_string(r.y0) + "&amp;longitude2=" + std::to_string(r.x1) + "&amp;lattitude2=" + std::to_string(r.y1) + "\" />";
            resp += "<url u=\"http://127.0.0.1:8081/MapDisplay?longitude=" + std::to_string(Coordinates::fromNormalizedLon(r.x0/2 + r.x1/2)) + "&amp;lattitude=" + std::to_string(Coordinates::fromNormalizedLat(r.y0/2 + r.y1/2))+"&amp;zoom="+std::to_string(zlevel)+"\"/>";
            resp += "<rectangle xo=\"" + std::to_string(Coordinates::fromNormalizedLon(r.x0))
                 + "\" y0=\"" + std::to_string(Coordinates::fromNormalizedLat(r.y0))
                 + "\" x1=\"" + std::to_string(Coordinates::fromNormalizedLon(r.x1))
                 + "\" y1=\"" + std::to_string(Coordinates::fromNormalizedLat(r.y1))
            	              + "\" />";
           resp += "<rectangle xo=\"" + std::to_string(Coordinates::toNormalizedLon(std::to_string(Coordinates::fromNormalizedLon(r.x0))))
            	              + "\" y0=\"" + std::to_string(Coordinates::toNormalizedLat(std::to_string(Coordinates::fromNormalizedLat(r.y0))))
            	              + "\" x1=\"" + std::to_string(Coordinates::toNormalizedLon(std::to_string(Coordinates::fromNormalizedLon(r.x1))))
            	              + "\" y1=\"" + std::to_string(Coordinates::toNormalizedLat(std::to_string(Coordinates::fromNormalizedLat(r.y1))))
            	              + "\" />";*/
            	                 
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
				if(i > 50) break;
				if(! a.r.isValid()) continue;
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
		    /*resp += "<score value=\"" + std::to_string(result.score) + "\"/>"; 
            resp += "<url u=\"http://127.0.0.1:8081/svgMap.svg?longitude1=" + std::to_string(r.x0) + "&amp;lattitude1=" + std::to_string(r.y0) + "&amp;longitude2=" + std::to_string(r.x1) + "&amp;lattitude2=" + std::to_string(r.y1) + "\" />";
            resp += "<url u=\"http://127.0.0.1:8081/MapDisplay?longitude=" + std::to_string(Coordinates::fromNormalizedLon(r.x0/2 + r.x1/2)) + "&amp;lattitude=" + std::to_string(Coordinates::fromNormalizedLat(r.y0/2 + r.y1/2))+"&amp;zoom="+std::to_string(zlevel)+"\"/>";
            resp += "<rectangle xo=\"" + std::to_string(Coordinates::fromNormalizedLon(r.x0))
                 + "\" y0=\"" + std::to_string(Coordinates::fromNormalizedLat(r.y0))
                 + "\" x1=\"" + std::to_string(Coordinates::fromNormalizedLon(r.x1))
                 + "\" y1=\"" + std::to_string(Coordinates::fromNormalizedLat(r.y1))
            	              + "\" />";
           resp += "<rectangle xo=\"" + std::to_string(Coordinates::toNormalizedLon(std::to_string(Coordinates::fromNormalizedLon(r.x0))))
            	              + "\" y0=\"" + std::to_string(Coordinates::toNormalizedLat(std::to_string(Coordinates::fromNormalizedLat(r.y0))))
            	              + "\" x1=\"" + std::to_string(Coordinates::toNormalizedLon(std::to_string(Coordinates::fromNormalizedLon(r.x1))))
            	              + "\" y1=\"" + std::to_string(Coordinates::toNormalizedLat(std::to_string(Coordinates::fromNormalizedLat(r.y1))))
            	              + "\" />";*/
            	                 
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
