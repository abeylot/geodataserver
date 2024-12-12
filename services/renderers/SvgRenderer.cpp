#include "SvgRenderer.hpp"
#include "../../GeoBox.hpp"
#include "../../helpers/FileIndex.hpp"
#include "../../helpers/hash.hpp"
#include "../../common/constants.hpp"
#include "../../Coordinates.hpp"
#include "../../helpers/StringBuffer.hpp"
#include <map>
#include <set>
#include <math.h>
#include <sstream>

#define LAYER_MULT 100000

Shape& SvgRenderer::getShape(std::shared_ptr<CssClass> c, unsigned char layer)
{
    uint64_t id = (c->rank << 8) + layer;
    auto it = shapes.find(id);
    if(it == shapes.end())
    {
        shapes[id] = std::make_shared<myShape>();
        shapes[id]->c = c;
        shapes[id]->layer = layer;
        return (shapes[id]->s);
    }
    else
    {
        return (it->second->s);
    }

}

bool compare(const std::shared_ptr<label_s>& l2, const std::shared_ptr<label_s>& l1)
{
    if(l1->zindex > l2->zindex) return false;
    if(l2->zindex > l1->zindex) return true;

    if(l1->style > l2->style) return true;
    if(l2->style > l1->style) return false;

    //if (l1.text.length() < l2.text.length()) return true;
    //if (l1.text.length() > l2.text.length()) return false;

    if (l1->pos_x > l2->pos_x) return true;
    if (l1->pos_x < l2->pos_x) return false;

    if (l1->pos_y > l2->pos_y) return true;
    if (l1->pos_y < l2->pos_y) return false;

    if (l1->text > l2->text) return true;
    if (l1->text < l2->text) return false;
    return false;
}

#define MAX_TEXT_LEN 18
#define MIN_TEXT_LEN 6

std::string cutString(std::string text, int x, int y, int dy)
{
    bool first = true;
    if(text.length() < MAX_TEXT_LEN) return text;
    std::string result;
    int iline = 0;
    int ilines = 0;
    std::string txt = text;
    while(true)
    {
        ilines++;
        if(txt.length() < MAX_TEXT_LEN )
        {
            break;
        }
        size_t pos = txt.find(' ', MIN_TEXT_LEN);
        if(pos == std::string::npos)
            pos = txt.find('/', MIN_TEXT_LEN);
        if(pos == std::string::npos)
            pos = txt.find('-', MIN_TEXT_LEN);
        if(pos == std::string::npos)
        {
            pos = MAX_TEXT_LEN;
        }
        else pos++;
        txt = txt.substr(pos);
    }

    int y0 = y - (dy*(ilines - 1)) / 2;

    while(true)
    {
        if(text.length() < MAX_TEXT_LEN )
        {
            return result + "</tspan><tspan  x=\""+std::to_string(x)+"\" y=\""+std::to_string(y0 + dy*iline)+"\">"+text+ "</tspan>";
        }
        if(!first)
        {
            result += "</tspan>";
        }
        result += "<tspan  x=\""+std::to_string(x)+"\" y=\""+std::to_string(y0 + dy*iline )+"\">";
        size_t pos = text.find(' ', MIN_TEXT_LEN);
        if(pos == std::string::npos)
            pos = text.find('/', MIN_TEXT_LEN);
        if(pos == std::string::npos)
            pos = text.find('-', MIN_TEXT_LEN);
        if(pos != std::string::npos)
        {
            result += text.substr(0,pos);
            text = text.substr(pos+1);
        }
        else
        {
            result += text + "</tspan>";
            return result;
        }
        first = false;
        iline++;
    }
}

size_t cutString(std::string txt, int& ilines)
{
    ilines = 1;
    if(txt.length() < MAX_TEXT_LEN) return txt.length();
    size_t result = 0;
    while(true)
    {
        if(txt.length() < MAX_TEXT_LEN )
        {
            break;
        }
        ilines++;
        size_t pos = txt.find(' ', MIN_TEXT_LEN);
        if(pos == std::string::npos)
            pos = txt.find('/', MIN_TEXT_LEN);
        if(pos == std::string::npos)
            pos = txt.find('-', MIN_TEXT_LEN);
        if(pos == std::string::npos)
        {
            pos = MAX_TEXT_LEN;
        }
        if((pos) > result) result = pos;
        if(pos != MAX_TEXT_LEN) pos++;
        txt = txt.substr(pos);
    }
    return result;
}




template<class ITEM> void SvgRenderer::iterate(const IndexDesc& idxDesc, const Rectangle& rect)
{
    GeoBoxSet gSet;
    Shape myShape;
    hh::THashIntegerTable hash(10000);

    //std::map <uint64_t, std::pair<std::shared_ptr<CssClass>, std::shared_ptr<ITEM>>> itemsToDraw;
    std::vector <std::pair<std::shared_ptr<CssClass>, std::shared_ptr<ITEM>>> itemsToDraw;

    Rectangle rect2;

    static_assert(std::is_same<ITEM,Relation>() ||
                  std::is_same<ITEM,Way>() ||
                  std::is_same<ITEM,Point>(), " Type unsuported for template method");

    if constexpr(std::is_same<ITEM,Relation>())
    {
         gSet = makeGeoBoxSet(rect*1.3);
         rect2 = rect*1.3;
    }
    if constexpr(std::is_same<ITEM,Way>())
    {
         gSet = makeGeoBoxSet(rect*1.3);
         rect2 = rect*1.3;
    }
    if constexpr(std::is_same<ITEM,Point>())
    {
         gSet = makeGeoBoxSet(rect*3);
         rect2 = rect*3;
    }

    std::set<uint64_t> done_geoboxes;
    //bool get_range(KEY key_min, KEY key_max, std::vector<KEY>& keys, std::vector<ITEM>& items)
    for(short i = 0; i < gSet.count; i++)
    {
        std::vector<IndexEntryMasked> indexEntry;
        GeoBox g;
        g = gSet.boxes[i];
        short  mask = g.get_maskLength();

        GeoBox maxGeoBox;
        maxGeoBox = g;

        if(mask < 63) {
            uint64_t myMask = UINT64_C(0x1) << (mask);
            maxGeoBox.set_pos(maxGeoBox.get_pos() + myMask);
        } else {
            maxGeoBox.set_pos(0xFFFFFFFFFFFFFFFF);
        }

        //fidx::Record<IndexEntryMasked, GeoBox> record;
        //uint64_t start;
        //if(idxDesc.idx->findLastLesser(g, start))
        if(idxDesc.idx->get_range(g, maxGeoBox, indexEntry))
        for(size_t i = 0; i < indexEntry.size(); i++)
        {
            if((indexEntry[i].zmMask &  zmMask )&&((indexEntry[i].r * (rect2)).isValid()))
            {
                if( hash.addIfUnique(indexEntry[i].id))
                {
                    std::shared_ptr<ITEM> item = nullptr;
                    if constexpr(std::is_same<ITEM,Relation>())
                    {
                        item = mger->loadRelationFast(indexEntry[i].id);
                    }
                    else
                    {
                        item = mger->load<ITEM>(indexEntry[i].id);
                    }
                    std::shared_ptr<CssClass> cl = getCssClass(idxDesc, *item, zoom, indexEntry[i].zmMask & 0X100000LL);

                    if constexpr(std::is_same<ITEM,Relation>())
                    {
                        if(cl && cl->opened) // not a filled area
                        {
                            item = mger->loadRelation(indexEntry[i].id, 2, false, &rect2);
                        }
                        else
                        {
                            item = mger->loadRelation(indexEntry[i].id, 2, true);
                        }
                    }
                    //label_s lbl;
                    if(cl)
                    {
                        if constexpr(! std::is_same<ITEM, Point>())
                        {
                            item->rect = indexEntry[i].r;
                        }
                        //itemsToDraw[indexEntry[i].id] = std::make_pair(cl, item);
                        itemsToDraw.push_back(std::make_pair(cl, item));
                    }
                }
            }
            //start++;
        }
        mask = g.get_maskLength();
        short max = 64;
        while(mask++ < max )
        {
            GeoBox maxGeoBox2 = g;
            maxGeoBox2.set_maskLength(mask);
            uint64_t myMask = UINT64_C(0xFFFFFFFFFFFFFFFF) << mask;
            maxGeoBox2.set_pos(maxGeoBox2.get_pos() & myMask);
            if(std::find(done_geoboxes.begin(), done_geoboxes.end(), maxGeoBox2.get_hash()) != done_geoboxes.end())
            {
                continue; // dont do twice the same job
            }
            done_geoboxes.insert(maxGeoBox2.get_hash());
    //        if(idxDesc.idx->findLastLesser(maxGeoBox2, start))
            if(idxDesc.idx->get_range(maxGeoBox2, maxGeoBox2, indexEntry))
            for(size_t i = 0; i < indexEntry.size(); i++)
            {
                if((indexEntry[i].zmMask &  zmMask )&&((indexEntry[i].r * (rect2)).isValid()))
                {
                    if( hash.addIfUnique(indexEntry[i].id))
                    {
                        std::shared_ptr<ITEM> item = nullptr;
                        if constexpr(std::is_same<ITEM,Relation>())
                        {
                           item = mger->loadRelationFast(indexEntry[i].id);
                        }
                        else
                        {
                            item = mger->load<ITEM>(indexEntry[i].id);
                        }
                        std::shared_ptr<CssClass> cl = getCssClass(idxDesc, *item, zoom, indexEntry[i].zmMask & 0X100000LL);
                        //std::shared_ptr<label_s> lbl;
                        if constexpr(std::is_same<ITEM,Relation>())
                        {
                            if(cl && cl->opened) // not a filled area
                            {
                                item = mger->loadRelation(indexEntry[i].id, 2, false, &rect2);
                            }
                            else
                            {
                                item = mger->loadRelation(indexEntry[i].id, 2, true);
                            }
                        }
                        if(cl)
                        {
                            if constexpr(! std::is_same<ITEM, Point>())
                            {
                                item->rect = indexEntry[i].r;
                            }
                            //itemsToDraw[indexEntry[i].id] = std::make_pair(cl, item);
                            itemsToDraw.push_back(std::make_pair(cl, item));
                        }
                    }
                }
            }
        }
    }
    for (auto value : itemsToDraw)
    {
        std::shared_ptr<label_s> lbl = std::make_shared<label_s>();
        std::shared_ptr<CssClass> cl = value.first;
        std::shared_ptr<ITEM> item = value.second;
        if constexpr(! std::is_same<ITEM, Point>())
        {
            tmp = render(*lbl, *item,
                         rect,
                         size_x,
                         size_y,
                         *cl,
                         getShape(cl, item->layer)
                         );
        }
        else
        {
            tmp = render(*lbl, *item,
                         rect,
                         size_x,
                         size_y,
                         *cl
                         );
        }
        auto it = resMap.find(value.first->zIndex + value.second->layer * LAYER_MULT);
        if(it != resMap.end())
        {
            it->second += tmp;
        }
        else
        {
            resMap[value.first->zIndex + value.second->layer * LAYER_MULT] = tmp;
        }
           if((lbl->text.length() > 0) && (lbl->fontsize > 5))
               label_vector.push_back(lbl);
    }
    if constexpr(! std::is_same<ITEM, Point>())
    {
        for(auto it : shapes)
        {
            tmp = renderShape(
            rect,
            size_x,
            size_y,
            *(it.second->c),
            it.second->s
            );
            auto it2 = resMap.find(it.second->c->zIndex + it.second->layer * LAYER_MULT);
            if(it2 != resMap.end())
            {
                it2->second = tmp + it2->second;
            }
            else
            {
                resMap[it.second->c->zIndex + it.second->layer * LAYER_MULT] = tmp;
            }
        }
        shapes.clear();
    }
}


std::string SvgRenderer::renderItems(const Rectangle& rect, uint32_t sizex, uint32_t sizey, const std::string& tag)
{
    cssClasses.clear();
    size_x = sizex;
    size_y = sizey;
    std::string libs = "";
    double ppm = 50 * ((sizex * 1.0) / ((1.0)*(rect.x1 - rect.x0)));
    uint32_t msz = rect.x1 - rect.x0;
    zoom = 31;
    msz = msz >> 1;
    while(msz)
    {
        zoom --;
        msz = msz >> 1;
    }
    msz = sizey;
    while(msz > 256)
    {
        zoom ++;
        msz = msz >> 1;
    }
    if (zoom > 19) zoom = 19;

    if(zoomLevel > 0) zoom = zoomLevel;

    //std::cout << "zoom;" << zoom << "\n";
    zmMask = 1LL << zoom;

    std::string resultString;
    std::string textsString;

    StringBuffer result(resultString);
    StringBuffer texts(textsString);

    result << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?> <!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"  \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
    result << "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\""<<size_x<<"\" height=\""<<size_y<<"\" viewBox=\"0 0 " << sizex << " " << sizey << "\">\n";

    indexId = 0;
    for (auto idxDesc : *(mger->indexes))
    {
        indexId++;
        if(!(idxDesc->mask & zmMask)) continue;
        if (idxDesc->type == "node")
            iterate<Point>(*idxDesc, rect);
        else if (idxDesc->type == "way")
            iterate<Way>(*idxDesc, rect);
        else if (idxDesc->type == "relation")
            iterate<Relation>(*idxDesc, rect);
    }

    std::sort(label_vector.begin(), label_vector.end(),compare);

    std::vector<std::shared_ptr<label_s>> to_print;


    for(auto t=label_vector.begin(); t!=label_vector.end(); ++t)
    {
        bool to_show = true;

        //double xc,xd,yc,yd;
        int ilines;
        int ilt = cutString((*t)->text, ilines);
        double lt = (*t)->fontsize*1.0*ilt;
        double ht = (*t)->fontsize*1.0*ilines;


        for(auto v = to_print.begin(); v != to_print.end(); ++v)
        {
            int ilv = cutString((*v)->text, ilines);

            double lv = (*v)->fontsize*1.0*ilv;
            double hv = (*v)->fontsize*1.0*ilines;

            double dx = (double)((*t)->pos_x) - (double)((*v)->pos_x);
            double dy = (double)((*t)->pos_y) - (double)((*v)->pos_y);

            if((lt*lt + ht*ht + lv*lv + hv*hv) > 4*(dx*dx + dy*dy))
            {
                    to_show = false;
                    break;
            }
        }
        if(to_show )
        {
            to_print.push_back(*t);
            (*t)->to_show = true;
        }
        else (*t)->to_show = false;
    }

    for(auto v=to_print.begin(); v!=to_print.end(); ++v)
    {
        if((*v)->pos_x < -0.5*size_x) continue;
        if((*v)->pos_x > 1.5*size_x) continue;
        if((*v)->pos_y < -0.5* size_y) continue;
        if((*v)->pos_y > 1.5*size_y) continue;
        {

               if((*v)->angle ==0)
               {
                   if((*v)->sizeFromStyle)
                   {
                       texts << "<text  class=\"c"
                          << (*v)->style
                          << "\" x=\"" << (*v)->pos_x
                          << "\" y=\"" << (*v)->pos_y
                          << "\">"
                          << cutString((*v)->text, (*v)->pos_x, (*v)->pos_y, (*v)->fontsize)
                          << "</text>\n";
                   }
                   else
                   {
                       texts << "<text  class=\"c"
                          << (*v)->style
                          << "\" style=\"font-size:"
                          << (*v)->fontsize
                          << "px\" x=\"" << (*v)->pos_x
                          << "\" y=\"" << (*v)->pos_y
                          << "\">"
                          << cutString((*v)->text, (*v)->pos_x, (*v)->pos_y, (*v)->fontsize)
                          << "</text>\n";
                    }
               }
               else
               {
                   if((*v)->sizeFromStyle)
                   {
                       texts << "<text  class=\"c"
                          << (*v)->style
                          << "\" x=\"" << (*v)->pos_x
                          << "\" y=\"" << (*v)->pos_y
                          << "\" transform=\"rotate("
                          << std::to_string((*v)->angle*180/M_PI)
                          << ","
                          << (*v)->pos_x
                          << ","
                          << (*v)->pos_y
                          << ")\">"
                          << cutString((*v)->text, (*v)->pos_x, (*v)->pos_y, (*v)->fontsize)
                          << "</text>\n";
                   }
                   else
                   {
                       texts << "<text  class=\"c"
                          << (*v)->style
                          << "\" style=\"font-size:"
                          << (*v)->fontsize
                          << "px\" x=\"" << (*v)->pos_x
                          << "\" y=\"" << (*v)->pos_y
                          << "\" transform=\"rotate("
                          << std::to_string((*v)->angle*180/M_PI)
                          << ","
                          << (*v)->pos_x
                          << ","
                          << (*v)->pos_y
                          << ")\">"
                          << cutString((*v)->text, (*v)->pos_x, (*v)->pos_y, (*v)->fontsize)
                          << "</text>\n";
                   }
               }
               cssClasses.insert("c"+std::to_string((*v)->style));

        }
    }


    result << "<defs>";

    std::set<std::string> symbols_set;

    for (auto idxDesc : *(mger->indexes))
    {
        for (auto cd : idxDesc->conditions)
        {
            for(auto cl : cd->classes)
            {
                if( cssClasses.find("sym#"+cl->symbol) != cssClasses.end())
                {
                    if (cl->symbol != "")
                    {
                        if(mger->symbols->find(cl->symbol) != mger->symbols->end())
                        {
                            symbols_set.insert(cl->symbol);
                        }
                    }
                }
            }
        }
    }

    for( auto pattern : *(mger->symbols))
    {
        bool found = false;
        for (auto idxDesc : *(mger->indexes))
        {
            for (auto cd : idxDesc->conditions)
            {
                for(auto cl : cd->classes)
                {
                    if( cssClasses.find("c"+std::to_string(cl->rank)) !=  cssClasses.end() )
                    {
                        std::string style = cl->makeClass("c" + std::to_string(cl->rank), ppm, idxDesc->type == "relation");
                        if(style.find("url(#"+pattern.first) != std::string::npos)
                        {
                            found = true;
                        }
                    }
                    if(found) break;
                }
                if(found) break;
            }
            if(found) break;
        }
        if(!found)
        {
             for(auto s : symbols_set)
             {
                 if((*(mger->symbols))[s].find("url(#"+pattern.first) != std::string::npos)
                 {
                     found = true;
                     break;
                 }
             }
        }
        if(found) result << pattern.second;
    }

    result << "</defs>";

    result << "<style>\ntext,tspan{dominant-baseline:central;text-anchor:middle;} path{fill:none;}";


    for (auto idxDesc : *(mger->indexes))
    {
        for (auto cd : idxDesc->conditions)
        {
            for(auto cl : cd->classes)
            {
                if( cssClasses.find("c"+std::to_string(cl->rank)) !=  cssClasses.end() )
                {
                    result << cl->makeClass("c" + std::to_string(cl->rank), ppm, idxDesc->type == "relation");
                }
            }
        }
    }
    result << "</style>\n";

    for( auto symbols : *(mger->symbols))
    {
        if(symbols_set.find(symbols.first) != symbols_set.end())
        {
            result << symbols.second;
        }
    }


    result << "<rect width=\"" << (sizex + 1) << "\" height=\"" << (sizey + 1) << "\" fill=\""+_defaultColor+"\" opacity=\"0.5\"/>\n";


    for(std::pair<int,std::string> tmp : resMap)
    {
        result << tmp.second;
    }
    texts.flush();
    result << textsString;
    if(tag != "") result << "<text style=\"font-size:10px\" x=\"0\" y=\"15\" >"
                         << tag
                         << "</text>\n";
    result << "</svg>";
    result.flush();
    return resultString;
}

std::string SvgRenderer::renderShape(Rectangle rect,uint32_t szx, uint32_t szy, CssClass& cl, Shape& s)
{
    std::string resultString;
    StringBuffer result(resultString);
    double oldx = -1;
    double oldy = -1;

    int width=0;

    double ppm = 50 * ((szx * 1.0) / ((1.0)*(rect.x1 - rect.x0)));

    if(cl.width.length()) width = std::stoi(cl.width);
    if(width && ((width*ppm) <  0.25)) return "";


    if (s.openedLines.empty() && s.closedLines.empty() ) return "";
    result << "<path  d=\"";
    for(Line* l : s.openedLines)
    {
        if(l->pointsCount < 2) continue;
        bool first = true;
        int32_t x=0;
        int32_t y=0;
        x=0; y=0;
        for(unsigned int i = 0 ; i < l->pointsCount; i++)
        {
            int64_t xx = l->points[i].x;
            int64_t yy = l->points[i].y;
            oldx = x;
            oldy = y;
            x = round(projectX(_proj, szx, rect.x0, rect.x1, xx));
            y = round(projectY(_proj, szy, rect.y0, rect.y1, yy, yProjectionCache));
            {
                if(first)
                {
                    result << "M" << x << " " << y;
                    first = false;
                }
                else
                {
                    if((x != oldx) || (y != oldy))
                    {
                        if(x == oldx)  result << "V" << y ;
                        else if(y == oldy) result << "H" << x ;
                        else result << "L" << x << " " << y;
                    }
                }
            }
        }
    }
    for(Line* l : s.closedLines)
    {
        bool first = true;
        int32_t x=0;
        int32_t y=0;
        x=0; y=0;
        if(l->pointsCount < 2) continue;
        for(unsigned int i = 0 ; i < l->pointsCount; i++)
        {
            int64_t xx = l->points[i].x;
            int64_t yy = l->points[i].y;
            oldx = x;
            oldy = y;
            x = round(projectX(_proj, szx, rect.x0, rect.x1, xx));
            y = round(projectY(_proj, szy, rect.y0, rect.y1, yy, yProjectionCache));
            {
                if(first)
                {
                    result << "M" << x << " " << y;
                    first = false;
                }
                else
                {
                    if((x != oldx) || (y != oldy))
                    {
                        if(x == oldx) result << "V" << y ;
                        else if(y == oldy) result << "H" << x ;
                        else result << "L" << x << " " << y ;
                    }
                }
            }
        }
    }
    result << "\" class=\"c" << cl.rank << "\"/>\n";
    cssClasses.insert("c"+std::to_string(cl.rank));
    result.flush();
    return resultString;
}


std::string SvgRenderer::render(label_s& lbl, Way& myWay, Rectangle rect,uint32_t szx, uint32_t szy, CssClass& cl, Shape& s)
{
    Rectangle r1 = rect*1.25;
    lbl.id = myWay.id + UINT64_C(0xA000000000000000);
    lbl.fontsize = 12;
    std::string name = "";
    std::string inherited_name = "";
    if(!lbl.text.empty())
    {
        inherited_name = lbl.text;//when called by relation render
        //std::cout << inherited_name << "\n";
    }
    lbl.text = "";
    lbl.ref = "";
    lbl.pos_x = lbl.angle = lbl.pos_y=0;
    lbl.style = 0;
    lbl.zindex = cl.textZIndex;
    std::string resultString;
    StringBuffer result(resultString);

    double symb_angle = 0;
    int64_t symb_x = 0;
    int64_t symb_y = 0;

    int32_t oldx = -1;
    int32_t oldy = -1;
    int32_t x=0;
    int32_t y=0;
    std::string color = "red";

    int width=0;
    int textWidth=0;
    double ppm = 50 * ((szx * 1.0) / ((1.0)*(rect.x1 - rect.x0)));

    bool draw = ((myWay.rect)*r1).isValid();
    std::string style = cl.style;
    std::string textStyle2;
    std::string textStyle = cl.textStyle;
    if(cl.width.length()) width = std::stoi(cl.width);
    if(cl.textWidth.length()) textWidth = std::stoi(cl.textWidth);
    if(textWidth) textStyle ="font-size:"+ std::to_string((int)(textWidth*ppm))+ "px;" + cl.textStyle;
    if(width && ((width*ppm) <  0.25)) return "";
    if(width) style ="stroke-width:"+ std::to_string(width*ppm)+";" + cl.style;
    std::string textField = "name";
    if(cl.textField != "") textField = cl.textField;
    if(textField != "name" && name.empty())
    {
        name = std::string(myWay.tags[textField]);
    }
    if((name == "") || (textField == "name") )
    {
        for ( unsigned int i = 0 ; i < _nb_locales; i++)
        {
            std::string tmp = std::string("name:") + std::string(_locales[i],2);
            name = std::string(myWay.tags[tmp]);
            if (name != "") break;

        }
        if(name == "") name = std::string(myWay.tags["name"]);
    }
    if(inherited_name != "") {
        if(inherited_name == "void") name ="";
        else name = inherited_name;
        //std::cout << inherited_name << "\n";
    }

    bool first = true;
    double length = 0;
    double halfLength = 0;
    for(unsigned int i = 0 ; i < myWay.pointsCount; i++)
    {
        int64_t xx = myWay.points[i].x;
        int64_t yy = myWay.points[i].y;
        oldx = x;
        oldy = y;
        x = round(projectX(_proj, szx, rect.x0, rect.x1, xx));
        y = round(projectY(_proj, szy, rect.y0, rect.y1, yy, yProjectionCache));
        {
            if(first)
            {
                first = false;
            }
            else
            {
                if((x != oldx) || (y != oldy)|| i == (myWay.pointsCount - 1)/*&&(x > -1*szx)&&(x < 2*szx)&&(y > -1*szy)&&(y < 2*szy)*/)
                length += sqrt((x-oldx)*(x-oldx) + (y-oldy)*(y-oldy));
            }
        }
    }
    if(cl.opened)
    {

        halfLength = length / 2;
        double curLength = 0;
        //double ratio = 0;
        first = true;

        for(unsigned int i = 0 ; i < myWay.pointsCount; i++)
        {
            int64_t xx = myWay.points[i].x;
            int64_t yy = myWay.points[i].y;
            oldx = x;
            oldy = y;
            //x = (xx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
            //y = (yy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
            x = round(projectX(_proj, szx, rect.x0, rect.x1, xx));
            y = round(projectY(_proj, szy, rect.y0, rect.y1, yy, yProjectionCache));

            {
                if(!first)
                {
                    //double oldLength = curLength;
                    curLength += sqrt((x-oldx)*(x-oldx) + (y-oldy)*(y-oldy));
                    if(curLength > halfLength)
                    {
                        //ratio = (halfLength - oldLength)/(curLength - oldLength);
                        lbl.pos_x = (x + oldx) / 2;
                        lbl.pos_y = (y + oldy) / 2;;
                        double dfx = x - oldx;
                        double dfy = y - oldy;
                        if(dfx == 0) lbl.angle = M_PI / 2;
                        else lbl.angle = atan2(dfy , dfx);
                        if(lbl.angle > M_PI / 2) lbl.angle -= M_PI;
                        if(lbl.angle < -1 * M_PI / 2) lbl.angle += M_PI;
                        break;
                    }
                }
                first = false;
            }
        }

        first = true;
        unsigned int  start = ( (myWay.pointsCount - 1) * 2 ) / 3;
        if(start > (myWay.pointsCount - 2)) start = myWay.pointsCount - 2;

        for(unsigned int i = start ; i < myWay.pointsCount; i++)
        {
            int64_t xx = myWay.points[i].x;
            int64_t yy = myWay.points[i].y;
            oldx = x;
            oldy = y;
            x = projectX(_proj, szx, rect.x0, rect.x1, xx);
            y = projectY(_proj, szy, rect.y0, rect.y1, yy, yProjectionCache);
            {
                if(!first)
                {
                    symb_x = oldx;
                    symb_y = oldy;
                    double dfx = x - oldx;
                    double dfy = y - oldy;
                    if((dfx == 0) && (dfy > 0)) symb_angle = M_PI / 2;
                    else if((dfx == 0) && (dfy <= 0)) symb_angle = - M_PI / 2;
                    else symb_angle = atan2(dfy , dfx);
                    break;
                }
                first = false;
            }
        }
    }
    if(draw)
        {
        myWay.crop(r1);
        myWay.reduce((rect.x1 - rect.x0)/size_x, (rect.y1 - rect.y0)/size_y);
        if(myWay.pointsCount > 1) s.mergePoints(myWay.points, myWay.pointsCount, myWay.points[0] == myWay.points[myWay.pointsCount - 1]);
    }
    lbl.fontsize = 12;
    std::size_t found = cl.textStyle.find("font-size:");
    if(found != std::string::npos)
    {
        lbl.fontsize = atoi(cl.textStyle.c_str() + found + 10);
        lbl.sizeFromStyle = true;
    }
    else
    {
       lbl.fontsize = 0;
    }

    if((textWidth*ppm >= 6) || (lbl.fontsize >= 6))
    {
        if((name != "" ) && (textStyle != "") && cl.opened)
        {
            {
                 if(cl.textWidth != "")
                 {
                     textWidth = atoi(cl.textWidth.c_str());
                     lbl.fontsize = textWidth*ppm;
                 }
                 unsigned int chars = 1.4*length / (lbl.fontsize);
                 if(name.length() < chars)
                 {
                       lbl.zindex = cl.textZIndex;
                       lbl.style = cl.rank;
                       lbl.text=name;

                       lbl.ref = std::to_string(myWay.id);
                       lbl.style = cl.rank;
                       lbl.text=name;
                 }
            }
        }
        else if((name != "" ) && (textStyle != "") && !cl.opened)
        {
            if(cl.textWidth != "")
            {
                textWidth = atoi(cl.textWidth.c_str());
                lbl.fontsize = textWidth*ppm;
            }
            unsigned int chars = 1.0*szx*(myWay.rect.x1 - myWay.rect.x0) / (lbl.fontsize * (rect.x1 - rect.x0));
            if(name.length() < chars)
            {
                style += ";stroke-width:" + std::to_string(atoi(cl.width.c_str())*ppm);
                int64_t xxx = myWay.rect.x0/2 + myWay.rect.x1/2;
                int64_t yyy = myWay.rect.y0/2 + myWay.rect.y1/2;

                lbl.zindex = cl.textZIndex;
                 double xi = projectX(_proj, szx, rect.x0, rect.x1, xxx);
                 double yi = projectY(_proj, szy, rect.y0, rect.y1, yyy, yProjectionCache);

                lbl.pos_x = round(xi);
                lbl.pos_y = round(yi);
                lbl.style = cl.rank;
                lbl.text = name;
                lbl.angle = 0;
            }
        }
    }


    if((cl.symbol != "") && !cl.opened)
    {
        int64_t xxx = (myWay.rect.x0 + myWay.rect.x1) / 2;
        int64_t yyy = (myWay.rect.y0 + myWay.rect.y1) /2;
        //x = (xxx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
        //y = (yyy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
          x = projectX(_proj, szx, rect.x0, rect.x1, xxx);
          y = projectY(_proj, szy, rect.y0, rect.y1, yyy, yProjectionCache);

        result << "<use xlink:href=\"#" << cl.symbol << "\"  x=\"" << (int32_t) x  << "\"  y=\"" << (int32_t) y << "\"/>";
        cssClasses.insert("sym#"+cl.symbol);
    } else {
            if(cl.symbol != "")
            {
                if(symb_angle == 0)
                {
                    result << "<use xlink:href=\"#"
                       <<  cl.symbol
                       << "\"  x=\"" << (int32_t)(symb_x) << "\"  y=\"" << (int32_t)(symb_y)
                       << "\"/>";
                } else {
                    result << "<use xlink:href=\"#"
                       <<  cl.symbol
                       << "\"  x=\"" << (int32_t)(symb_x) << "\"  y=\"" << (int32_t)(symb_y)
                       << "\" transform=\"rotate("
                       << std::to_string((symb_angle*180/M_PI))
                       << ","
                       << (int32_t)(symb_x)
                       << ","
                       << (int32_t)(symb_y)
                       << ")\"/>";
                }
                cssClasses.insert("sym#"+cl.symbol);
            }
    }
    result.flush();
    return resultString;
}


std::string SvgRenderer::render(label_s& lbl, Relation& myRelation,Rectangle rect,uint32_t szx, uint32_t szy, CssClass& cl, Shape& s)
{
    lbl.id = myRelation.id  + UINT64_C(0xB000000000000000);;
    lbl.fontsize = 12;
    lbl.text = "";
    lbl.ref = "";
    lbl.pos_x = lbl.pos_y = lbl.angle = 0;
    lbl.style = 0;
    lbl.zindex = cl.textZIndex;
    std::string resultString = "";
    StringBuffer result(resultString);
    std::string resultStringTmp = "";
    StringBuffer resultTmp(resultStringTmp);
    std::string textField = "name";
    if(!(cl.textField == "")) textField = cl.textField;
    bool draw = ((myRelation.rect)*(rect*1.25)).isValid();
    bool keep = false;

    if(cl.style != "")
    {

        if(cl.opened)
        {
            std::string name = "";
            if(cl.textStyle != "")
            {
                if(textField != "name")
                {
                    name = std::string(myRelation.tags[textField]);
                }
                if(name == "" || textField == "name")
                {
                    for ( unsigned int i = 0 ; i < _nb_locales; i++)
                    {
                        std::string tmp = std::string("name:") + std::string(_locales[i],2);
                        name = std::string(myRelation.tags[tmp]);
                        if (name != "") break;

                    }
                    if(name == "") name = std::string(myRelation.tags["name"]);
                }
            }

            std::shared_ptr<label_s> lbl2 = std::make_shared<label_s>();
            lbl2->text = name;
            if(lbl2->text == "") lbl2->text = "void";
            for(auto myWay : myRelation.ways)
            {
                //myWay->fillrec();
                if(!(((myWay->rect)*(rect*1.5)).isValid())) continue;
                keep = true;
                result << render(*lbl2,*myWay, rect, szx, szy, cl, s);
                if((lbl2->text.length() > 0) && (lbl2->fontsize > 5))
                    label_vector.push_back(lbl2);
            }
        }
        else
        {
            if(draw)
            {
                bool started = false;
                for(Line* l: myRelation.shape.openedLines)
                {
                    Rectangle r1 = rect*1.25;
                    //l->crop(r1);
                    //s.mergePoints(l->points, l->pointsCount);
                    l->crop(r1);
                    if(l->pointsCount < 2) continue;
                    bool first = true;
                    int32_t x=0;
                    int32_t y=0;
                    bool good = false;
                    resultTmp.clear();
                    for(unsigned int i = 0 ; i < l->pointsCount; i++)
                    {
                        keep = true;
                        int64_t xx = l->points[i].x;
                        int64_t yy = l->points[i].y;
                        int32_t oldx = x;
                        int32_t oldy = y;
                        x = round(projectX(_proj, szx, rect.x0, rect.x1, xx));
                        y = round(projectY(_proj, szy, rect.y0, rect.y1, yy, yProjectionCache));

                        if((x != oldx) || (y != oldy))
                        {
                            if(first)
                            {
                                resultTmp << "M" << x << " " << y;
                                first = false;
                            }
                            else
                            {
                                if(x== oldx)  resultTmp << "V" << y;
                                else if(y== oldy)  resultTmp << "H" << x;
                                else resultTmp << "L" << x << " " << y;
                                good = true;
                            }
                        }
                    }
                    if(good)
                    {
                        if(!started)
                        {
                            started = true;
                           result << "<path  d=\"";
                        }
                        resultTmp.flush();
                        result << resultStringTmp;
                    }
                }
                for(Line* l: myRelation.shape.closedLines)
                {
                    Rectangle r1 = rect*1.25;
                    l->crop(r1);
                    if(l->pointsCount <= 3) continue; // shape is 'flat'
                    bool first = true;
                    int32_t x=0;
                    int32_t y=0;
                    bool good = false;
                    resultTmp.clear();
                    for(unsigned int i = 0 ; i < l->pointsCount; i++)
                    {
                        keep = true;
                        int64_t xx = l->points[i].x;
                        int64_t yy = l->points[i].y;
                        int32_t oldx = x;
                        int32_t oldy = y;
                        x = round(projectX(_proj, szx, rect.x0, rect.x1, xx));
                        y = round(projectY(_proj, szy, rect.y0, rect.y1, yy, yProjectionCache));

                        if((x != oldx) || (y != oldy))
                        {
                            if(first)
                            {
                                resultTmp << "M" << x << " " << y;
                                first = false;
                            }
                            else
                            {
                                if(x== oldx)  resultTmp << "V" << y;
                                else if(y== oldy)  resultTmp << "H" << x;
                                else resultTmp << "L" << x << " " << y;
                                good = true;
                            }
                        }
                    }
                    if(good)
                    {
                        if(!started)
                        {
                            started = true;
                           result << "<path  d=\"";
                        }
                        resultTmp.flush();
                        result << resultStringTmp;
                    }
                }
                if (started) result << "\" class=\"c" << cl.rank <<"\"/>\n";
            }
            cssClasses.insert("c"+std::to_string(cl.rank));
            if(cl.textStyle != "")
            {
                std::string name = "";
                if(textField != "name")
                {
                    name = std::string(myRelation.tags[textField]);
                }
                if(name == "" || textField == "name")
                {
                    for ( unsigned int i = 0 ; i < _nb_locales; i++)
                    {
                        std::string tmp = std::string("name:") + std::string(_locales[i],2);
                        name = std::string(myRelation.tags[tmp]);
                        if (name != "") break;

                    }
                    if(name == "") name = std::string(myRelation.tags["name"]);
                }
                if(name != "")
                {
                    unsigned int chars = 1.4*szx*(myRelation.rect.x1 - myRelation.rect.x0) / (lbl.fontsize * (rect.x1 - rect.x0));
                    if(name.length() < chars)
                    {
                        int64_t xxx = (myRelation.rect.x0/2 + myRelation.rect.x1/2);
                        int64_t yyy = (myRelation.rect.y0/2 + myRelation.rect.y1/2);
                        //int32_t x = (xxx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
                        //int32_t y = (yyy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
                        double x = projectX(_proj, szx, rect.x0, rect.x1, xxx);
                        double y = projectY(_proj, szy, rect.y0, rect.y1, yyy, yProjectionCache);

                        lbl.pos_x = round(x);
                        lbl.pos_y = round(y);
                        lbl.zindex = cl.textZIndex;
                        lbl.style = cl.rank;
                        lbl.text = name;
                        lbl.angle = 0;
                    }

                }
            }
        }
    }

    if(cl.symbol != "")
    {
        int64_t xxx = (myRelation.rect.x0 + myRelation.rect.x1) / 2;
        int64_t yyy = (myRelation.rect.y0 + myRelation.rect.y1) /2;
        double x = projectX(_proj, szx, rect.x0, rect.x1, xxx);
        double y = projectY(_proj, szy, rect.y0, rect.y1, yyy, yProjectionCache);
        //int64_t x = (xxx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
        //int64_t y = (yyy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
        result << "<use xlink:href=\"#" << cl.symbol << "\"  x=\"" << (int32_t)(x) << "\"  y=\"" << (int32_t)(y) << "\"/>";
        cssClasses.insert("sym#"+cl.symbol);
    }
    result.flush();
    if(keep) return resultString;
    else return "";
}


std::string SvgRenderer::render(label_s& lbl, Point& myNode,
                                    Rectangle rect, uint32_t szx, uint32_t szy, CssClass& cl )
{

    lbl.id = myNode.id  + UINT64_C(0xC000000000000000);
    lbl.fontsize = 12;
    lbl.text = "";
    lbl.ref = "";
    lbl.pos_x = lbl.pos_y = lbl.angle = 0;
    lbl.style = 0;
    lbl.zindex = cl.textZIndex;
    std::string resultString = "";
    StringBuffer result(resultString);
    double x,y;
    double ppm = 107 * ((szx * 1.0) / ((1.0)*(rect.x1 - rect.x0)));
    std::string name = "";

    if(cl.textStyle != "")
    {
        std::string fieldName = "name";
        if (cl.textField != "") fieldName = cl.textField;
        name = std::string(myNode.tags[fieldName]);

        if(fieldName != "name")
        {
            name = std::string(myNode.tags[fieldName]);
        }
        if((name == "") || (fieldName == "name") )
        {
            for ( unsigned int i = 0 ; i < _nb_locales; i++)
            {
                std::string tmp = std::string("name:") + std::string(_locales[i],2);
                name = std::string(myNode.tags[tmp]);
                if (name != "") break;

            }
            if(name == "") name = std::string(myNode.tags["name"]);
        }

        lbl.text = name;

        std::size_t found = cl.textStyle.find("font-size:");
        if(found != std::string::npos)
        {
            lbl.fontsize = atoi(cl.textStyle.c_str() + found + 10);
            lbl.sizeFromStyle = true;
        }


        if(cl.textWidth != "")
        {
            int textWidth = atoi(cl.textWidth.c_str());
            lbl.fontsize = textWidth*ppm;
        }
        else
        {
        }


        if(name != "" )
        {
            int64_t xxx = myNode.x;
            int64_t yyy = myNode.y;
            //x = (xxx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
            //y = (yyy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
            x = projectX(_proj, szx, rect.x0, rect.x1, xxx);
            y = projectY(_proj, szy, rect.y0, rect.y1, yyy, yProjectionCache);
            lbl.pos_x = round(x);
            lbl.pos_y = round(y);
            lbl.style = cl.rank;
            lbl.angle = 0;
        }
    }
    if(cl.symbol != "")
    {
        int64_t xxx = myNode.x;
        int64_t yyy = myNode.y;
        //x = (xxx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
        //y = (yyy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
        x = projectX(_proj, szx, rect.x0, rect.x1, xxx);
        y = projectY(_proj, szy, rect.y0, rect.y1, yyy, yProjectionCache);
        result << "<use xlink:href=\"#" << cl.symbol << "\"  x=\"" << (int32_t)(x) << "\"  y=\"" << (int32_t)(y) << "\"/>";
        cssClasses.insert("sym#"+cl.symbol);
    }
    result.flush();
    return resultString;
}




template<class ITEM> std::shared_ptr<CssClass> SvgRenderer::getCssClass(const IndexDesc& idx, ITEM& item,      short zoom, bool closed)
{
    uint32_t mask = 1LL << zoom;
    std::shared_ptr<CssClass> myCl = nullptr;
    for (auto cd : idx.conditions)
    {

        bool cond = false;

        if constexpr(std::is_same<ITEM,Relation>() || std::is_same<ITEM,Way>())
        {
            cond = ((closed && cd->closed)||(!closed && cd->opened)) && (item.tags[cd->tagKey] != "");
        }


        if constexpr(std::is_same<ITEM,Point>())
        {
            cond = (item.tags[cd->tagKey] != "");
        }


        if (cond)
        {
            for(auto cl : cd->classes)
            {
                if(cl->mask & mask)
                    if (cl->tagValue=="default")
                    {
                        myCl = cl;
                    }
            }
            for(auto cl : cd->classes)
            {
                if(cl->mask & mask)
                    if (cl->tagValue == item.tags[cd->tagKey])
                    {
                        if(myCl && ((myCl->zIndex < cl->zIndex)||((myCl->zIndex == cl->zIndex)&&(myCl->textZIndex < cl->textZIndex))))
                        {
                            myCl = cl;
                        } else if(!myCl){
                            myCl = cl;
                        }
                    }
            }
        }
    }
    return myCl;
}




