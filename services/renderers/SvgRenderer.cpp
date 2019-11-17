#include "SvgRenderer.hpp"
#include "../../GeoBox.hpp"
#include "../../helpers/FileIndex.hpp"
#include "../../helpers/hash.hpp"
#include "../../common/constants.hpp"
#include "../../Coordinates.hpp"
#include <map>
#include <set>
#include <math.h>
#include <vector>
#include <sstream>




bool compare(const label_s& l2, const label_s& l1)
{
    if(l1.zindex > l2.zindex) return false;
    if(l2.zindex > l1.zindex) return true;
    //if(l1.fontsize > l2.fontsize) return false;
    //if(l2.fontsize > l1.fontsize) return true;
    //if(l1.text.length() < l2.text.length()) return false;
    //if(l1.text.length() > l2.text.length()) return true;
    if(l1.id < l2.id) return true;
    if(l2.id < l1.id) return false;
    //std::cout << "argh " << l1.x << " " << l2.x << "\n";
    return (&l1 > &l2);
}



std::string SvgRenderer::renderItems(Rectangle rect, uint32_t sizex, uint32_t sizey, std::string tag)
{
    std::string libs = "";
    std::map<int,std::string> resMap;
    std::map<int,std::string>::iterator it;
    std::string tmp = "";
    std::vector<label_s> label_vector;
    resetWayId();
    double ppm = 50 * ((sizex * 1.0) / ((1.0)*(rect.x1 - rect.x0)));
    uint32_t msz = rect.x1 - rect.x0;
    short zoom = 31;
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
    uint32_t zmMask = 1LL << zoom;
    std::string    result = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?> <!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"  \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
    result += "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"0 0 "+std::to_string(sizex)+" "+std::to_string(sizey)+"\">\n";
    result += "<style>\n";
	std::cout << "0";
	uint32_t mask = 1LL << zoom;
    for (IndexDesc* idxDesc : *(mger->indexes))
    {
        for (Condition* cd : idxDesc->conditions)
        {
            for(CssClass* cl : cd->classes)
            {
				if( cl->mask & mask )
				{
				    result += cl->makeClass("c" + std::to_string(cl->rank), ppm);
				}
			}
		}
	}    
    result += "</style>\n";
    result += "<rect width=\""+std::to_string(sizex + 1)+"\" height=\""+std::to_string(sizey + 1)+"\" fill=\"antiquewhite\"/>\n";
    hh::THashIntegerTable relationHash(100000), wayHash(100000), nodeHash(100000);
    //std::set<std::string> texts;
    int zIndex;

    Rectangle rect1 = rect*1.5;
    GeoBoxSet gSet = makeGeoBoxSet(rect*1.5);
    short indexId;
    for(short i = 0; i < gSet.count; i++)
    {
        //std::cout << "count : " << std::to_string(i) << "\n";
        GeoBox g;
        g = gSet.boxes[i];
        short  mask = g.maskLength;

        GeoBox maxGeoBox;
        maxGeoBox = g;

        uint64_t myMask = UINT64_C(0x1) << (mask);
        maxGeoBox.pos = maxGeoBox.pos + myMask;

        fidx::Record<IndexEntry, GeoBox> record;
        uint64_t start;
        indexId = 0;
        for (IndexDesc* idxDesc : *(mger->indexes))
        {
            indexId++;
            if(!(idxDesc->mask & zmMask)) continue;
            if(idxDesc->type == "relation")
            {
                if(idxDesc->idx->findLastLesser(g, start))
                //idxDesc->idx->findLastLesser(g, start);
                while(idxDesc->idx->get(start, &record) && (record.key <= maxGeoBox))
                {
                    if((record.key.zmMask &  zmMask )&&((record.value.r * rect1).isValid()))
                    {
                        if( relationHash.addIfUnique(record.value.id*100 + indexId))
                        {
                            Relation* myRelation = mger->loadRelation(record.value.id);

                            CssClass* cl = getCssClass(*idxDesc, *myRelation, zoom, record.key.zmMask & 0X100000LL);
                            label_s lbl;
                            if(cl && (myRelation->rect * rect1).isValid() )
                            {
                                tmp = render(zIndex, lbl, idxDesc, *myRelation,
                                                     rect,
                                                     sizex,
                                                     sizey,
                                                     cl//, texts
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
                            delete myRelation;
                        }
                    }
                    start++;
                }
                short mask = g.maskLength;
                short max = 64;
                if (max >= 64) max = 64;
                //std::cout << "max = " << max << "\n"; 
                while(mask++ < max )
                {
                    //std::cout << "mask = " << mask << "\n"; 
                    GeoBox maxGeoBox2 = g;
                    maxGeoBox2.maskLength = mask;
                    uint64_t myMask = UINT64_C(0xFFFFFFFFFFFFFFFF) << mask;
                    maxGeoBox2.pos = maxGeoBox2.pos & myMask;
                    if(idxDesc->idx->findLastLesser(maxGeoBox2, start))
                    //idxDesc->idx->findLastLesser(maxGeoBox2, start);
                    while(idxDesc->idx->get(start++, &record) && (record.key <= maxGeoBox2))
                    {
						//std::cout << record.key.pos << "|||" << maxGeoBox2.pos << "\n";
                        if( relationHash.addIfUnique(record.value.id*100 +indexId))
                        {
                            if((record.key.zmMask &  zmMask ) && ((record.value.r * rect1).isValid()))
                            {
                                Relation* myRelation = mger->loadRelation(record.value.id);

                                CssClass* cl = getCssClass(*idxDesc, *myRelation, zoom, record.key.zmMask & 0X100000LL);
                                label_s lbl;

                                if(cl && (myRelation->rect * rect1).isValid())
                                {
                                    tmp = render(zIndex, lbl, idxDesc,*myRelation,
                                                         rect,
                                                         sizex,
                                                         sizey, cl/*, texts*/);
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
                                delete myRelation;
                            }
                        }
                    }
                }
            }
            else if (idxDesc->type == "way")
            {
                Rectangle rect1 = rect*1.5;
                //idxDesc->idx->findLastLesser(g, start);
                if(idxDesc->idx->findLastLesser(g, start))
                while(idxDesc->idx->get(start, &record) && (record.key <= maxGeoBox))
                {
                    if((record.key.zmMask &  zmMask ) && ((record.value.r * rect1).isValid())
                            /*&& (getRectangle(record.key) * rect).isValid()*/)
                    {
                        //GeoWayIndex idx;
                        if( wayHash.addIfUnique(record.value.id*100 + indexId))  //
                        {
                            Way* myWay = mger->loadWay(record.value.id);
                            CssClass* cl = getCssClass(*idxDesc, *myWay, zoom,  record.key.zmMask & 0X100000LL);
                            if(cl && (myWay->rect * rect1).isValid())
                            {
                                //std::string label = std::to_string(record.key.pos) +":"+ std::to_string(record.key.maskLength);
                                label_s lbl;
                                tmp = render(zIndex,lbl, idxDesc, *myWay,
                                                rect,
                                                sizex,
                                                sizey, cl/*, texts*/);
                                it = resMap.find(zIndex);
                                if(it != resMap.end())
                                {
                                    it->second += tmp;
                                }
                                else
                                {
                                    resMap[zIndex] = tmp;
                                }
                                //libs += label;
                                if((lbl.text.length() > 0) && (lbl.fontsize >= 5))
                                    label_vector.push_back(lbl);
                            }
                            delete myWay;
                        }
                    }
                    start++;
                }
                short mask = g.maskLength;
                //short max = mask + 8;
                short max = 64;
                if (max >= 64) max = 64;
                while(mask++ < max)
                {
                    GeoBox maxGeoBox2 = g;
                    maxGeoBox2.maskLength = mask;
                    uint64_t myMask = UINT64_C(0xFFFFFFFFFFFFFFFF) << mask;
                    maxGeoBox2.pos = maxGeoBox2.pos & myMask;
                    if(idxDesc->idx->findLastLesser(maxGeoBox2, start))
                    //idxDesc->idx->findLastLesser(maxGeoBox2, start);
                    while(idxDesc->idx->get(start++, &record) && (record.key <= maxGeoBox2))
                    {
                        if((record.key.zmMask &  zmMask )&&((record.value.r * rect1).isValid()))
                        {
                            //GeoWayIndex idx;
                            if( wayHash.addIfUnique(record.value.id * 100 + indexId))
                            {
                                Way* myWay = mger->loadWay(record.value.id);
                                CssClass* cl = getCssClass(*idxDesc, *myWay, zoom, record.key.zmMask & 0X100000LL);

                                if(cl && (myWay->rect * rect1).isValid())
                                {
                                    label_s lbl;
                                    tmp = render(zIndex,lbl, idxDesc,*myWay,
                                                    rect,
                                                    sizex,
                                                    sizey, cl/*, texts*/);
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
                                delete myWay;
                            }
                        }
                    }
                }
            }
        }
    }
    rect1 = rect * 4;
    gSet = makeGeoBoxSet(rect1);
    //gSet = makeGeoBoxSet(rect*1.5);
    for(short i = 0; i < gSet.count; i++)
    {

        GeoBox g;

        g = gSet.boxes[i];
        short  mask = g.maskLength;

        GeoBox maxGeoBox;
        maxGeoBox = g;

        uint64_t myMask = UINT64_C(0x1) << (mask);
        maxGeoBox.pos = maxGeoBox.pos + myMask;


        fidx::Record<IndexEntry, GeoBox> record;
        uint64_t start;
        indexId = 0;
        for (IndexDesc* idxDesc : *(mger->indexes))
        {
            indexId++;
            if(!(idxDesc->mask & zmMask)) continue;
            if (idxDesc->type == "node")
            {
                if(idxDesc->idx->findLastLesser(g, start))
                //idxDesc->idx->findLastLesser(g, start);
                while(idxDesc->idx->get(start, &record) && (record.key <= maxGeoBox))
                {
                    label_s label;
                    if( nodeHash.addIfUnique(record.value.id*100 + indexId))
                    {
                        if(record.key.zmMask &  zmMask )
                        {
                            Point* myNode = mger->loadPoint(record.value.id);
                            CssClass* cl = getCssClass(*idxDesc, *myNode, zoom,false);
                            if(cl)
                            {
                                //if((myNode->x > rect1.x0) && (myNode->x < rect1.x1) && (myNode->y > rect1.y0) && (myNode->y < rect1.y1))
                                {
                                    tmp = render(zIndex,label, idxDesc, *myNode,
                                                     rect,
                                                     sizex,
                                                     sizey, cl);
                                    it = resMap.find(zIndex);
                                    if(it != resMap.end())
                                    {
                                        it->second += tmp;
                                    }
                                    else
                                    {
                                        resMap[zIndex] = tmp;
                                    }
                                    if((label.text.length() > 0) && (label.fontsize > 5))
                                        label_vector.push_back(label);
                                }
                            }
                            delete myNode;
                        }
                    }
                    start++;
                }
            }
        }
    }

    std::sort(label_vector.begin(), label_vector.end(),compare);

    std::vector<label_s> to_print;    

    for(auto t=label_vector.begin(); t!=label_vector.end(); ++t)
    {
        bool to_show = true;
        for(auto v = to_print.begin(); v!=to_print.end(); ++v)
        {
            int dx = v->pos_x - t->pos_x;
            int dy = v->pos_y - t->pos_y;
            //unsigned int dz = (t->fontsize + v->fontsize) * 4;
            if(dx < 0) dx = -dx;
            if(dy < 0) dy = -dy;
            if((t->text == v->text)&&((dx*dx +dy*dy)< 10000))
            {
                //std::cout << "ttt\n"; 
                to_show = false;
                break;
            }
            if((t->ref == "") && (v->ref == ""))
            {
                if(((uint64_t)(dy << 1) < (t->fontsize*2 + v->fontsize*2))
                  &&((uint64_t)(dx << 1) < (t->fontsize*1.5*t->text.length() + v->fontsize*1.5*v->text.length())))
                {
                    //std::cout << "collision : " << v->text << " " <<t->text << "\n";    
                    to_show = false;
                    break;
                } 
            //}else{
            //    if(t->ref != "") {
            //        to_show = true;
            //        break;
            //    } 
            }
        }
        if(to_show) to_print.push_back(*t);
    }
    for(auto v=to_print.begin(); v!=to_print.end(); ++v)
    {
        int32_t x0 = v->pos_x - v->fontsize*0.75*v->text.length();
        int32_t x1 = v->pos_x + v->fontsize*0.75*v->text.length();
        int32_t y0 = v->pos_y - v->fontsize*0.75*v->text.length();
        int32_t y1 = v->pos_y + v->fontsize*0.75*v->text.length();
        if((x1 >= 0) && (y1 >= 0)  && (x0 < (int32_t)sizex) && (y0 < (int32_t)sizey))
        {
        //    libs +=  "<!--" + std::to_string(x0) + ","+ std::to_string(x1) + ","+ std::to_string(y0) + ","+ std::to_string(y1) + "," + std::to_string(sizex) + ","+ std::to_string(sizey) + "-->" ;
            if(v->ref == "")
                libs += "<text text-anchor=\"middle\" dominant-baseline=\"central\" x=\""+std::to_string(v->pos_x)+"\" y=\""+std::to_string(v->pos_y)+"\" class=\"c"+std::to_string(v->style)+"\">"+v->text+"</text>\n";
            else
               libs += "<text text-anchor=\"middle\" dominant-baseline=\"central\" class=\"c"+std::to_string(v->style)+"\" style=\"font-size:" +std::to_string(v->fontsize)+ "px\"><textPath xlink:href=\"#W"+v->ref+"\" startOffset=\"50%\">"+v->text+"</textPath></text>\n";
		}
    }

    for(std::pair<int,std::string> tmp : resMap)
    {
        result += tmp.second;
        //std::cout << "drawing z-index : " << std::to_string(tmp.first) << "\n";
    }
    result += libs;
    if(tag != "") result += "<text style=\"font-size:10px\" x=\"0\" y=\"15\" >"+tag+"</text>\n";
    result+="</svg>";
    return result;
}

std::string SvgRenderer::render(int& zIndex, label_s& lbl, IndexDesc* idx,Way& myWay, Rectangle rect,uint32_t szx, uint32_t szy, CssClass* cl)
{
    lbl.id = myWay.id;
    lbl.fontsize = 12;    
    lbl.text = "";    
    lbl.ref = "";    
    lbl.pos_x = lbl.angle = lbl.pos_y=0;
    lbl.style = 0;    
    lbl.zindex = cl->zIndex;
    std::ostringstream result;
    int64_t oldx = -1;
    int64_t oldy = -1;
    int64_t x=0;
    int64_t y=0;
    std::string color = "red";

    int width=0;
    int textWidth=0;
    std::string style= "";
    std::string textStyle= "";
    double ppm = 50 * ((szx * 1.0) / ((1.0)*(rect.x1 - rect.x0)));
    std::string name = "";
    //if((name == "" ) || (textStyle == "") /*&& cl->opened*/) {
    //    myWay.crop(rect);
    //}
    if((cl )/*&& ((myWay.rect)*rect).isValid()*/)
    {
        uint64_t wayId = makeWayId();
        style = cl->style;
        std::string textStyle2;
        textStyle = cl->textStyle;
        if(cl->width.length()) width = std::stoi(cl->width);
        if(cl->textWidth.length()) textWidth = std::stoi(cl->textWidth);
        if(textWidth) textStyle ="font-size:"+ std::to_string((int)(textWidth*ppm))+ "px;" + cl->textStyle;
        if(width && ((width*ppm) <  0.25)) return "";
        if(width) style ="stroke-width:"+ std::to_string(width*ppm)+";" + cl->style;
        std::string textField = "name";
        if(cl->textField != "") textField = cl->textField;
        zIndex = cl->zIndex;
        name = myWay.tags[textField.c_str()];

        result << "<path  id=\"W" << wayId << "\" d=\"";
        bool first = true;
        double xd,xf;
        double length = 0;

        xd = myWay.points[0].x;
        xf = myWay.points[myWay.pointsCount -1].x;

        //if(xf > xd) {
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
            if(((x >> 2 != oldx >> 2) || (y >> 2 != oldy >> 2)|| i == (myWay.pointsCount - 1))/*&&(x > -1*szx)&&(x < 2*szx)&&(y > -1*szy)&&(y < 2*szy)*/)
            {
                if(first)
                {
                    result << "M" << x << " " << y  << " ";
                    first = false;
                    //xd = x;
                }
                else
                {
                    result << "L" << x << " " << y << " ";
                    length += sqrt((x-oldx)*(x-oldx) + (y-oldy)*(y-oldy));
                    //xf = x;
                }
            }
            //oldx=x;
            //oldy=y;
        }


        //result << " \" style=\"" << style << "\" />\n";
        result << " \" class=\"c" << cl->rank << "\" />\n";
        //int64_t xx = ((myWay.rect.x0 + myWay.rect.x1)/2);
        //int64_t yy = ((myWay.rect.y0 + myWay.rect.y1)/2);
        //x = xx*szx;
        //y = yy*szy;
        //std::string textRef = name +std::to_string(x / 256) +":"+std::to_string(y / 256);
        lbl.fontsize = 12;
        std::size_t found = cl->textStyle.find("font-size:");
        if(found != std::string::npos)
        {
            lbl.fontsize = atoi(cl->textStyle.c_str() + found + 10);
        }
        else
        {
            lbl.fontsize = 0;
        }
        
        if((textWidth*ppm > 6) || (lbl.fontsize > 6))
        {
            if((name != "" ) && (textStyle != "") && cl->opened)
            {
                if(name.length() < 6)
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
                        int64_t xxx = myWay.points[myWay.pointsCount / 2].x;
                        int64_t yyy = myWay.points[myWay.pointsCount / 2].y;
                        x = (xxx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
                        y = (yyy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
                        //std::cout << x << " " << y << "\n";
                        lbl.pos_x = x;
                        lbl.pos_y = y;
                        lbl.style = cl->rank;
                        lbl.text=name;
                    }

                }
                else
                {
                     if(cl->textWidth != "")
                     {
                       textWidth = atoi(cl->textWidth.c_str());
                       lbl.fontsize = textWidth*ppm;
                     }
                    //texts.insert(textRef);
                    unsigned int chars = 1.4*length / (lbl.fontsize);
                    //if(xf < xd ) side = " side=\"right\" ";
                    if(name.length() < chars)
                    {
                       int64_t xxx = myWay.points[myWay.pointsCount / 2].x;
                       int64_t yyy = myWay.points[myWay.pointsCount / 2].y;
                       lbl.zindex = cl->zIndex;
                       lbl.pos_x = (xxx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
                       lbl.pos_y = (yyy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));

                        //std::cout << lbl.pos_x << " " << lbl.pos_y << "\n";

                       double dx = (double)myWay.points[0].x - (double)myWay.points[myWay.pointsCount - 1].x;
                       double dy = (double)myWay.points[0].y - (double)myWay.points[myWay.pointsCount - 1].y;
                       double dfx = dx*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
                       double dfy = dy*(szy*1.0) /(1.0*(rect.y1 - rect.y0));

                       
                       if(dx == 0) lbl.angle = M_PI / 2;
                       else lbl.angle = atan2(dfy , dfx);
                       
                       lbl.ref = std::to_string(wayId);
                       lbl.style = cl->rank;
                       lbl.text=name;
                       
            //if(xd > xf)    
                    //    label = "<text class=\"c"+std::to_string(cl->rank)+"\"><textPath xlink:href=\"#W"+std::to_string(wayId)+"\" startOffset=\"50%\"><tspan dy=\""+std::to_string(textWidth*ppm*0.4)+"\">"+name+"</tspan></textPath></text>\n";
            //else     
            //            label = "<text style=\""+textStyle+"\"><textPath xlink:href=\"#W"+std::to_string(myWay.rect.x0)+"-"+std::to_string(myWay.rect.y0)+"\" startOffset=\"66%\"><tspan dy=\""+std::to_string(textWidth*ppm*0.4)+"\">"+name+"</tspan></textPath></text>";
                    //std::cerr << textRef << "\n";
                    }
                }
            }
            else if((name != "" ) && (textStyle != "") && !cl->opened)
            {
                unsigned int chars = 1.4*szx*(myWay.rect.x1 - myWay.rect.x0) / (lbl.fontsize * (rect.x1 - rect.x0));
                if(name.length() < chars)
                {
                //texts.insert(textRef);
                style += ";stroke-width:" + std::to_string(atoi(cl->width.c_str())*ppm);
                //int width = atoi(cl->width.c_str());
                //int width = atoi(cl->width.c_str());

                int64_t xxx = myWay.rect.x0/2 + myWay.rect.x1/2;
                int64_t yyy = myWay.rect.y0/2 + myWay.rect.y1/2;
                //int64_t xxx = myWay.rect.x0;
                //int64_t yyy = myWay.rect.y0;

                lbl.zindex = cl->zIndex;
                int32_t x = (xxx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
                int32_t y = (yyy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
                lbl.pos_x = x;
                lbl.pos_y = y;
                lbl.style = cl->rank;
                lbl.text = name;
                //label = "<text text-anchor=\"middle\" x=\""+std::to_string(x)+"\" y=\""+std::to_string(y)+"\" class=\"c"+std::to_string(cl->rank)+"\">"+name+"</text>\n";
                //std::cerr << label << "\n";
                }
            }
        }
    }
    return result.str();
}


std::string SvgRenderer::render(int& zIndex, label_s& lbl, IndexDesc* idx,Relation& myRelation,Rectangle rect,uint32_t szx, uint32_t szy, CssClass* cl)
{
    lbl.id = myRelation.id;
    lbl.fontsize = 12;    
    lbl.text = "";    
    lbl.ref = "";    
    lbl.pos_x = lbl.pos_y = lbl.angle = 0;    
    lbl.style = 0;    
    lbl.zindex = cl->zIndex;
    std::string style= "";
    std::string result = "";
    double ppm = 50 * ((szx * 1.0) / ((1.0)*(rect.x1 - rect.x0)));
    std::string textField = "name";
    if(!(cl->textField == "")) textField = cl->textField;

    if(cl->opened)
    {
        for(Way* myWay : myRelation.ways)
        {
            //GeoWayIndex g;
            //mger->fillPoints(*myWay, g);
            if(!(((myWay->rect)*(rect*1.5)).isValid())) continue;
            label_s lbl;
            result += render(zIndex,lbl, idx, *myWay, rect, szx, szy, cl/*, texts*/);
            //delete myWay;
        }
    }
    else
    {

        style = cl->style;
        zIndex = cl->zIndex;
        result += "<path  d=\"";
        //Shape shape;
        //for(Way* myWay : myRelation.ways) {
        //    shape.mergePoints(myWay->points, myWay->pointsCount);
        //}
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
        result += " \" class=\"c"+std::to_string(cl->rank)+"\" />\n";
        if(cl->textStyle != "")
        {
            std::string textStyle ="";
            int  textWidth;
            std::string name = "";
            name = myRelation.tags[textField.c_str()];
            if(name != "")
            {
                if(cl->textWidth != "")
                {
                    if ( cl->textWidth == "auto" )
                    {
                        int length = name.length();
                        if (length < 12) length = 12;
                        int64_t dxx = (myRelation.rect.x1 - myRelation.rect.x0);
                        uint32_t dx = (dxx)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
                        textWidth = (dx*1.5)/(2.0*length);
                        textStyle ="font-size:"+ std::to_string((int)textWidth)+ "px;" + cl->textStyle;
                    }
                    else
                    {
                        //style += ";stroke-width:" + std::to_string(atoi(cl->width.c_str())*ppm);
                        //int width = atoi(cl->width.c_str());
                        textWidth = atoi(cl->textWidth.c_str());
                        textStyle ="font-size:"+ std::to_string((int)(textWidth*ppm))+ "px;" + cl->textStyle;
                    }
                }
                else
                {
                    textStyle = cl->textStyle;
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
                lbl.zindex = cl->zIndex;
                lbl.style = cl->rank;
                lbl.text = name;
                //label = "<text text-anchor=\"middle\" x=\""+std::to_string(x)+"\" y=\""+std::to_string(y)+"\" style=\""+textStyle+"\">"+name+"</text>\n";
                //std::cerr << label << "\n";
                }
            }
        }
    }
    //std::cerr <<"label : " << label <<  "-" << cl->textStyle <<"\n";
    return result;
}


std::string SvgRenderer::render(int& zIndex, label_s& lbl, IndexDesc* idx,Point& myNode,
                                    Rectangle rect, uint32_t szx, uint32_t szy, CssClass* cl)
{

    lbl.id = myNode.id;
    lbl.fontsize = 12;    
    lbl.text = "";    
    lbl.ref = "";    
    lbl.pos_x = lbl.pos_y = lbl.angle = 0;    
    lbl.style = 0;    
    lbl.zindex = cl->zIndex;
    std::string result = "";
    int x,y;
    int textWidth=0;
    std::string style= "";
    std::string textStyle= "";
    double ppm = 107 * ((szx * 1.0) / ((1.0)*(rect.x1 - rect.x0)));
    std::string name = "";

    if(cl)
    {
        std::string fieldName = "name";
        if (cl->textField != "") fieldName = cl->textField;
        name = myNode.tags[fieldName.c_str()];
        lbl.text = name;
        style = cl->style;
        
        std::size_t found = cl->textStyle.find("font-size:");
        if(found != std::string::npos)
        {
            lbl.fontsize = atoi(cl->textStyle.c_str() + found + 10);
        }


        if(cl->textWidth != "")
        {
            style += ";stroke-width:" + std::to_string(atoi(cl->width.c_str())*ppm);
            //int width = atoi(cl->width.c_str());
            textWidth = atoi(cl->textWidth.c_str());
            lbl.fontsize = textWidth*ppm;
            textStyle ="font-size:"+ std::to_string((int)(textWidth*ppm))+ "px;" + cl->textStyle;
        }
        else
        {
            textStyle = cl->textStyle;
        }
        zIndex = cl->zIndex;


        if(name != "" )
        {
            int64_t xxx = myNode.x;
            int64_t yyy = myNode.y;
            x = (xxx - rect.x0)*(szx*1.0) /(1.0*(rect.x1 - rect.x0));
            y = (yyy - rect.y0)*(szy*1.0) /(1.0*(rect.y1 - rect.y0));
			lbl.pos_x = x;
            lbl.pos_y = y;
			lbl.style = cl->rank;
                
            //label = "<text text-anchor=\"middle\" x=\""+std::to_string(x)+"\" y=\""+std::to_string(y)+"\" style=\""+textStyle+"\">"+name+"</text>";
            //std::cout << textStyle << "\n";
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
    //bool isClosed = (b.points[b.pointsCount-1] == b.points[0]);
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


