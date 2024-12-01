#ifndef SVGRENDERER_HPP
#define SVGRENDERER_HPP
#include <string>
#include <set>
#include <deque>
#include <memory>
#include "../../helpers/hash.hpp"
#include "../../common/Projections.hpp"
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
    bool to_show;
    label_s():id{0},zindex{0},text{""},ref{""},style{0},pos_x{0},pos_y{0},angle{0},fontsize{0}, to_show(true)
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
        to_show = true;
    }
};

struct myShape
{
    std::shared_ptr<CssClass> c;
    char      layer;
    Shape     s;
};

inline double projectX(const Projection& p, const uint64_t size, const uint32_t left_bound, const uint32_t right_bound, const uint32_t x)
{
  double x_  = Coordinates::fromNormalizedLon(x);
  double x0_ = Coordinates::fromNormalizedLon(left_bound);
  double x1_ = Coordinates::fromNormalizedLon(right_bound);

  return size*(
     (p.lon2x(x_) - p.lon2x(x0_))/
     (p.lon2x(x1_) - p.lon2x(x0_))
  );
}

inline double projectY(const Projection& p, const uint64_t size, const uint32_t lower_bound, const uint32_t upper_bound, const uint32_t yt, std::unordered_map<uint32_t, double>& yProjectionCache )
{
    uint32_t y = yt | 0b111111;
    double result;
    auto it = yProjectionCache.find(y);
    if(it == yProjectionCache.end())
    {
        double y_  = Coordinates::fromNormalizedLat(y);
        double y0_ = Coordinates::fromNormalizedLat(lower_bound);
        double y1_ = Coordinates::fromNormalizedLat(upper_bound);

        result =  size*(
           (p.lat2y(y_) - p.lat2y(y0_))/
           (p.lat2y(y1_) - p.lat2y(y0_))
        );

        yProjectionCache[y] = result;
    }
    else
    {
        result = it->second;
    }
    return result;
}



class SvgRenderer
{
private:
    std::unordered_map<uint32_t, double> yProjectionCache;
    CompiledDataManager* mger;
    short zoomLevel;
    short zoom;
    uint32_t size_x, size_y;
    uint32_t zmMask;
    short indexId;
    std::map<long ,std::string> resMap;
    std::map<int,std::string>::iterator it;
    std::string tmp = "";
    std::vector<std::shared_ptr<label_s>> label_vector;
    std::map<uint64_t, std::shared_ptr<myShape>> shapes;
    std::set<std::string> cssClasses;
    std::string _locale, _defaultColor;
    char _locales[32][2];
    unsigned char _nb_locales = 0;
    WebMercatorProj _proj;

public:
    SvgRenderer(CompiledDataManager* m, const std::string& locale, const std::string& defaultColor) :  _locale(locale), _defaultColor(defaultColor)
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
    SvgRenderer(CompiledDataManager* m, short z, const std::string& locale, const std::string& defaultColor) : _locale(locale), _defaultColor(defaultColor)
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

    std::string renderItems(const Rectangle& rect, uint32_t sizex, uint32_t sizey, const std::string& tag);
    std::string render(label_s& lbl, Way& myWay,     Rectangle rect, uint32_t sizex, uint32_t sizey, CssClass& cl, Shape& s);
    std::string render(label_s& lbl, Relation& myRel,Rectangle rect, uint32_t sizex, uint32_t sizey, CssClass& cl, Shape& s);
    std::string render(label_s& lbl, Point& myNode,  Rectangle rect,uint32_t  sizex, uint32_t sizey, CssClass& cl);
    std::string renderShape(Rectangle rect,uint32_t  sizex, uint32_t sizey, CssClass& cl, Shape& s);
    Shape& getShape(std::shared_ptr<CssClass> c, unsigned char layer);
    template<class ITEM> void iterate(const IndexDesc& idxDesc, const Rectangle& r);
    template<class ITEM> std::shared_ptr<CssClass> getCssClass(const IndexDesc& idx, ITEM& b, short zoom, bool closed);
};

#endif
