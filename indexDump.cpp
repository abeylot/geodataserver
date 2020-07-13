//0123456789012345678901234/56789select
#define MAIN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdint.h>
#include <iostream>
#include "DataManager.hpp"
#include "GeoBox.hpp"
#include "common/constants.hpp"
#include "common/GeoTypes.hpp"
#include "Coordinates.hpp"
#include "helpers/config.hpp"

using namespace std;
using namespace fidx;


void measureBalise(Balise &b, uint32_t& minx, uint32_t& miny, uint32_t& maxx, uint32_t& maxy)
{
    if(b.baliseName == BALISENAME_NODESHORT)
    {
        uint32_t x = b.x0;
        uint32_t y = b.y0;
        if (x > maxx) maxx = x;
        if (x < minx) minx = x;
        if (y > maxy) maxy = y;
        if (y < miny) miny = y;
    }
    else
        for(Balise* c : b.childs)
        {
            measureBalise(*c, minx, miny, maxx, maxy);
        }

}


GeoBoxSet getGeoBoxSet(Balise& b)
{
    uint32_t xmin = UINT32_C(0xFFFFFFFF);
    uint32_t xmax = 0;
    uint32_t ymin = UINT32_C(0xFFFFFFFF);;
    uint32_t ymax = 0;
    measureBalise(b, xmin, ymin, xmax, ymax);
    return makeGeoBoxSet(xmin, ymin, xmax, ymax);
}


int main(int argc, char *argv[])
{
    std::vector<IndexDesc*> indexes;

    XmlVisitor v(indexes, true);
    FILE* config = fopen("config.xml","r");
    XmlFileParser<XmlVisitor>::parseXmlFile(config,v);

    DataManager mger(argv[1], &indexes);

    Balise* b;
    GeoBox ctx;
    for(uint64_t i=0; i < mger.relationIndex->getSize(); i++)
    {
        ctx.maskLength = -1;
        GeoIndex r;
        bool found = mger.relationIndex->get(i,&r);
        if(!found) continue;
        b = mger.loadBaliseFull(r.start, r.size);
        for(Balise* c : b->childs)
        {
            if (c->baliseName == BALISENAME_TAGSHORT)
            {
                for( IndexDesc *d : indexes)
                {
                    if(d->type == "relation")
                    {
                        for (Condition* cd : d->conditions)
                        {
                            if (c->keyValues["k"] == cd->tagKey)
                            {
                                for(CssClass* cl : cd->classes)
                                {
                                    //bool kept =false;
                                    if (cl->tagValue == c->keyValues["v"])
                                    {
                                        //kept = true;
                                        GeoBoxSet g = getGeoBoxSet(*b);
                                        for(int j = 0; j < g.count; j++)
                                        {
                                            d->idx->append(g.boxes[j],i);
                                            //std::cout << std::to_string(g.boxes[j].pos) << "::" << std::to_string(g.boxes[j].maskLength) << "\n";
                                        }
                                        break;
                                    }
                                }
                                //if(kept) std::cout << std::to_string(i)<< "\n" << std::flush;;
                            }
                        }
                    }
                }
            }
        }
        if ((i &  0xF) == 0)
            std::cout << "\rrelation " << i << '/' << mger.relationIndex->getSize() << "  " << std::flush;
        delete b;
    }
    //uint64_t count = 0;
    for(uint64_t i=0; i < mger.wayIndex->getSize(); i++)
    {
        //bool keepBuilding, keepClc;
        ctx.maskLength = -1;
        GeoIndex r;
        bool found = mger.wayIndex->get(i,&r);
        if(!found) continue;
        b = mger.loadBaliseFull(r.start, r.size);
        bool closed = b->isClosed;
        for(Balise* c : b->childs)
        {
            if (c->baliseName == BALISENAME_TAGSHORT)
            {
                for( IndexDesc *d : indexes)
                {
                    if(d->type == "way")
                    {
                        for (Condition* cd : d->conditions)
                        {
                            if ((c->keyValues["k"] == cd->tagKey)
                                    &&((closed && cd->closed)||((!closed) && !(cd->closed)))
                               )
                            {
                                //bool kept =false;
                                for(CssClass* cl : cd->classes)
                                {
                                    if (cl->tagValue ==c->keyValues["v"])
                                    {
                                        //kept = true;
                                        GeoBoxSet g = getGeoBoxSet(*b);
                                        for(int j = 0; j < g.count; j++)
                                        {
                                            d->idx->append(g.boxes[j],i);
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        delete b;
        if ((i &  0xFF) == 0)
            std::cout << "\rway " << i << '/' << mger.wayIndex->getSize() << "  " << std::flush;
    }
    for( IndexDesc *d : indexes) d->idx->flush();
    return 0;
}
