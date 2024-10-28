#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "../GeoBox.hpp"
#include "Sequence.hpp"
#include "FileIndex.hpp"
#include "../common/GeoTypes.hpp"
#include "../Coordinates.hpp"
#include <algorithm>
#include <memory>

struct CssClass
{
    std::string tagValue;
    std::string className;
    std::string style;
    std::string width;
    std::string textStyle;
    std::string textWidth;
    std::string textField;
    std::string symbol;
    std::string pattern;

    int zIndex;
    int textZIndex;
    int rank;
    uint32_t mask;
    bool closed;
    bool opened;
    std::string makeStyle(double ppm)
    {
        std::string result = style;
        int iWidth = 0;
        if(width != "") iWidth = std::stoi(width);
        if (iWidth)
        {
            result = "stroke-width:"+ std::to_string((int)(iWidth*ppm))+";" + result;
        }
        return result;
    }
    std::string makeTextStyle(double ppm)
    {
        std::string result = textStyle;
        int iWidth = 0;
        if(textWidth != "") iWidth = std::stoi(textWidth);
        if (iWidth)
        {
            result = "font-size:"+ std::to_string((int)(iWidth*ppm))+ "px;" + result;
        }
        return result;
    }
    std::string makeClass(const std::string& clName, double ppm)
    {
        std::string result = "";
        //std::string result = "/*" + tagValue + "*/\n";
        std::string tStyle = makeTextStyle(ppm);
        std::string oStyle = makeStyle(ppm);
        std::string fillRule="fill-rule:";
        /*if(evenOdd) fillRule +="evenodd;";
        else*/ fillRule += "nonzero;";
        if(tStyle != "") result += "text." + clName +  "{" + tStyle + "}\n";
        if(oStyle != "") result += "path." + clName +  "{" +fillRule + oStyle + "}\n";
        return result;
    }
};

struct Condition
{
    std::string tagKey;
    std::vector<std::shared_ptr<CssClass>> classes;
    bool closed;
    bool opened;
    //~Condition()
    //{
    //    for(auto c : classes) delete c;
    //}
};

struct Selector
{
    std::string tagKey;
    std::string tagValue;
};

struct __attribute__((packed)) IndexEntry
{
    uint64_t id;
    Rectangle r;
};

struct __attribute__((packed)) IndexEntryMasked
{
    uint64_t id;
    Rectangle r;
    uint32_t zmMask;
};



struct __attribute__((packed)) IndexRange
{
    uint64_t first;
    uint64_t last;
};

struct IndexDesc
{
    std::string type;
    std::string name;
    std::vector<std::shared_ptr<Condition>> conditions;
    std::vector<std::shared_ptr<Selector>> selectors;
    std::vector<std::shared_ptr<Selector>> excludeSelectors;
    fidx::FileIndex<IndexEntryMasked,GeoBox>* idx;
    uint32_t mask;
    ~IndexDesc()
    {
        //for(auto a : conditions) delete a;
        //for(auto b : selectors) delete b;
        //for(auto c: excludeSelectors) delete c;
        delete idx;
    }
};

struct PngImage
{
    std::string filename;
    double lon_min, lon_max, lat_min, lat_max;
    explicit PngImage(const std::string filename, double lon_min, double lon_max, double lat_min, double lat_max):
    filename(filename),
    lon_min(lon_min),
    lon_max(lon_max),
    lat_min(lat_min),
    lat_max(lat_max)
    {
    }
};

struct ParmsXmlVisitor
{
    std::map<std::string, std::string> parameters;
    void log([[maybe_unused]] uint64_t done){};
    void startTag([[maybe_unused]] std::vector<SeqBalise*>& tagStack, SeqBalise* b)
    {
        if (b->baliseName == "parameter")
        {
            parameters[ b->keyValues["name"] ] = b->keyValues["value"];
        }

    }
    void endTag([[maybe_unused]] std::vector<SeqBalise*>& tagStack, [[maybe_unused]] SeqBalise* b)
    {
    }

    void stringNode([[maybe_unused]] std::vector<SeqBalise*>& tagStack, [[maybe_unused]] std::string& s)
    {
    }

    long long getNumParam(const std::string& key, long long defaultValue) const
    {
        auto it = parameters.find(key);
        if(it != parameters.end()) return atoll(it->second.c_str());
        else return defaultValue;
    }

    long long getNumParam(const std::string& key) const
    {
        return getNumParam(key, 0);
    }

    std::string getParam(std::string key, std::string defaultValue) const
    {
        auto it = parameters.find(key);
        if(it != parameters.end()) return it->second;
        else return defaultValue;
    }

    std::string getParam(const std::string& key) const
    {
        return getParam(key, "");
    }

};


struct XmlVisitor
{
    std::vector<std::shared_ptr<IndexDesc>>* idxList;
    std::vector<PngImage> imageList;
    std::map<std::string, std::string> symbols;
    //std::map<std::string, std::string> patterns;
    std::map<std::string, std::string> shortcuts;
    std::map<std::string, std::string> charconvs;
    int clRank;
    bool closed,opened,ccre,symbol,pattern;
    std::string root;
    uint32_t mask, idxMask;
    std::string symbolId;
    std::string symbolStr="";


//    std::string patternId;
//    std::string patternStr="";


    XmlVisitor(std::vector<std::shared_ptr<IndexDesc>>& indexList, bool create,const std::string& rt):root(rt), symbolId(""),symbolStr("")
    {
        idxList = &indexList;
        ccre = create;
        clRank=0;
        opened = false;
        closed = false;
        symbol = false;
        pattern = false;
        mask = 0;
        idxMask = 0;
    }

    ~XmlVisitor()
    {
    }
    void log([[maybe_unused]] uint64_t done)
    {
    }

    void startTag([[maybe_unused]]std::vector<SeqBalise*>& tagStack, SeqBalise* b)
    {
        if (b->baliseName == "style")
        {
            shortcuts[b->keyValues["id"]] = b->keyValues["value"];
        }
        if (
            (b->baliseName == "symbol")
            ||
            (b->baliseName == "pattern")
            ||
            (b->baliseName == "marker")
            ||
            (b->baliseName == "filter")
           )
        {
            //shortcuts[b->keyValues["ref"]] = b->keyValues["value"];
            symbolId = b->keyValues["id"];
            symbolStr = "";
        }
        if(symbolId != "")
        {
            symbolStr += "<" + b->baliseName + " ";
            for(auto it = b->keyValues.begin(); it != b->keyValues.end(); it++)
            {
                symbolStr += " " + it->first + "=\"" + it->second + "\" ";
            }
            symbolStr += ">";
        }
        if (b->baliseName == "index")
        {
            auto idx = std::make_shared<IndexDesc>();
            idxList->push_back(idx);
            //idx->tagKey = b->keyValues["tagKey"];
            idx->type = b->keyValues["type"];
            idx->name = b->keyValues["name"];
            std::string fName = root + "/" + b->keyValues["name"];
            //std::cout << "opening " << fName << "\n";
            idx->idx = new fidx::FileIndex<IndexEntryMasked,GeoBox>(fName, ccre);
            idx->mask = 0;
            idxMask = 0;
        }
        if (b->baliseName == "condition")
        {
            auto cdt = std::make_shared<Condition>();
            //IndexDesc* desc =
            (*idxList)[idxList->size() - 1]->conditions.push_back(cdt);
            cdt->tagKey = b->keyValues["tagKey"];
            closed = (b->keyValues["closed"] == "yes") || (b->keyValues["closed"] == "both");
            opened = (b->keyValues["closed"] == "no") || (b->keyValues["closed"] == "both");
            cdt->opened = opened;
            cdt->closed = closed;
        }
        if (b->baliseName == "exclude")
        {
            auto sel = std::make_shared<Selector>();
            (*idxList)[idxList->size() - 1]->excludeSelectors.push_back(sel);
            sel->tagKey = b->keyValues["tagKey"];
            sel->tagValue = b->keyValues["tagValue"];
            //std::cerr << "found selector for : " << sel->tagKey << " " << sel->tagValue << "\n";
        }
        if (b->baliseName == "select")
        {
            auto sel = std::make_shared<Selector>();
            (*idxList)[idxList->size() - 1]->selectors.push_back(sel);
            sel->tagKey = b->keyValues["tagKey"];
            sel->tagValue = b->keyValues["tagValue"];
            //std::cerr << "found selector for : " << sel->tagKey << " " << sel->tagValue << "\n";
        }
        if (b->baliseName == "class")
        {
            auto cdt = std::make_shared<CssClass>();
            //IndexDesc* desc =
            (*idxList)[idxList->size() - 1]->conditions[(*idxList)[idxList->size() - 1]->conditions.size() - 1]->classes.push_back(cdt);
            cdt->tagValue = b->keyValues["value"];
            cdt->className = (b->keyValues["name"]);
            cdt->style = (b->keyValues["style"]);
            if(cdt->style.length() > 1 && cdt->style.at(0) == '#')
            {
                if(shortcuts.find(cdt->style.substr(1)) != shortcuts.end())
                {
                    cdt->style = shortcuts[cdt->style.substr(1)];
                }
            }
            cdt->width = (b->keyValues["width"]);
            cdt->textStyle = (b->keyValues["textStyle"]);
            if(cdt->textStyle.length() > 1 && cdt->textStyle.at(0) == '#')
            {
                if(shortcuts.find(cdt->textStyle.substr(1)) != shortcuts.end())
                {
                    cdt->textStyle = shortcuts[cdt->textStyle.substr(1)];
                }
            }
            cdt->symbol = (b->keyValues["symbol"]);
            cdt->textField = (b->keyValues["textField"]);
            cdt->textWidth = (b->keyValues["textWidth"]);
            cdt->zIndex = atoi(b->keyValues["zIndex"].c_str());
            cdt->textZIndex = atoi(b->keyValues["textZIndex"].c_str());
            cdt->mask = mask;
            cdt->rank = clRank++;
            idxMask = idxMask | mask;
            cdt->opened = opened;
            cdt->closed = closed;
        }
        if (b->baliseName == "restriction")
        {
            mask = 0;
            std::string zLevels = b->keyValues["zLevels"];
            if(zLevels.length())
            {
                const char* c = zLevels.c_str();
                while(*c)
                {
                    long level = 1LL << (atoll(c));
                    mask |= level;
                    while(*c && isdigit(*c)) c++;
                    if(*c) c++;
                }
            }
        }
        if (b->baliseName == "image")
        {
            double x1 = atof(b->keyValues["x1"].c_str());
            double x2 = atof(b->keyValues["x2"].c_str());
            double x3 = atof(b->keyValues["x3"].c_str());
            double x4 = atof(b->keyValues["x4"].c_str());


            double y1 = atof(b->keyValues["y1"].c_str());
            double y2 = atof(b->keyValues["y2"].c_str());
            double y3 = atof(b->keyValues["y3"].c_str());
            double y4 = atof(b->keyValues["y4"].c_str());

            imageList.push_back(PngImage(b->keyValues["ref"],
                std::min(std::min(x1,x2),std::min(x3,x4)),
                std::max(std::max(x1,x2),std::max(x3,x4)),
                std::min(std::min(y1,y2),std::min(y3,y4)),
                std::max(std::max(y1,y2),std::max(y3,y4))));
        }
    }

    void endTag([[maybe_unused]] std::vector<SeqBalise*>& tagStack, SeqBalise* b)
    {
        if ( symbolId != "")
        {
            symbolStr += "</" + b->baliseName + ">";
        }
        if (
            (b->baliseName == "symbol")
            ||
            (b->baliseName == "pattern")
            ||
            (b->baliseName == "marker")
            ||
            (b->baliseName == "filter")
           )
        {
            symbols[symbolId] = symbolStr;
            //std::cout << symbolId << " -> " << symbolStr << "\n";
            symbolId = "";
        }
        if (b->baliseName == "restriction")
        {
            mask= 0XFFF;
        }
        else if(b->baliseName == "index")
        {
            idxList->back()->mask = idxMask;
        }
        else if(b->baliseName == "charconv")
        {
            if(!b->keyValues["from"].empty())
            {
                charconvs[b->keyValues["from"]] = b->keyValues["to"];
            }
        }
    }

    void stringNode([[maybe_unused]] std::vector<SeqBalise*>& tagStack, std::string& s)
    {
        if(symbolId != "")
        {
            symbolStr += s;
            //std::cout << "[" << s << "]\n";
        }
    }


};
#endif
