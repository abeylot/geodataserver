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
	label_s():id{0},zindex{0},text{""},ref{""},style{0},pos_x{0},pos_y{0},angle{0},fontsize{0}
	{}
	void clear()
	{
		id = 0;
		zindex = 0;
		text = "";
		ref = "";
		style = 0;
		pos_x = 0;
		pos_y = 0;
		angle = 0;
		fontsize = 0;
	}
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
    std::set<std::string> cssClasses;
    std::string _locale, _defaultColor;
    char _locales[32][2];
    unsigned char _nb_locales = 0; 
    

public:
    SvgRenderer(CompiledDataManager* m, std::string locale, std::string defaultColor) : relationHash(10000), wayHash(10000), nodeHash(1000), _locale(locale), _defaultColor(defaultColor)
    {
        mger = m;
        zoom = 0;
        size_x = 0;
        size_y = 0;
        zoomLevel = -1;
        zmMask = 0;
        indexId = 0;
        unsigned int i = 0;
        while(2*i < _locale.size())
        {
            _locales[i][0] = _locale[2*i];
            _locales[i][1] = _locale[2*i + 1];
            _nb_locales ++ ;
        }
    }
    SvgRenderer(CompiledDataManager* m, short z, std::string locale, std::string defaultColor) : relationHash(10000), wayHash(10000), nodeHash(1000), _locale(locale), _defaultColor(defaultColor)
    {
        zoom = 0;
        size_x = 0;
        size_y = 0;
        zmMask = 0;
        indexId = 0;
        
        mger = m;
        zoomLevel = z;
        unsigned int i = 0;
        while(3*i < _locale.size())
        {
            _locales[i][0] = _locale[3*i];
            _locales[i][1] = _locale[1*i + 1];
            _nb_locales ++ ;
            i++;
        }
    }
    
    std::string renderItems(Rectangle rect, uint32_t sizex, uint32_t sizey, std::string tag);
    std::string render(label_s& lbl, Way& myWay,     Rectangle rect, uint32_t sizex, uint32_t sizey, CssClass& cl, Shape& s);
    std::string render(label_s& lbl, Relation& myWay,Rectangle rect, uint32_t sizex, uint32_t sizey, CssClass& cl, Shape& s);
    std::string render(label_s& lbl, Point& myNode,  Rectangle rect,uint32_t  sizex, uint32_t sizey, CssClass& cl);
    std::string renderShape(Rectangle rect,uint32_t  sizex, uint32_t sizey, CssClass& cl, Shape& s);
    Shape* getShape(CssClass* cssClass);
    template<class ITEM> void iterate(const IndexDesc& idxDesc, const Rectangle& r);
    template<class ITEM> CssClass* getCssClass(const IndexDesc& idx, ITEM& b, short zoom, bool closed);
};

#endif
