#include "Geolocation.hpp"
#include "../Coordinates.hpp"
#include "ServicesFactory.hpp"
#include "../common/constants.hpp"
#include <stdlib.h>

const uint64_t UNDEFINED_ID = 0xFFFFFFFFFFFFFFFF;

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
			uint64_t k = fidx::makeLexicalKey(word.c_str(), word.length(), *mger.charconvs);
			words.push_back(k);
			queryWordsVector.push_back(k);
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
			        area.score = calcMatchScore(queryWordsVector, nameWordVector);
			        std::string sPlace = item->tags["admin_level"];
			        if(!sPlace.empty())
			        {
						int level = 10 - atoi(sPlace.c_str());
						if(level) area.score *= (1.0 + 0.1 / level);
					}
			        //std::cout << my_string << " " << area.score << "\n";
			        area.found = "Node_"+std::to_string(i)+":"+my_string;
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
                    //mger.load(item, record.value.id, true);
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
			        area.score = calcMatchScore(queryWordsVector, nameWordVector) + 2;
			        std::string sPlace = item->tags["admin_level"];
			        if(!sPlace.empty())
			        {
						int level = 10 - atoi(sPlace.c_str());
						if(level) area.score *= (1.0 + 0.1 / level);
					}
			        //std::cout << my_string << " " << area.score << "\n";
			        area.found = "Relation_"+std::to_string(i)+":"+my_string;
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
			        area.score = calcMatchScore(queryWordsVector, nameWordVector) + 1;
			        std::string sPlace = item->tags["admin_level"];
			        if(!sPlace.empty())
			        {
						int level = 10 - atoi(sPlace.c_str());
						if(level) area.score *= (1.0 + 0.1 / level);
					}
			        //std::cout << my_string << " " << area.score << "\n";
			        area.found = "Way_"+std::to_string(i)+":"+my_string;
			        areas.push_back(area);
			        delete item;		    
			    }
		    }
		    // keep only best score(s)
		}
		if(best_areas.size() == 0)
		{
		    //uint64_t best_score = 0;
		    //for(auto a : areas) {if (a.score > best_score) best_score = a.score;} 
		    for(auto a : areas)
		    {
		        /*if (a.score == best_score)*/ best_areas.push_back(a);
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
    
    while(std::getline(my_stream,word,','))
    {
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
						    //std::cout << "match !!" << "\n";
						    weightedArea c;
						    c.r = b.r * a.r;
						    c.score = a.score + b.score;
						    c.found = a.found + ", " + b.found;
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

    uint64_t best_size = 0;
    int64_t best_score = 0;
    weightedArea result;
    for (auto a: best_areas)
    {
		if(a.score > best_score) best_score = a.score;
	}
	std::cout << "best score : "<< best_score << "\n";
    for (auto a: best_areas)
    {
		if(a.score == best_score)
		if(((a.r.x1 - a.r.x0)*(a.r.y1 - a.r.y0)) >= best_size)
		{
			result = a;
			best_size = (a.r.x1 - a.r.x0)*(a.r.y1 - a.r.y0);
		} 
	}
	
	  best_areas.sort(weightedAreaCompare);

//    for (auto a: best_areas)
//    {
		Rectangle r = result.r;
		int zlevel = 32;
		//best_size *= 1.5;
		best_size = (r.x1 - r.x0);
		if ((r.y1 - r.y0) > (r.x1 - r.x0)) best_size =(r.y1 - r.y0);
		while(best_size)
		{
			zlevel --;
			best_size >>= 1;
		}
		std::cout << zlevel << "\n";
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
                resp += "<area "
                    "lon_min=\""+std::to_string(Coordinates::fromNormalizedLon(a.r.x0))+"\" "
                    "lon_max=\""+std::to_string(Coordinates::fromNormalizedLon(a.r.x1))+"\" "
                    "lat_min=\""+std::to_string(Coordinates::fromNormalizedLat(a.r.y1))+"\" "
                    "lat_max=\""+std::to_string(Coordinates::fromNormalizedLat(a.r.y0))+"\" "
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
 				resp+="{"
                    "\"lon_min\":"+std::to_string((Coordinates::fromNormalizedLon(a.r.x0)))+
                    ", \"lon_max\":"+std::to_string(Coordinates::fromNormalizedLon(a.r.x1))+
                    ", \"lat_min\":"+std::to_string(Coordinates::fromNormalizedLat(a.r.y1))+
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
            std::string URL = "/MapDisplay?pin=true&longitude=" + std::to_string(Coordinates::fromNormalizedLon(r.x0/2 + r.x1/2)) + "&lattitude=" + std::to_string(Coordinates::fromNormalizedLat(r.y0/2 + r.y1/2))+"&zoom="+std::to_string(zlevel)+"";
            encoder.build303Header(rep,URL);
            encoder.addContent(rep,"redirect!!!");
            return rep;
		}
}
