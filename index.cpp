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



int main(int argc, char *argv[])
{
    
    if(argc != 2)
    {
        std::cerr << "path argument is missing\n";
        exit(1);
    }

    
    std::vector<IndexDesc*> indexes;

    XmlVisitor v(indexes, true, argv[1]);
    std::string file = argv[1];
    file += "/config.xml";
    FILE* config = fopen(file.c_str(),"r");
    XmlFileParser<XmlVisitor>::parseXmlFile(config,v);
    fclose(config);
    CompiledDataManager mger(argv[1], &indexes);

    Way* w = NULL;
    Point* p = NULL;
    Relation* r = NULL;

    for(uint64_t i=0; i < mger.relationIndex->getSize(); i++)
    {
        r = mger.loadRelation(i);
        if(!r) continue;

        if(r->tags.data == NULL) {
            delete r;
            if ((i &  0xF) == 0)
                std::cout << "\rrelation " << i * 100.0 / mger.relationIndex->getSize() << "%  " << std::flush;
            continue;
        }

        uint64_t used = 0;

        while( used < r->tags.data_size)
        {

            char* tag = NULL;
            char* value = NULL;

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
            for( IndexDesc *d : indexes)
            {
                bool kept =false;
                uint32_t zmMask = 0;
                if(d->type == "relation")
                {
                    for (Condition* cd : d->conditions)
                    {
                        if ((tag_size == cd->tagKey.length()) && (strncmp(tag, cd->tagKey.c_str(), tag_size) == 0))
                        {
                            for(CssClass* cl : cd->classes)
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
                                        for(Selector* sel : d->selectors)
                                        {
                                            kept = kept || (((sel->tagValue == "*")&&(r->tags[sel->tagKey.c_str()] !=""))||( r->tags[sel->tagKey.c_str()] == sel->tagValue ));
                                        }
                                        for(Selector* sel : d->excludeSelectors)
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
                        g.boxes[j].zmMask = zmMask;
                        d->idx->append(g.boxes[j],{i, r->rect});
                    }
                }
            }
        }
        if ((i &  0xF) == 0)
            std::cout << "\rrelation " << i * 100.0 / mger.relationIndex->getSize() << "%  " << std::flush;
        delete r;
    }

    for(uint64_t i=0; i < mger.wayIndex->getSize(); i++)
    {
        w = mger.loadWay(i);
        if(!w) continue;
        bool closed = false;
        if(w->pointsCount > 0)  closed = (w->points[0] == w->points[w->pointsCount -1]);
        if(w->tags.data == NULL) {delete w; continue;}

        uint64_t used = 0;
        while( used < w->tags.data_size)
        {

            char* tag = NULL;
            char* value = NULL;

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
            for( IndexDesc *d : indexes)
            {
                bool kept =false;
                uint32_t zmMask = 0;
                if(d->type == "way")
                {
                    for (Condition* cd : d->conditions)
                    {
                        if ((tag_size == cd->tagKey.length()) && (strncmp(tag, cd->tagKey.c_str(), tag_size) == 0))
                        {
                            for(CssClass* cl : cd->classes)
                            {
                                if (
                                   ((value_size == cl->tagValue.length()) && (strncmp(value, cl->tagValue.c_str(), value_size) == 0))
                                   || (cl->tagValue =="default"))
                                {
                                    if ((closed && cd->closed)||((!closed) && (cd->opened)))
                                    {
                                        kept = true;
                                        if(d->selectors.size() > 0) kept = false;
                                        for(Selector* sel : d->selectors)
                                        {
                                            kept = kept || (((sel->tagValue == "*")&&(w->tags[sel->tagKey.c_str()] !=""))||( w->tags[sel->tagKey.c_str()] == sel->tagValue ));
                                        }
                                        for(Selector* sel : d->excludeSelectors)
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
                        g.boxes[j].zmMask = zmMask;
                        d->idx->append(g.boxes[j],{i, w->rect});
                    }
                }
            }
        }
        if ((i &  0xFFF) == 0)
            std::cout << "\rway " << i * 100.0 / mger.wayIndex->getSize() << "%  " << std::flush;
        delete w;
    }
    for(uint64_t i=0; i < mger.nodeIndex->getSize(); i++)
    {
        if ((i &  0xFFFFF) == 0)
            std::cout << "\rpoint " << i *100.0 / mger.nodeIndex->getSize() << "%  " << std::flush;

        p = mger.loadPoint(i);
        if(!p) continue;
        if(p->tags.data == NULL) { delete p; continue; }

        uint64_t used = 0;

        while( used < p->tags.data_size)
        {

            char* tag = NULL;
            char* value = NULL;
    
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
            for( IndexDesc *d : indexes)
            {
                bool kept =false;
                uint32_t zmMask = 0;
                if(d->type == "node")
                {
                    for (Condition* cd : d->conditions)
                    {
                        if ((tag_size == cd->tagKey.length()) && (strncmp(tag, cd->tagKey.c_str(), tag_size) == 0))
                        {
                            for(CssClass* cl : cd->classes)
                            {
                                if (
                                   ((value_size == cl->tagValue.length()) && (strncmp(value, cl->tagValue.c_str(), value_size) == 0))
                                   || (cl->tagValue =="default"))
                                {
                                        kept = true;
                                        if(d->selectors.size() > 0) kept = false;
                                        for(Selector* sel : d->selectors)
                                        {
                                            kept = kept || (((sel->tagValue == "*")&&(p->tags[sel->tagKey.c_str()] !=""))||( p->tags[sel->tagKey.c_str()] == sel->tagValue ));
                                        }
                                        for(Selector* sel : d->excludeSelectors)
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
                    g.zmMask = zmMask;
                    d->idx->append(g,{i,{p->x, p->y, p->x, p->y}});
                }
            }
        }
        delete p;
    }
    for( IndexDesc *d : indexes)
    {
        d->idx->flush();
        d->idx->sort();
    }
    return 0;
}
