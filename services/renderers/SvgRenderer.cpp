#include "SvgRenderer.hpp"
#include "../../GeoBox.hpp"
#include "../../helpers/FileIndex.hpp"
#include "../../helpers/hash.hpp"
#include "../../common/constants.hpp"
#include "../../Coordinates.hpp"
#include <map>
#include <set>
#include <math.h>
#include <sstream>




bool compare(const label_s& l2, const label_s& l1)
{
    if(l1.zindex > l2.zindex) return false;
    if(l2.zindex > l1.zindex) return true;
    if(l1.id < l2.id) return true;
    if(l2.id < l1.id) return false;
    return (l1.text > l2.text);
}

#define MAX_TEXT_LEN 20
#define MIN_TEXT_LEN 15

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
		if(pos != std::string::npos)
		{
		    txt = txt.substr(pos);
		}
		else
		{
            break;
		}
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
		if(pos != std::string::npos)
		{
		    result += text.substr(0,pos);
		    text = text.substr(pos);
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


template<class ITEM> void SvgRenderer::iterate(IndexDesc& idxDesc, Rectangle rect)
{
    GeoBoxSet gSet = makeGeoBoxSet(rect*2.0);
    hh::THashIntegerTable* hash = &nodeHash;;
    if(idxDesc.type == "relation") hash = &relationHash;
    else if(idxDesc.type == "relation") hash = &wayHash; 
    int zIndex;
    for(short i = 0; i < gSet.count; i++)
    {
        GeoBox g;
        g = gSet.boxes[i];
        short  mask = g.maskLength;

        GeoBox maxGeoBox;
        maxGeoBox = g;

        if(mask < 63) {
            uint64_t myMask = UINT64_C(0x1) << (mask);
            maxGeoBox.pos = maxGeoBox.pos + myMask;
        } else {
            maxGeoBox.pos=0xFFFFFFFFFFFFFFFF;
        }

        fidx::Record<IndexEntry, GeoBox> record;
        uint64_t start;
        if(idxDesc.idx->findLastLesser(g, start))
        while(idxDesc.idx->get(start, &record) && (record.key <= maxGeoBox))
        {
            if((record.key.zmMask &  zmMask )&&((record.value.r * (rect*2)).isValid()))
            {
                if( hash->addIfUnique(record.value.id*100 + indexId))
                {
                    zIndex = 0;
                    ITEM* item = NULL;
                    mger->load(item, record.value.id);
                    CssClass* cl = getCssClass(idxDesc, *item, zoom, record.key.zmMask & 0X100000LL);
                    label_s lbl;
                    if(cl)
                    {
                        tmp = render(zIndex, lbl, &idxDesc, *item,
                                                     rect,
                                                     size_x,
                                                     size_y,
                                                     *cl
                                                    );
                        it = resMap.find(zIndex);
                        if(it != resMap.end())
                        {
                            it->second += tmp;
                        }
                        else
                        {
                            resMap[zIndex] = tmp;
                        }
                        if((lbl.text.length() > 0) && (lbl.fontsize > 5))
                            label_vector.push_back(lbl);
                        }
                        delete item;
                }
            }
            start++;
        }
        mask = g.maskLength;
        short max = 64;
        if (max >= 64) max = 64;
        while(mask++ < max )
        {
            GeoBox maxGeoBox2 = g;
            maxGeoBox2.maskLength = mask;
            uint64_t myMask = UINT64_C(0xFFFFFFFFFFFFFFFF) << mask;
            maxGeoBox2.pos = maxGeoBox2.pos & myMask;
            if(idxDesc.idx->findLastLesser(maxGeoBox2, start))
            while(idxDesc.idx->get(start++, &record) && (record.key <= maxGeoBox2))
            {
                if( relationHash.addIfUnique(record.value.id*100 +indexId))
                {
                    if((record.key.zmMask &  zmMask ) && ((record.value.r * (rect *4) ).isValid()))
                    {
                        ITEM* item = NULL;
                        mger->load(item, record.value.id);

                        CssClass* cl = getCssClass(idxDesc, *item, zoom, record.key.zmMask & 0X100000LL);
                        label_s lbl;

                        if(cl /*&& (myRelation->rect * rect1).isValid()*/)
                        {
                            tmp = render(zIndex, lbl, &idxDesc,*item,
                                         rect,
                                         size_x,
                                         size_y, *cl/*, texts*/);
                            it = resMap.find(zIndex);
                            if(it != resMap.end())
                            {
                                it->second += tmp;
                            }
                            else
                            {
                                resMap[zIndex] = tmp;
                            }
                            if((lbl.text.length() > 0) && (lbl.fontsize > 5))
                                label_vector.push_back(lbl);
                        }
                        delete item;
                    }
                }
            }
        }
    }    
}


std::string SvgRenderer::renderItems(Rectangle rect, uint32_t sizex, uint32_t sizey, std::string tag)
{
    size_x = sizex;
    size_y = sizey;
    std::string libs = "";
    //std::map<int,std::string> resMap;
    //std::map<int,std::string>::iterator it;
    //std::string tmp = "";
    //std::vector<label_s> label_vector;
    int zIndex;
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

    std::cout << "zoom;" << zoom << "\n";
    zmMask = 1LL << zoom;
    std::ostringstream result; 
    result << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?> <!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"  \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
    result << "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"0 0 " << std::to_string(sizex) << " " << std::to_string(sizey) << "\">\n";
    result << "<style>\ntext,tspan{dominant-baseline:central;}";
	//std::cout << "0";
	uint32_t mask = 1LL << zoom;
    for (IndexDesc* idxDesc : *(mger->indexes))
    {
        for (Condition* cd : idxDesc->conditions)
        {
            for(CssClass* cl : cd->classes)
            {
				if( cl->mask & mask )
				{
				    result << cl->makeClass("c" + std::to_string(cl->rank), ppm);
				}
			}
		}
	}    
    result << "</style>\n";
    result << "<rect width=\"" << std::to_string(sizex + 1) << "\" height=\"" << std::to_string(sizey + 1) << "\" fill=\"antiquewhite\"/>\n";
    indexId = 0;
    for (IndexDesc* idxDesc : *(mger->indexes))
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

    std::vector<label_s> to_print;    

    
    for(auto t=label_vector.begin(); t!=label_vector.end(); ++t)
    {
        bool to_show = true;
        
        for(auto v = label_vector.begin(); v!=t; ++v)
        {
            int ilt = t->text.length();
            int ilv = v->text.length();
            
            if(ilt > MAX_TEXT_LEN) ilt = MAX_TEXT_LEN;
            if(ilv > MAX_TEXT_LEN) ilv = MAX_TEXT_LEN;

            int dx = v->pos_x - t->pos_x;
            int dy = v->pos_y - t->pos_y;
            if(dx < 0) dx = -dx;
            if(dy < 0) dy = -dy;
            if((t->text == v->text)&&((dx*dx +dy*dy)< 1000))
            {
                to_show = false;
                break;
            }
            if((t->ref == "") && (v->ref == ""))
            {
                if(((uint64_t)(dy << 1) < (t->fontsize + v->fontsize))
                  &&((uint64_t)(dx << 1) < (t->fontsize*0.75*ilt + v->fontsize*0.75*ilv)))
                {
                    to_show = false;
                    break;
                } 
            }
            if((t->ref != "") && (v->ref != ""))
            {
            //double angle = abs(t->angle -v->angle);
            //double sinus = abs(sin(angle));
            //double dist = sqrt(dx*dx +dy*dy);
            double xa,xb,xc,xd,ya,yb,yc,yd;
            
            
            double lt = t->fontsize*0.75*ilt;
            double lv = v->fontsize*0.75*ilv;
            
            xa = v->pos_x + lv*(cos(v->angle));
            ya = v->pos_y + lv*(sin(v->angle));
            xb = v->pos_x - lv*(cos(v->angle));
            yb = v->pos_y - lv*(sin(v->angle));
            
            xc = t->pos_x + lt*(cos(t->angle));
            yc = t->pos_y + lt*(sin(t->angle));
            xd = t->pos_x - lt*(cos(t->angle));
            yd = t->pos_y - lt*(sin(t->angle));
            
            double xab = xb - xa;
            double yab = yb - ya;
            
            double xac = xc - xa;
            double yac = yc -ya;

            double xad = xd - xa;
            double yad = yd -ya;
            
            double pvectabac = (xab * yac) - (yab * xac);
            double pvectabad = (xab * yad) - (yab * xad);
            
            bool diffsidecd = (pvectabac*pvectabad < 0);
            
            double xcd = xd - xc;
            double ycd = yd - yc;
            
            double xca = -xac;
            double yca = -yac;
            
            double xcb = xb -xc;
            double ycb = yb - yc;
            
            double pvectcdca = (xcd*yca) - (ycd*xca);
            double pvectcdcb = (xcd*ycb) - (ycd*xcb);
            
            bool diffsideab = (pvectcdca * pvectcdcb) < 0;  
            
            
            
            if(diffsideab && diffsidecd)
            {
                    to_show = false;
                    break;
            }
            }

        }
        if(to_show) to_print.push_back(*t);
    }

    for(std::pair<int,std::string> tmp : resMap)
    {
        result << tmp.second;
    }
    //result += libs;
    for(auto v=to_print.begin(); v!=to_print.end(); ++v)
    {
        {
            if(v->ref == "")
                result << "<text text-anchor=\"middle\" x=\""
                       << std::to_string(v->pos_x)
                       << "\" y=\""
                       << std::to_string(v->pos_y)
                       << "\" class=\"c"
                       << std::to_string(v->style)
                       << "\">"
                       << cutString(v->text, v->pos_x, v->pos_y, v->fontsize)+"</text>\n";
            else
            {
//               libs += "<text text-anchor=\"middle\" dominant-baseline=\"central\" class=\"c"+std::to_string(v->style)+"\" style=\"font-size:" +std::to_string(v->fontsize)+ "px\"><textPath xlink:href=\"#W"+std::to_string(v->id)+"\" startOffset=\"50%\">"+v->text+"</textPath></text>\n";
               result << "<text text-anchor=\"middle\" class=\"c"
                      << std::to_string(v->style)
                      << "\" style=\"font-size:"
                      << std::to_string(v->fontsize)
                      << "px\" x=\""+std::to_string(v->pos_x)
                      << "\" y=\""+std::to_string(v->pos_y)
                      << "\" transform=\"rotate("
                      << std::to_string(v->angle*180/M_PI)
                      << ","
                      << std::to_string(v->pos_x)
                      << ","
                      << std::to_string(v->pos_y)
                      << ")\">"
                      << cutString(v->text, v->pos_x, v->pos_y, v->fontsize)
                      << "</text>\n";
            }
		}
    }
    if(tag != "") result << "<text style=\"font-size:10px\" x=\"0\" y=\"15\" >"
                         << tag
                         << "</text>\n";
    result << "</svg>";
    return result.str();
}

std::string SvgRenderer::render(int& zIndex, label_s& lbl, IndexDesc* idx,Way& myWay, Rectangle rect,uint32_t szx, uint32_t szy, CssClass& cl)
{
    lbl.id = myWay.id + UINT64_C(0xA000000000000000);
    lbl.fontsize = 12;    
    lbl.text = "";    
    lbl.ref = "";    
    lbl.pos_x = lbl.angle = lbl.pos_y=0;
    lbl.style = 0;    
    lbl.zindex = cl.zIndex;
    std::ostringstream result;
    double oldx = -1;
    double oldy = -1;
    double x=0;
    double y=0;
    std::string color = "red";

    int width=0;
    int textWidth=0;
    std::string style= "";
    std::string textStyle= "";
    double ppm = 50 * ((szx * 1.0) / ((1.0)*(rect.x1 - rect.x0)));
    std::string name = "";
    //if((cl )/*&& ((myWay.rect)*rect).isValid()*/)
    {
        bool draw = ((myWay.rect)*rect).isValid();
        style = cl.style;
        std::string textStyle2;
        textStyle = cl.textStyle;
        if(cl.width.length()) width = std::stoi(cl.width);
        if(cl.textWidth.length()) textWidth = std::stoi(cl.textWidth);
        if(textWidth) textStyle ="font-size:"+ std::to_string((int)(textWidth*ppm))+ "px;" + cl.textStyle;
        if(width && ((width*ppm) <  0.25)) return "";
        if(width) style ="stroke-width:"+ std::to_string(width*ppm)+";" + cl.style;
        std::string textField = "name";
        if(cl.textField != "") textField = cl.textField;
        zIndex = cl.zIndex;
        name = myWay.tags[textField.c_str()];
        if(name == "" && textField != "name" ) name = myWay.tags["name"];

        if(draw) result << "<path  id=\"W" << myWay.id << "\" d=\"";
        bool first = true;
        double xd,xf;
        double length = 0;
        double halfLength = 0;

        xd = myWay.points[0].x;
        xf = myWay.points[myWay.pointsCount -1].x;

        int j;
        for(unsigned int i = 0 ; i < myWay.pointsCount; i++)
        {
            if(xf > xd) j = i;
            else j=myWay.pointsCount - (i+1);
            int64_t xx = myWay.points[j].x;
            int64_t yy = myWay.points[j].y;
            oldx = x;
            oldy = y;
            x = (xx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
            y = (yy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
            {
                if(first)
                {
                    if(draw)result << "M" << trunc(x) << " " << trunc(y)  << " ";
                    first = false;
                }
                else
                {
                    if((x != oldx) || (y != oldy)|| i == (myWay.pointsCount - 1)/*&&(x > -1*szx)&&(x < 2*szx)&&(y > -1*szy)&&(y < 2*szy)*/)
                        if(draw) result << "L" << trunc(x) << " " << trunc(y) << " ";
                    length += sqrt((x-oldx)*(x-oldx) + (y-oldy)*(y-oldy));
                }
            }
        }

        halfLength = length / 2;
        double curLength = 0;
        double ratio = 0;
        first = true;
        for(unsigned int i = 0 ; i < myWay.pointsCount; i++)
        {
            if(xf > xd) j = i;
            else j=myWay.pointsCount - (i+1);
            int64_t xx = myWay.points[j].x;
            int64_t yy = myWay.points[j].y;
            oldx = x;
            oldy = y;
            x = (xx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
            y = (yy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
            {
                if(!first)
                {
                    double oldLength = curLength;
                    curLength += sqrt((x-oldx)*(x-oldx) + (y-oldy)*(y-oldy));
                    if(curLength < halfLength)
                    {
                    }
                    else
                    {
                        ratio = (halfLength - oldLength)/(curLength - oldLength);
                        lbl.pos_x = x * ratio + oldx * (1 - ratio);
                        lbl.pos_y = y * ratio + oldy * (1 - ratio);
                        double dfx = x - oldx;
                        double dfy = y - oldy;
                        if(dfx == 0) lbl.angle = M_PI / 2;
                        else lbl.angle = atan2(dfy , dfx);
                        //if(lbl.angle > M_PI / 2) lbl.angle -= M_PI;
                        //if(lbl.angle < -1 * M_PI / 2) lbl.angle += M_PI;
                        break;
                    }
                }
                first = false;
            }
        }

        if(draw) result << " \" class=\"c" << cl.rank << "\" />\n";
        lbl.fontsize = 12;
        std::size_t found = cl.textStyle.find("font-size:");
        if(found != std::string::npos)
        {
            lbl.fontsize = atoi(cl.textStyle.c_str() + found + 10);
        }
        else
        {
            lbl.fontsize = 0;
        }
        
        if((textWidth*ppm >= 6) || (lbl.fontsize >= 6))
        {
            if((name != "" ) && (textStyle != "") && cl.opened)
            {
                /*if(name.length() < 6)
                {
                    lbl.text = name;
                    if(cl->width != "")
                      style += ";stroke-width:" + std::to_string(atoi(cl->width.c_str())*ppm);
                    if(cl->textWidth != "")
                    {
                        textWidth = atoi(cl->textWidth.c_str());
                        lbl.fontsize = textWidth*ppm;
                        textStyle ="font-size:"+ std::to_string(lbl.fontsize)+ "px;" + cl->textStyle;
                    }
                    else
                    {
                        textStyle = cl->textStyle;
                    }
                    zIndex = cl->zIndex;
                    lbl.zindex = zIndex;
                    if(name != "" )
                    {
                        lbl.angle = 0;
                        lbl.style = cl->rank;
                        lbl.text=name;
                        lbl.angle = 0;
                    }

                }
                else*/
                {
                     if(cl.textWidth != "")
                     {
                       textWidth = atoi(cl.textWidth.c_str());
                       lbl.fontsize = textWidth*ppm;
                     }
                    unsigned int chars = 1.4*length / (lbl.fontsize);
                    if(name.length() < chars)
                    {
                       lbl.zindex = cl.zIndex;
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
                unsigned int chars = 1.4*szx*(myWay.rect.x1 - myWay.rect.x0) / (lbl.fontsize * (rect.x1 - rect.x0));
                if(name.length() < chars)
                {
                style += ";stroke-width:" + std::to_string(atoi(cl.width.c_str())*ppm);

                int64_t xxx = myWay.rect.x0/2 + myWay.rect.x1/2;
                int64_t yyy = myWay.rect.y0/2 + myWay.rect.y1/2;

                lbl.zindex = cl.zIndex;
                int32_t x = (xxx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
                int32_t y = (yyy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
                lbl.pos_x = x;
                lbl.pos_y = y;
                lbl.style = cl.rank;
                lbl.text = name;
                }
            }
        }
    }
    return result.str();
}


std::string SvgRenderer::render(int& zIndex, label_s& lbl, IndexDesc* idx,Relation& myRelation,Rectangle rect,uint32_t szx, uint32_t szy, CssClass& cl)
{
    lbl.id = myRelation.id  + UINT64_C(0xB000000000000000);;
    lbl.fontsize = 12;    
    lbl.text = "";    
    lbl.ref = "";    
    lbl.pos_x = lbl.pos_y = lbl.angle = 0;    
    lbl.style = 0;    
    lbl.zindex = cl.zIndex;
    std::string style= "";
    std::string result = "";
    double ppm = 50 * ((szx * 1.0) / ((1.0)*(rect.x1 - rect.x0)));
    std::string textField = "name";
    if(!(cl.textField == "")) textField = cl.textField;
    bool draw = ((myRelation.rect)*rect).isValid();
    if(cl.opened)
    {
        for(Way* myWay : myRelation.ways)
        {
            if(!(((myWay->rect)*(rect*1.5)).isValid())) continue;
            label_s lbl;
            result += render(zIndex,lbl, idx, *myWay, rect, szx, szy, cl/*, texts*/);
        }
    }
    else
    {

        style = cl.style;
        zIndex = cl.zIndex;
        if(draw)
        {
            result += "<path  d=\"";
            for(Line* l: myRelation.shape.lines)
            {
                Rectangle r1 = rect*1.1;
                l->crop(r1);
                bool first = true;
                int oldx = -1;
                int oldy = -1;
                int x=0;
                int y=0;
                for(unsigned int i = 0 ; i < l->pointsCount; i++)
                {
                    int64_t xx = l->points[i].x;
                    int64_t yy = l->points[i].y;
                    oldx = x;
                    oldy = y;
                    x = (xx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
                    y = (yy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
                    if((x != oldx) || (y != oldy) || i == (l->pointsCount - 1))
                    {
                        if(first)
                        {
                            result += "M" + std::to_string(x) + " " + std::to_string(y) + " ";
                            first = false;
                        }
                        else
                        {
                            result += "L" + std::to_string(x) + " " + std::to_string(y) + " ";
                        //length += sqrt((x-oldx)*(x-oldx) + (y-oldy)*(y-oldy));
                        }
                    }
                }
            }
            result += " \" class=\"c"+std::to_string(cl.rank)+"\" />\n";
        }
        if(cl.textStyle != "")
        {
            std::string textStyle ="";
            int  textWidth;
            std::string name = "";
            name = myRelation.tags[textField.c_str()];
            if(name == "" && textField != "name" ) name = myRelation.tags["name"];
            if(name != "")
            {
                if(cl.textWidth != "")
                {
                    if ( cl.textWidth == "auto" )
                    {
                        int length = name.length();
                        if (length < 12) length = 12;
                        int64_t dxx = (myRelation.rect.x1 - myRelation.rect.x0);
                        uint32_t dx = (dxx)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
                        textWidth = (dx*1.5)/(2.0*length);
                        textStyle ="font-size:"+ std::to_string((int)textWidth)+ "px;" + cl.textStyle;
                    }
                    else
                    {
                        textWidth = atoi(cl.textWidth.c_str());
                        textStyle ="font-size:"+ std::to_string((int)(textWidth*ppm))+ "px;" + cl.textStyle;
                    }
                }
                else
                {
                    textStyle = cl.textStyle;
                }
                unsigned int chars = 1.4*szx*(myRelation.rect.x1 - myRelation.rect.x0) / (lbl.fontsize * (rect.x1 - rect.x0));
                if(name.length() < chars)
                {
                int64_t xxx = (myRelation.rect.x0/2 + myRelation.rect.x1/2);
                int64_t yyy = (myRelation.rect.y0/2 + myRelation.rect.y1/2);
                int32_t x = (xxx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
                int32_t y = (yyy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
                lbl.pos_x = x;
                lbl.pos_y = y;
                lbl.zindex = cl.zIndex;
                lbl.style = cl.rank;
                lbl.text = name;
                }
            }
        }
    }
    return result;
}


std::string SvgRenderer::render(int& zIndex, label_s& lbl, IndexDesc* idx,Point& myNode,
                                    Rectangle rect, uint32_t szx, uint32_t szy, CssClass& cl)
{

    lbl.id = myNode.id  + UINT64_C(0xC000000000000000);
    lbl.fontsize = 12;    
    lbl.text = "";    
    lbl.ref = "";    
    lbl.pos_x = lbl.pos_y = lbl.angle = 0;    
    lbl.style = 0;    
    lbl.zindex = cl.zIndex;
    std::string result = "";
    int x,y;
    int textWidth=0;
    std::string style= "";
    std::string textStyle= "";
    double ppm = 107 * ((szx * 1.0) / ((1.0)*(rect.x1 - rect.x0)));
    std::string name = "";

    //if(cl)
    {
        std::string fieldName = "name";
        if (cl.textField != "") fieldName = cl.textField;
        name = myNode.tags[fieldName.c_str()];
        if(name == "" && fieldName != "name" ) name = myNode.tags["name"];
        lbl.text = name;
        style = cl.style;
        
        std::size_t found = cl.textStyle.find("font-size:");
        if(found != std::string::npos)
        {
            lbl.fontsize = atoi(cl.textStyle.c_str() + found + 10);
        }


        if(cl.textWidth != "")
        {
            style += ";stroke-width:" + std::to_string(atoi(cl.width.c_str())*ppm);
            textWidth = atoi(cl.textWidth.c_str());
            lbl.fontsize = textWidth*ppm;
            textStyle ="font-size:"+ std::to_string((int)(textWidth*ppm))+ "px;" + cl.textStyle;
        }
        else
        {
            textStyle = cl.textStyle;
        }
        zIndex = cl.zIndex;


        if(name != "" )
        {
            int64_t xxx = myNode.x;
            int64_t yyy = myNode.y;
            x = (xxx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
            y = (yyy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
            lbl.pos_x = x;
            lbl.pos_y = y;
            lbl.style = cl.rank;
        }
    }
    return result;
}


CssClass* SvgRenderer::getCssClass(IndexDesc& idx, Relation& b, short zoom, bool isClosed)
{
    uint32_t mask = 1LL << zoom;
    CssClass* myCl = NULL;
    for (Condition* cd : idx.conditions)
    {
        if (((b.isClosed && cd->closed)||(!b.isClosed && cd->opened)) && (b.tags[cd->tagKey.c_str()] != ""))
        {
            for(CssClass* cl : cd->classes)
            {
                if(cl->mask & mask)
                    if (cl->tagValue=="default")
                    {
                        myCl = cl;
                    }
            }
            for(CssClass* cl : cd->classes)
            {
                if(cl->mask & mask)
                    if (cl->tagValue == b.tags[cd->tagKey.c_str()])
                    {
                        myCl = cl;
                    }
            }
        }
    }
    return myCl;
}

CssClass* SvgRenderer::getCssClass(IndexDesc& idx, Way& b, short zoom, bool isClosed)
{
    uint32_t mask = 1LL << zoom;
    CssClass* myCl = NULL;
    for (Condition* cd : idx.conditions)
    {
        if (((isClosed && cd->closed)||(!isClosed && cd->opened)) && (b.tags[cd->tagKey.c_str()] != ""))
        {
            for(CssClass* cl : cd->classes)
            {
                if(cl->mask & mask)
                    if (cl->tagValue=="default")
                    {
                        myCl = cl;
                    }
            }
            for(CssClass* cl : cd->classes)
            {
                if(cl->mask & mask)
                    if (cl->tagValue == b.tags[cd->tagKey.c_str()])
                    {
                        myCl = cl;
                    }
            }
        }
    }
    return myCl;
}

CssClass* SvgRenderer::getCssClass(IndexDesc& idx, Point& b, short zoom, bool isClosed)
{
    uint32_t mask = 1LL << zoom;
    CssClass* myCl = NULL;
    for (Condition* cd : idx.conditions)
    {
        if (b.tags[cd->tagKey.c_str()] != "")
        {
            for(CssClass* cl : cd->classes)
            {
                if(cl->mask & mask)
                    if (cl->tagValue=="default")
                    {
                        myCl = cl;
                    }
            }
            for(CssClass* cl : cd->classes)
            {
                if(cl->mask & mask)
                    if (cl->tagValue == b.tags[cd->tagKey.c_str()])
                    {
                        myCl = cl;
                    }
            }
        }
    }
    return myCl;
}


