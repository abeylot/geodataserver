#include "Geolocation.hpp"
#include "../Coordinates.hpp"
#include "ServicesFactory.hpp"
#include "../common/constants.hpp"
#include <list>
#include <stdlib.h>

Msg* Geolocation::processRequest(Msg* request, CompiledDataManager& mger)
{
    std::string name = HttpEncoder::urlDecode(request->getRecord(1)->getNamedValue("name"));
/*

    fidx::Record<IndexEntry, uint64_t> eN;
    fidx::Record<IndexEntry, uint64_t> eW;
    fidx::Record<IndexEntry, uint64_t> eR;

    uint64_t k = fidx::makeLexicalKey(name.c_str(), name.length());


    bool foundN = mger.textIndexNode->find(k, &eN);
    bool foundW = mger.textIndexWay->find(k, &eW);
    bool foundR = mger.textIndexRelation->find(k, &eR);

   if(foundR)
   {
    std::string resp = "<root>";
    resp += "<word_match match_count=\"" + std::to_string(999) + "\" item_count=\"" + std::to_string(1) + "\" >";
        Rectangle r = eR.value.r;
        resp += "<url u=\"http://127.0.0.1:8081/svgMap.svg?longitude1=" + std::to_string(r.x0) + "&amp;lattitude1=" + std::to_string(r.y0) + "&amp;longitude2=" + std::to_string(r.x1) + "&amp;lattitude2=" + std::to_string(r.y1) + "\" />";
        resp += "<rectangle xo=\"" + std::to_string(Coordinates::fromNormalizedLon(r.x0))
              + "\" y0=\"" + std::to_string(Coordinates::fromNormalizedLat(r.y0))
              + "\" x1=\"" + std::to_string(Coordinates::fromNormalizedLon(r.x1))
              + "\" y1=\"" + std::to_string(Coordinates::fromNormalizedLat(r.y1))
              + "\" />";
    resp += "</word_match>";
    resp += "</root>";
    Msg* rep = new Msg;
    encoder.build200Header(rep, "application/xml");
    encoder.addContent(rep,resp);
    return rep;
   }

   if(foundW)
   {
    std::string resp = "<root>";
    resp += "<word_match match_count=\"" + std::to_string(999) + "\" item_count=\"" + std::to_string(1) + "\" >";
        Rectangle r = eW.value.r;
        resp += "<url u=\"http://127.0.0.1:8081/svgMap.svg?longitude1=" + std::to_string(r.x0) + "&amp;lattitude1=" + std::to_string(r.y0) + "&amp;longitude2=" + std::to_string(r.x1) + "&amp;lattitude2=" + std::to_string(r.y1) + "\" />";
        resp += "<rectangle xo=\"" + std::to_string(Coordinates::fromNormalizedLon(r.x0))
              + "\" y0=\"" + std::to_string(Coordinates::fromNormalizedLat(r.y0))
              + "\" x1=\"" + std::to_string(Coordinates::fromNormalizedLon(r.x1))
              + "\" y1=\"" + std::to_string(Coordinates::fromNormalizedLat(r.y1))
              + "\" />";
    resp += "</word_match>";
    resp += "</root>";
    Msg* rep = new Msg;
    encoder.build200Header(rep, "application/xml");
    encoder.addContent(rep,resp);
    return rep;
   }

   if(foundN)
   {
    std::string resp = "<root>";
    resp += "<word_match match_count=\"" + std::to_string(999) + "\" item_count=\"" + std::to_string(1) + "\" >";
        Rectangle r = eN.value.r;
        resp += "<url u=\"http://127.0.0.1:8081/svgMap.svg?longitude1=" + std::to_string(r.x0) + "&amp;lattitude1=" + std::to_string(r.y0) + "&amp;longitude2=" + std::to_string(r.x1) + "&amp;lattitude2=" + std::to_string(r.y1) + "\" />";
        resp += "<rectangle xo=\"" + std::to_string(Coordinates::fromNormalizedLon(r.x0))
              + "\" y0=\"" + std::to_string(Coordinates::fromNormalizedLat(r.y0))
              + "\" x1=\"" + std::to_string(Coordinates::fromNormalizedLon(r.x1))
              + "\" y1=\"" + std::to_string(Coordinates::fromNormalizedLat(r.y1))
              + "\" />";
    resp += "</word_match>";
    resp += "</root>";
    Msg* rep = new Msg;
    encoder.build200Header(rep, "application/xml");
    encoder.addContent(rep,resp);
    return rep;
   }


*/


/*    std::stringstream my_stream(name);
    std::string word;
    const int level_count = 10;
    std::list<Rectangle> rectangles[level_count];
    int best_level = -1;
    fidx::Record<IndexEntry, uint64_t> record;
    while(std::getline(my_stream,word,' '))
    {
        std::cout << word << "\n";
        std::list<Rectangle> new_rectangles[level_count];
        for(int i = 0; i < level_count;i++) new_rectangles[i].clear();
        uint64_t key = fidx::makeLexicalKey(word.c_str(), word.length());
        uint64_t start;
        // search in node, way, relation text indexes
        if(mger.textIndexNode->findLastLesser(key, start))
        {
            start++;
            while((mger.textIndexNode->get(start, &record) && record.key == key))
            {
                new_rectangles[0].push_back(record.value.r);
                start++;
            }
        }
        if(mger.textIndexWay->findLastLesser(key, start))
        {
            start++;
            while((mger.textIndexWay->get(start, &record) && record.key == key))
            {
                new_rectangles[0].push_back(record.value.r);
                start++;
            }
        }
        if(mger.textIndexRelation->findLastLesser(key, start))
        {
            start++;
            while((mger.textIndexRelation->get(start, &record) && record.key == key))
            {
                new_rectangles[0].push_back(record.value.r);
                start++;
            }
        }
        
        std::cout << new_rectangles[0].size() << "\n";
        
        new_rectangles[0].sort();
        new_rectangles[0].unique();
        
        for(auto nr : new_rectangles[0])
        {
            for(int level = 0; level <= best_level && level < ( level_count - 1 ); level++)
            {
                for(auto r : rectangles[level])
                {
                    if((nr*r).isValid() &&(nr.isIn(r) || r.isIn(nr)))
                    {
                        new_rectangles[level+1].push_back(nr*r);
                    }
                }
            }
        }
        for(int i = 0; i < level_count; i++)
        {
            new_rectangles[i].sort();
            new_rectangles[i].unique();
        }
        best_level = -1;
        for(int level = 0; level < level_count ; level++)
        {
            rectangles[level].insert(rectangles[level].end(),new_rectangles[level].begin(),new_rectangles[level].end());
            new_rectangles[level].clear();
            if(rectangles[level].size()) best_level = level;
        }
        for(int i = 0; i < level_count; i++)
        {
            rectangles[i].sort();
            rectangles[i].unique();
        }
    }

    std::string resp = "<root>";
    for(int i = best_level; i >= best_level; i--)
    {
        if(rectangles[i].size())
        {
            bool first = true;
            resp += "<word_match match_count=\"" + std::to_string(i + 1) + "\" item_count=\"" + std::to_string(rectangles[i].size()) + "\" >";
            for(auto r : rectangles[i])
            {
                if (first)
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
            }
            resp += "</word_match>";
        }
    }*/
    std::string resp = "<root>";
    std::string word;
    
    std::stringstream my_stream(name);
    
    while(std::getline(my_stream,word,' '))
    {
        fidx::Record<IndexRange, uint64_t> eN;
        fidx::Record<IndexRange, uint64_t> eW;
        fidx::Record<IndexRange, uint64_t> eR;

        uint64_t k = fidx::makeLexicalKey(name.c_str(), name.length());


        bool foundN = mger.textIndexNodeRange->find(k, &eN);
        bool foundW = mger.textIndexWayRange->find(k, &eW);
        bool foundR = mger.textIndexRelationRange->find(k, &eR);
        
        if (foundN)
        {
			resp += "<node word=\"";
			resp += word + "\" count = \"" + std::to_string(eN.value.last - eN.value.first) + "\"";
			resp += "/>\n";
		}
        if (foundW)
        {
			resp += "<way word=\"";
			resp += word + "\" count = \"" + std::to_string(eW.value.last - eW.value.first) + "\"";
			resp += "/>\n";
		}
        if (foundR)
        {
			resp += "<rel word=\"";
			resp += word + "\" count = \"" + std::to_string(eR.value.last - eR.value.first) + "\"";
			resp += "/>\n";
		}
	}
    resp +=            "</root>";
    Msg* rep = new Msg;
    encoder.build200Header(rep, "application/xml");
    encoder.addContent(rep,resp);
    return rep;
}
