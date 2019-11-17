#ifndef DATAMANAGER_HPP
#define DATAMANAGER_HPP
#include <map>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "helpers/FileIndex.hpp"
#include "helpers/config.hpp"
#include "common/GeoTypes.hpp"
#include "GeoBox.hpp"
#include "helpers/Rectangle.hpp"

//int GeoIndexComp(const void* a, const void* b);
//bool GeoIndexFind(uint64_t key, uint64_t& result, GeoIndex firstItem[], uint64_t size);

class Balise;

struct StrBuf
{
    char Buffer[128];
    short count;
    StrBuf()
    {
        count = 0;
    }
    inline void appendChar(char c)
    {
        if(count < 128)
            Buffer[count++] = c;
    }
    inline std::string toString()
    {
        return std::string(Buffer, count);
    }
    inline void clear()
    {
        count = 0;
    }
};


struct Context
{
    std::vector<Balise*> tagStack;
    Balise* rootBalise;
    Balise* curBalise;
    int state, previous_state;
    StrBuf attrName, attrValue, baliseName;
};

class sequence
{
public:
    uint32_t bigChar[4];
    char c[17];
    int i, lastInfPos, lastSupPos;
    bool check(const char*s, short length);
    void dump();
    void append(uint32_t utf8Char);
    sequence()
    {
        memset(c,0,18);
        memset(bigChar,0,16);
    }
};

class DataManager
{
public :
    FILE* fIn;
    fidx::FileRawIndex<GeoIndex> *relationIndex;
    fidx::FileRawIndex<GeoWayIndex> *wayIndex;
    fidx::FileRawIndex<GeoPointIndex> *nodeIndex;
    fidx::FileRawData<GeoPoint> *wayPoints;
    std::vector<IndexDesc*>* indexes;
    Context context;
    void select(sequence& s, bool full=false);
    Balise* getParent();
    Balise* loadBalise(long start, long length);
    Balise* completeBalise(Balise* b);
    Balise* completeBalise(Balise* b, short recurs);
    Balise* loadBaliseFull(long start, long length);
    Balise* loadBaliseFull(long start, long length, short recurs, Balise* b);
    uint32_t fillPoints(Balise& b, GeoWayIndex idx);
    DataManager(std::string name,std::vector<IndexDesc*>* conf )
    {
        indexes = conf;
        fIn = fopen((name + "/data.xml").c_str(),"r");
        nodeIndex = new fidx::FileRawIndex<GeoPointIndex>((name + "/nodeIndex").c_str(),false);
        wayIndex = new fidx::FileRawIndex<GeoWayIndex>((name + "/wayIndex").c_str(),false);
        relationIndex = new fidx::FileRawIndex<GeoIndex>((name + "/relationIndex").c_str(),false);
        wayPoints = new fidx::FileRawData<GeoPoint>((name + "/wayPoints").c_str(),false);
    }
    virtual ~DataManager()
    {
        fclose(fIn);
        delete nodeIndex;
        delete relationIndex;
        delete wayIndex;
        delete wayPoints;
    }
};


class Balise
{
public:
    std::string baliseName;
    int startsAt,endsAt;
    bool isClosed;
    std::map<std::string,std::string> keyValues;
    std::vector<Balise*> childs;
    GeoPoint* points;
    uint64_t pointsCount;
    uint64_t firstNodeRef;
    uint64_t lastNodeRef;
    Rectangle rect;
    bool isLoaded, isFullyLoaded;
    void addChild(Balise* b)
    {
        childs.push_back(b);
    }
    Balise()
    {
        isLoaded = false;
        isFullyLoaded = false;
        baliseName="";
        firstNodeRef = lastNodeRef = UINT64_C(0xFFFFFFFFFFFFFFFF);
        isClosed = false;
        points = NULL;
        pointsCount = 0;
    }
    ~Balise()
    {
        keyValues.clear();
        if(points) free(points);
        for(Balise* b : childs) delete b;
        childs.clear();
    }
    std::string toString()
    {
        std::string result = baliseName + '[';
        std::map<std::string,std::string>::iterator attrIt;
        bool first = true;
        for (attrIt = keyValues.begin(); attrIt != keyValues.end(); attrIt ++)
        {
            result += attrIt->first + "=" + "\"" + attrIt->second + "\" ";
            if(! first) result += ',';
        }
        result += ']';
        return result;

    }

    std::string toStringWithPath(Context* context)
    {
        std::vector<Balise*>::iterator tagStackIt;
        bool first = true;
        std::string result = "";
        for (tagStackIt = context->tagStack.begin(); tagStackIt != context->tagStack.end(); tagStackIt ++)
        {
            Balise* b = (*tagStackIt);
            if(! first) result += '.';
            first = false;
            result += b->toString();
        }
        if(!first) result += '.';
        result += toString();
        return result;

    }
};

#endif
