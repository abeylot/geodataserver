#ifndef SVGRENDERER_HPP
#define SVGRENDERER_HPP
#include <string>
#include <set>
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
};

class SvgRenderer
{
private:
    CompiledDataManager* mger;
    short zoomLevel;
    uint32_t wayId;
    inline uint32_t makeWayId() {
        return wayId++;
    }
    inline void resetWayId() {
        wayId = 0;
    }
public:
    SvgRenderer(CompiledDataManager* m)
    {
        mger = m;
        zoomLevel = -1;
        wayId = 1;
    }
    SvgRenderer(CompiledDataManager* m, short z)
    {
        mger = m;
        zoomLevel = z;
    }
    
    std::string renderItems(Rectangle rect, uint32_t sizex, uint32_t sizey, std::string tag);
    std::string render(int& zIndex, label_s& lbl, IndexDesc* idx, Way& myWay,     Rectangle rect, uint32_t sizex, uint32_t sizey, CssClass* cl);
    std::string render(int& zIndex, label_s& lbl, IndexDesc* idx, Relation& myWay,Rectangle rect, uint32_t sizex, uint32_t sizey, CssClass* cl);
    std::string render(int& zIndex, label_s& lbl, IndexDesc* idx, Point& myNode,  Rectangle rect,uint32_t  sizex, uint32_t sizey, CssClass* cl);
    static CssClass* getCssClass(IndexDesc& idx, Relation& b, short zoom, bool closed);
    static CssClass* getCssClass(IndexDesc& idx, Point& b,    short zoom, bool closed);
    static CssClass* getCssClass(IndexDesc& idx, Way& b,      short zoom, bool closed);
};

#endif
