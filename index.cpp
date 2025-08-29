#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdint.h>
#include <iostream>
#include "CompiledDataManager.hpp"
#include "GeoBox.hpp"
#include "common/constants.hpp"
#include "common/GeoTypes.hpp"
#include "Coordinates.hpp"
#include "helpers/config.hpp"
#include "helpers/Rectangle.hpp"

using namespace std;
using namespace fidx;

inline bool isName(const char* tag, const unsigned char tag_size, const char locales[32][2], const unsigned char nb_locales)
{
    if(tag_size == 4 && (strncmp(tag, "name", tag_size) == 0)) return true;
    else if(tag_size == 7)
    {
        if(tag[4] == ':')
        {
            char comp[7] = "name:";
            for(int i = 0; i < nb_locales; i++)
            {
                comp[5]=locales[i][0];
                comp[6]=locales[i][1];
                if(strncmp(tag, comp, 7) == 0) return true;
            } 
        }
    }
    return false;
}

int main(int argc, char *argv[])
{

    if(argc != 2)
    {
        std::cerr << "path argument is missing\n";
        exit(1);
    }


    std::vector<std::shared_ptr<IndexDesc>> indexes;

    XmlVisitor v(indexes, true, argv[1]);
    ParmsXmlVisitor params;

    std::string fileRoot = std::string(argv[1]) + "/";

    fidx::FileIndex<IndexEntry,uint64_t> textIndexNode(fileRoot + "textIndexNode"    , true);
    fidx::FileIndex<IndexEntry,uint64_t> textIndexWay(fileRoot + "textIndexWay"     , true);
    fidx::FileIndex<IndexEntry,uint64_t> textIndexRelation(fileRoot + "textIndexRelation", true);

    fidx::FileIndex<IndexRange,uint64_t> textIndexNodeRange(fileRoot + "textIndexNodeRange"    , true);
    fidx::FileIndex<IndexRange,uint64_t> textIndexWayRange(fileRoot + "textIndexWayRange"     , true);
    fidx::FileIndex<IndexRange,uint64_t> textIndexRelationRange(fileRoot + "textIndexRelationRange", true);

    std::string file = argv[1];
    file += "/config.xml";
    FILE* config = fopen(file.c_str(),"r");

    if(!config)
    {
        std::cout << "config.xml file not found, exiting !!!\n";
        exit(1);
    }

    XmlFileParser<XmlVisitor>::parseXmlFile(config,v);
    XmlFileParser<ParmsXmlVisitor>::parseXmlFile(config,params);
    fclose(config);
    CompiledDataManager mger(argv[1], &indexes);

    std::shared_ptr<Way> w = nullptr;
    std::shared_ptr<Point> p = nullptr;
    std::shared_ptr<Relation> r = nullptr;

    std::string locales_string  = params.getParam("locale");
    char locales[32][2];
    unsigned char nb_locales = 0;
    while(3*nb_locales < locales_string.size() && nb_locales < 32)
    {
        locales[nb_locales][0] = locales_string[3*nb_locales];
        locales[nb_locales][1] = locales_string[3*nb_locales + 1];
        nb_locales ++ ;
    }
    std::cout << "indexing names with locales : "<< locales_string <<"\n";
    for(uint64_t i=0; i < mger.relationIndex->getSize(); i++)
    {
        r = mger.loadRelation(i);
        if(!r) continue;

        if(r->tags.data == nullptr) {
            if ((i &  0xFFF) == 0)
                std::cout << "\rrelation " << i * 100.0 / mger.relationIndex->getSize() << "%  " << std::flush;
            continue;
        }

        uint64_t used = 0;

        while( used < r->tags.data_size)
        {

            char* tag = nullptr;
            char* value = nullptr;

            unsigned char tag_size = 0;
            unsigned char value_size = 0;



            tag_size = (unsigned char) r->tags.data[used];
            used++;
            tag = r->tags.data + used;
            used += tag_size;
            value_size =  r->tags.data[used];
            used++;
            value = r->tags.data+used;
            used += value_size;

            if(isName(tag,tag_size,locales,nb_locales))
            {
                std::string my_string(value, value_size);
                std::replace( my_string.begin(), my_string.end(), '-', ' ');
                stringstream my_stream(my_string);
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
                    textIndexRelation.append(fidx::makeLexicalKey(word.c_str(), word.length(), v.charconvs),{i, r->rect});
                }
            }

            for( auto d : indexes)
            {
                bool kept =false;
                uint32_t zmMask = 0;
                if(d->type == "relation")
                {
                    for (auto cd : d->conditions)
                    {
                        if ((tag_size == cd->tagKey.length()) && (strncmp(tag, cd->tagKey.c_str(), tag_size) == 0))
                        {
                            for(auto cl : cd->classes)
                            {
                                if (
                                   ((value_size == cl->tagValue.length()) && (strncmp(value, cl->tagValue.c_str(), value_size) == 0))
                                   || (cl->tagValue =="default"))
                                {
                                    bool closed = r->isClosed;
                                    if ((closed && cd->closed)||((!closed) && (cd->opened)))
                                    {
                                        kept = true;
                                        if(d->selectors.size() > 0) kept = false;
                                        for(auto sel : d->selectors)
                                        {
                                            kept = kept || (((sel->tagValue == "*")&&(r->tags[sel->tagKey.c_str()] !=""))||( r->tags[sel->tagKey.c_str()] == sel->tagValue ));
                                        }
                                        for(auto sel : d->excludeSelectors)
                                        {
                                            kept = kept && !(((sel->tagValue == "*")&&(r->tags[sel->tagKey.c_str()] !=""))||( r->tags[sel->tagKey.c_str()] == sel->tagValue ));
                                        }
                                        if(kept) zmMask = zmMask | cl->mask;
                                    }
                                }
                            }
                        }
                    }
                }
                if(kept)
                {
                    GeoBoxSet g = makeGeoBoxSet(r->rect);
                    for(int j = 0; j < g.count; j++)
                    {
                        if(r->isClosed) zmMask |= 0X100000LL;
                        //g.boxes[j].zmMask = zmMask;
                        d->idx->append(g.boxes[j],{i, r->rect, zmMask});
                    }
                }
            }
        }
        if ((i &  0xFF) == 0)
            std::cout << "\rrelation " << i * 100.0 / mger.relationIndex->getSize() << "%  " << std::flush;
    }

    for(uint64_t i=0; i < mger.wayIndex->getSize(); i++)
    {
        w = mger.loadWay(i);
        if(!w) continue;
        bool closed = false;
        if(w->pointsCount > 0)  closed = (w->points[0] == w->points[w->pointsCount -1]);
        if(w->tags.data == nullptr) continue;

        uint64_t used = 0;
        while( used < w->tags.data_size)
        {

            char* tag = nullptr;
            char* value = nullptr;

            unsigned char tag_size = 0;
            unsigned char value_size = 0;
            tag_size = (unsigned char) w->tags.data[used];
            used++;
            tag = w->tags.data + used;
            used += tag_size;
            value_size =  w->tags.data[used];
            used++;
            value = w->tags.data+used;
            used += value_size;

            if(isName(tag,tag_size,locales,nb_locales))
            {
                std::string my_string(value, value_size);
                std::replace( my_string.begin(), my_string.end(), '-', ' ');
                stringstream my_stream(my_string);
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
                    textIndexWay.append(fidx::makeLexicalKey(word.c_str(), word.length(), v.charconvs),{i, w->rect});
                }
            }

            for( auto d : indexes)
            {
                bool kept =false;
                uint32_t zmMask = 0;
                if(d->type == "way")
                {
                    for (auto cd : d->conditions)
                    {
                        if ((tag_size == cd->tagKey.length()) && (strncmp(tag, cd->tagKey.c_str(), tag_size) == 0))
                        {
                            for(auto cl : cd->classes)
                            {
                                if (
                                   ((value_size == cl->tagValue.length()) && (strncmp(value, cl->tagValue.c_str(), value_size) == 0))
                                   || (cl->tagValue =="default"))
                                {
                                    if ((closed && cd->closed)||((!closed) && (cd->opened)))
                                    {
                                        kept = true;
                                        if(d->selectors.size() > 0) kept = false;
                                        for(auto sel : d->selectors)
                                        {
                                            kept = kept || (((sel->tagValue == "*")&&(w->tags[sel->tagKey.c_str()] !=""))||( w->tags[sel->tagKey.c_str()] == sel->tagValue ));
                                        }
                                        for(auto sel : d->excludeSelectors)
                                        {
                                            kept = kept && !(((sel->tagValue == "*")&&(w->tags[sel->tagKey.c_str()] !=""))||( w->tags[sel->tagKey.c_str()] == sel->tagValue ));
                                        }
                                        if(kept) zmMask = zmMask | cl->mask;
                                    }
                                }
                            }
                        }
                    }
                }
                if(kept)
                {
                    GeoBoxSet g = makeGeoBoxSet(w->rect);
                    for(int j = 0; j < g.count; j++)
                    {
                        if(closed) zmMask |= 0X100000LL;
                        //g.boxes[j].zmMask = zmMask;
                        d->idx->append(g.boxes[j],{i, w->rect, zmMask});
                    }
                }
            }
        }
        if ((i &  0xFFFF) == 0)
            std::cout << "\rway " << i * 100.0 / mger.wayIndex->getSize() << "%  " << std::flush;
    }
    for(uint64_t i=0; i < mger.nodeIndex->getSize(); i++)
    {
        if ((i &  0xFFFFF) == 0)
            std::cout << "\rpoint " << i *100.0 / mger.nodeIndex->getSize() << "%  " << std::flush;

        p = mger.loadPoint(i);
        if(!p) continue;
        if(p->tags.data == nullptr) {continue; }

        uint64_t used = 0;

        while( used < p->tags.data_size)
        {

            char* tag = nullptr;
            char* value = nullptr;

            unsigned char tag_size = 0;
            unsigned char value_size = 0;

            tag_size = (unsigned char) p->tags.data[used];
            used++;
            tag = p->tags.data + used;
            used += tag_size;
            value_size =  p->tags.data[used];
            used++;
            value = p->tags.data+used;
            used += value_size;

            if(isName(tag,tag_size,locales,nb_locales))
            {
                std::string my_string(value, value_size);
                std::replace( my_string.begin(), my_string.end(), '-', ' ');
                stringstream my_stream(my_string);
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
                    textIndexNode.append(fidx::makeLexicalKey(word.c_str(), word.length(), v.charconvs),{i, {p->x, p->y, p->x, p->y}});
                }
            }

            for( auto d : indexes)
            {
                bool kept =false;
                uint32_t zmMask = 0;
                if(d->type == "node")
                {
                    for (auto cd : d->conditions)
                    {
                        if ((tag_size == cd->tagKey.length()) && (strncmp(tag, cd->tagKey.c_str(), tag_size) == 0))
                        {
                            for(auto cl : cd->classes)
                            {
                                if (
                                   ((value_size == cl->tagValue.length()) && (strncmp(value, cl->tagValue.c_str(), value_size) == 0))
                                   || (cl->tagValue =="default"))
                                {
                                        kept = true;
                                        if(d->selectors.size() > 0) kept = false;
                                        for(auto sel : d->selectors)
                                        {
                                            kept = kept || (((sel->tagValue == "*")&&(p->tags[sel->tagKey.c_str()] !=""))||( p->tags[sel->tagKey.c_str()] == sel->tagValue ));
                                        }
                                        for(auto sel : d->excludeSelectors)
                                        {
                                            kept = kept && !(((sel->tagValue == "*")&&(p->tags[sel->tagKey.c_str()] !=""))||( p->tags[sel->tagKey.c_str()] == sel->tagValue ));
                                        }
                                        if(kept) zmMask = zmMask | cl->mask;
                                }
                            }
                        }
                    }
                }
                if(kept)
                {
                    GeoBox g = makeGeoBox(p->x,p->y);
                    //g.zmMask = zmMask;
                    d->idx->append(g,{i,{p->x, p->y, p->x, p->y}, zmMask});
                }
            }
        }
    }
    for( auto d : indexes)
    {
        d->idx->flush();
        d->idx->sort();
    }
    textIndexNode.flush();
    textIndexNode.sort();
    textIndexWay.flush();
    textIndexWay.sort();
    textIndexRelation.flush();
    textIndexRelation.sort();

    uint64_t last_key = 0;
    uint64_t start_id = 0;
    uint64_t stop_id = 0;

    for(uint64_t i=0; i < textIndexNode.getSize(); i++)
    {
        Record<IndexEntry,uint64_t> rec;
        textIndexNode.get(i, &rec);
        if(i!=0 && last_key != rec.key)
        {
            stop_id = i - 1;
            textIndexNodeRange.append(last_key,{start_id, stop_id});
            start_id = i;
        }
        last_key = rec.key;
    }
    textIndexNodeRange.flush();

    last_key = 0;
    start_id = 0;
    stop_id = 0;

    for(uint64_t i=0; i < textIndexWay.getSize(); i++)
    {
        Record<IndexEntry,uint64_t> rec;
        textIndexWay.get(i, &rec);
        if(i!=0 && last_key != rec.key)
        {
            stop_id = i - 1;
            textIndexWayRange.append(last_key,{start_id, stop_id});
            start_id = i;
        }
        last_key = rec.key;
    }
    textIndexWayRange.flush();

    last_key = 0;
    start_id = 0;
    stop_id = 0;

    for(uint64_t i=0; i < textIndexRelation.getSize(); i++)
    {
        Record<IndexEntry,uint64_t> rec;
        textIndexRelation.get(i, &rec);
        if(i!=0 && last_key != rec.key)
        {
            stop_id = i - 1;
            textIndexRelationRange.append(last_key,{start_id, stop_id});
            start_id = i;
        }
        last_key = rec.key;
    }
    textIndexRelationRange.flush();
    //for (auto i : indexes) delete i;
    return 0;
}
