#ifndef SVGRENDERER_HPP
#define SVGRENDERER_HPP
#include <string>
#include <set>
#include <deque>
#include "../../helpers/hash.hpp"

#include "../../CompiledDataManager.hpp"

struct label_s
{
    uint64_t id;
	int zindex;
	std::string text;
    std::string ref;
	int style;
	int64_t pos_x, pos_y;
	double angle;
	unsigned int fontsize;
    //std::deque<GeoPoint> points;
};

class SvgRenderer
{
private:
    CompiledDataManager* mger;
    short zoomLevel;
    short zoom;
    uint32_t size_x, size_y;
    uint32_t zmMask;
    hh::THashIntegerTable relationHash, wayHash, nodeHash;
    short indexId;   
    std::map<int,std::string> resMap;
    std::map<int,std::string>::iterator it;
    std::string tmp = "";
    std::vector<label_s> label_vector;
    std::map<CssClass*, Shape*> shapes;

    
/*    inline uint32_t makeWayId() {
        return wayId++;
    }
    inline void resetWayId() {
        wayId = 0;
    }*/
public:
    SvgRenderer(CompiledDataManager* m) : relationHash(10000), wayHash(10000), nodeHash(1000)
    {
        mger = m;
        zoomLevel = -1;
    }
    SvgRenderer(CompiledDataManager* m, short z) : relationHash(10000), wayHash(10000), nodeHash(1000)
    {
        mger = m;
        zoomLevel = z;
    }
    
    std::string renderItems(Rectangle rect, uint32_t sizex, uint32_t sizey, std::string tag);
    std::string render(label_s& lbl, Way& myWay,     Rectangle rect, uint32_t sizex, uint32_t sizey, CssClass& cl, Shape& s);
    std::string render(label_s& lbl, Relation& myWay,Rectangle rect, uint32_t sizex, uint32_t sizey, CssClass& cl, Shape& s);
    std::string render(label_s& lbl, Point& myNode,  Rectangle rect,uint32_t  sizex, uint32_t sizey, CssClass& cl, Shape& s);
    std::string renderShape(Rectangle rect,uint32_t  sizex, uint32_t sizey, CssClass& cl, Shape& s);
    static CssClass* getCssClass(IndexDesc& idx, Relation& b, short zoom, bool closed);
    static CssClass* getCssClass(IndexDesc& idx, Point& b,    short zoom, bool closed);
    static CssClass* getCssClass(IndexDesc& idx, Way& b,      short zoom, bool closed);
    Shape* getShape(CssClass* cssClass);
    template<class ITEM> void iterate(IndexDesc& idxDesc, Rectangle r);
};

#endif
