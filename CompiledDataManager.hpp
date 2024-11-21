#ifndef COMPILEDDATAMANAGER_HPP
#define COMPILEDDATAMANAGER_HPP
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
#include <memory>

struct Tags
{
    char* data;
    uint64_t data_size;
    std::string_view operator [](std::string_view my_tag) const
    {
        if(data == nullptr) return "";
        uint64_t used = 0;
        uint64_t len = my_tag.size();

        while( used < data_size)
        {
            char* tag = nullptr;
            char* value = nullptr;

            unsigned char tag_size = 0;
            unsigned char value_size = 0;

            tag_size = (unsigned char) data[used];
            used++;
            tag = data + used;
            used += tag_size;
            value_size =  data[used];
            used++;
            value = data+used;
            used += value_size;
            if(tag_size == len)
            {
              if(strncmp(my_tag.data(), tag, tag_size) == 0)
              {
                  return std::string_view(value, value_size);
              }
            }
        }
    return "";
    }
};

inline int atoiSW(std::string_view s)
{
    int result = 0;
    for(char c : s)
    {
        result = result * 10 + (c - '0');
    }
    return result;
}

struct Point
{
    uint64_t id;
    char layer;
    uint32_t x;
    uint32_t y;
    Tags tags;
    virtual ~Point()
    {
        if(tags.data != nullptr) free(tags.data);
    }
};

struct Way
{
    uint64_t id;
    char layer;
    Rectangle rect;
    GeoPoint* points;
    uint64_t pointsCount;
    Tags tags;
    virtual ~Way()
    {
        free(points);
        if(tags.data != nullptr) free(tags.data);
    }
    void crop (Rectangle& r);
    void fillrec();
};



struct Line
{
    GeoPoint* points;
    uint64_t pointsCount;
    bool isClosed();
    bool closed;
    bool mergePoints(GeoPoint* points, uint64_t pointsCount);
    void crop (Rectangle& r);
    Line():points(nullptr),pointsCount(0),closed(false){};
    virtual ~Line()
    {
        free(points);
    }
};

struct Shape
{
    std::vector<Line*> openedLines;
    std::vector<Line*> closedLines;
    void mergePoints(GeoPoint* points, uint64_t pointsCount, bool closed);
    virtual ~Shape()
    {
        for(Line* l : openedLines)
        {
            delete l;
        }
        for(Line* l : closedLines)
        {
            delete l;
        }
    }
};

struct Relation
{
    uint64_t id;
    char layer;
    Rectangle rect;
    std::vector<std::shared_ptr<Relation>> relations;
    std::vector<std::shared_ptr<Way>> ways;
    std::vector<bool> reverseWay;
    std::vector<std::shared_ptr<Point>> points;
    Tags tags;
    bool isPyramidal;
    Shape shape;
    bool isClosed;
    virtual ~Relation()
    {
        if(tags.data != nullptr) free(tags.data);
    }
};

class CompiledDataManager
{
private:
    CompiledDataManager(const CompiledDataManager&);
    CompiledDataManager& operator= (CompiledDataManager&);
public :
    fidx::FileRawData<GeoPoint>*  wayPoints;
    fidx::FileRawData<GeoMember>* relMembers;
    fidx::FileRawVarData<GeoString>*  baliseTags;

    fidx::FileRawIndex<GeoIndex>      *relationIndex;
    fidx::FileRawIndex<GeoWayIndex>   *wayIndex;
    fidx::FileRawIndex<GeoPointIndex> *nodeIndex;


    fidx::FileIndex<IndexEntry,uint64_t> *textIndexNode;
    fidx::FileIndex<IndexEntry,uint64_t> *textIndexWay;
    fidx::FileIndex<IndexEntry,uint64_t> *textIndexRelation;

    fidx::FileIndex<IndexRange,uint64_t> *textIndexNodeRange;
    fidx::FileIndex<IndexRange,uint64_t> *textIndexWayRange;
    fidx::FileIndex<IndexRange,uint64_t> *textIndexRelationRange;

    std::vector<std::shared_ptr<IndexDesc>>* indexes;
    std::map<std::string, std::string>* symbols;
    std::map<std::string, std::string>* charconvs;
    //std::map<std::string, std::string>* patterns;

    std::string path;

    CompiledDataManager(const std::string& name,std::vector<std::shared_ptr<IndexDesc>>* conf,std::map<std::string, std::string>* symbs = nullptr, std::map<std::string, std::string>* convs = nullptr):path(name)
    {
        indexes = conf;
        symbols = symbs;
        charconvs = convs;
        //patterns = patts;
        nodeIndex = new fidx::FileRawIndex<GeoPointIndex>((name + "/nodeIndex").c_str(),false);
        wayIndex = new fidx::FileRawIndex<GeoWayIndex>((name + "/wayIndex").c_str(),false);
        relationIndex = new fidx::FileRawIndex<GeoIndex>((name + "/relationIndex").c_str(),false);

        wayPoints = new fidx::FileRawData<GeoPoint>((name + "/wayPoints").c_str(),false);
        relMembers= new fidx::FileRawData<GeoMember>((name + "/relMembers").c_str(),false);
        baliseTags = new fidx::FileRawVarData<GeoString>((name + "/baliseTags").c_str(),false);



        textIndexNode =     new fidx::FileIndex<IndexEntry,uint64_t> ((name + "/textIndexNode"    ).c_str(), false);
        textIndexWay =      new fidx::FileIndex<IndexEntry,uint64_t> ((name + "/textIndexWay"     ).c_str(), false);
        textIndexRelation = new fidx::FileIndex<IndexEntry,uint64_t> ((name + "/textIndexRelation").c_str(), false);

        textIndexNodeRange =     new fidx::FileIndex<IndexRange,uint64_t> ((name + "/textIndexNodeRange"    ).c_str(), false);
        textIndexWayRange =      new fidx::FileIndex<IndexRange,uint64_t> ((name + "/textIndexWayRange"     ).c_str(), false);
        textIndexRelationRange = new fidx::FileIndex<IndexRange,uint64_t> ((name + "/textIndexRelationRange").c_str(), false);


    }
    virtual ~CompiledDataManager()
    {
        delete nodeIndex;
        delete relationIndex;
        delete wayIndex;
        delete wayPoints;
        delete relMembers;
        delete baliseTags;
        delete textIndexNode;
        delete textIndexWay;
        delete textIndexRelation;
        delete textIndexNodeRange;
        delete textIndexWayRange;
        delete textIndexRelationRange;
    }

    std::shared_ptr<Way> loadWay(uint64_t id, bool fast = false);
    std::shared_ptr<Point> loadPoint(uint64_t id);
    //Relation* loadRelation(uint64_t id);
    std::shared_ptr<Relation> loadRelationFast(uint64_t id);
    std::shared_ptr<Relation> loadRelation(uint64_t id, short recurs = 2, bool fast = false);

    /*
    inline void load(std::shared_ptr<Relation> r, uint64_t id, bool fast)
    {
        r = loadRelation(id, 3, fast);
    }

    inline void load(std::shared_ptr<Way> w, uint64_t id, bool fast)
    {
        w = loadWay(id, fast);
    }

    inline void load(std::shared_ptr<Point> p, uint64_t id, [[maybe_unused]] bool fast)
    {
        p =  loadPoint(id);
    }
*/
    void fillPoints(GeoPoint ** points, uint64_t& pointsCount, uint64_t start, uint64_t size);

    void fillTags(Tags& tags, uint64_t start, uint64_t size);

    void fillLinkedItems(Relation& r, uint64_t start, uint64_t size, short recurs, bool fast);
        template<typename ITEM>

    std::shared_ptr<ITEM> load(uint64_t id, [[maybe_unused]] bool fast)
    {
        if constexpr(std::is_same<ITEM,Point>())
        {
            return loadPoint(id);
        }
        if constexpr(std::is_same<ITEM,Way>())
        {
            return loadWay(id, fast);
        }
        if constexpr(std::is_same<ITEM,Relation>())
        {
            return loadRelation(id, 3, fast);
        }
        return nullptr;
    }

};
#endif
