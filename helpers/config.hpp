#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "../GeoBox.hpp"
#include "Sequence.hpp"
#include "FileIndex.hpp"

struct CssClass
{
    std::string tagValue;
    std::string className;
    std::string style;
    std::string width;
    std::string textStyle;
    std::string textWidth;
    std::string textField;
    int zIndex;
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
	std::string makeClass(std::string clName, double ppm)
	{
		std::string result = "";
		std::string tStyle = makeTextStyle(ppm);
		std::string oStyle = makeStyle(ppm);
		if(tStyle != "") result += "text." + clName +  "{" + tStyle + "}\n";
		if(oStyle != "") result += "path." + clName +  "{" + oStyle + "}\n";
		return result;
	}
};

struct Condition
{
    std::string tagKey;
    std::vector<CssClass*> classes;
    bool closed;
    bool opened;
};

struct Selector
{
    std::string tagKey;
    std::string tagValue;
};

struct IndexEntry
{
	uint64_t id;
	Rectangle r;
};

struct IndexDesc
{
    std::string type;
    std::string name;
    std::vector<Condition*> conditions;
    std::vector<Selector*> selectors;
    std::vector<Selector*> excludeSelectors;
    fidx::FileIndex<IndexEntry,GeoBox>* idx;
    uint32_t mask;
};

struct ParmsXmlVisitor
{
    std::map<std::string, std::string> parameters;
    void log(uint64_t done){};
    void startTag(std::vector<SeqBalise*> tagStack, SeqBalise* b)
    {
        if (b->baliseName == "parameter")
        {
            parameters[ b->keyValues["name"] ] = b->keyValues["value"];
        }

    }
    void endTag(std::vector<SeqBalise*> tagStack, SeqBalise* b)
    {
    }

    long long getNumParam(std::string key, long long defaultValue)
    {
        if(parameters.count(key)) return atoll(parameters[key].c_str());
        else return defaultValue;
    }

    long long getNumParam(std::string key)
    {
        return getNumParam(key, 0);
    }

    std::string getParam(std::string key, std::string defaultValue)
    {
        if(parameters.count(key)) return parameters[key];
        else return defaultValue;
    }

    std::string getParam(std::string key)
    {
        return getParam(key, "");
    }

};


struct XmlVisitor
{
    std::vector<IndexDesc*>* idxList;
    int clRank;
    bool closed,opened,ccre;
    std::string root;
    uint32_t mask, idxMask;

    XmlVisitor(std::vector<IndexDesc*>& indexList, bool create, std::string rt)
    {
        idxList = &indexList;
        ccre = create;
        root = rt;
		clRank=0;
    }

    ~XmlVisitor()
    {
    }
    void log(uint64_t done)
    {
    }

    void startTag(std::vector<SeqBalise*> tagStack, SeqBalise* b)
    {
        if (b->baliseName == "index")
        {
            IndexDesc* idx = new IndexDesc;
            idxList->push_back(idx);
            //idx->tagKey = b->keyValues["tagKey"];
            idx->type = b->keyValues["type"];
            idx->name = b->keyValues["name"];
            std::string fName = root + "/" + b->keyValues["name"];
            //std::cout << "opening " << fName << "\n";
            idx->idx = new fidx::FileIndex<IndexEntry,GeoBox>(fName, ccre);
            idx->mask = 0;
            idxMask = 0;
        }
        if (b->baliseName == "condition")
        {
            Condition* cdt = new Condition;
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
            Selector* sel = new Selector;
            (*idxList)[idxList->size() - 1]->excludeSelectors.push_back(sel);
            sel->tagKey = b->keyValues["tagKey"];
            sel->tagValue = b->keyValues["tagValue"];
            //std::cerr << "found selector for : " << sel->tagKey << " " << sel->tagValue << "\n";
        }
        if (b->baliseName == "select")
        {
            Selector* sel = new Selector;
            (*idxList)[idxList->size() - 1]->selectors.push_back(sel);
            sel->tagKey = b->keyValues["tagKey"];
            sel->tagValue = b->keyValues["tagValue"];
            //std::cerr << "found selector for : " << sel->tagKey << " " << sel->tagValue << "\n";
        }
        if (b->baliseName == "class")
        {
            CssClass* cdt = new CssClass;
            //IndexDesc* desc =
            (*idxList)[idxList->size() - 1]->conditions[(*idxList)[idxList->size() - 1]->conditions.size() - 1]->classes.push_back(cdt);
            cdt->tagValue = b->keyValues["value"];
            cdt->className = (b->keyValues["name"]);
            cdt->style = (b->keyValues["style"]);
            cdt->width = (b->keyValues["width"]);
            cdt->textStyle = (b->keyValues["textStyle"]);
            cdt->textField = (b->keyValues["textField"]);
            cdt->textWidth = (b->keyValues["textWidth"]);
            cdt->zIndex = atoi(b->keyValues["zIndex"].c_str());
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
                    while(isdigit(*c)) c++;
                    c++;
                }
            }
        }
    }

    void endTag(std::vector<SeqBalise*> tagStack, SeqBalise* b)
    {
        if (b->baliseName == "restriction")
        {
            mask= 0XFFF;
        }
        else if(b->baliseName == "index")
        {
            idxList->back()->mask = idxMask;
        }
    }

};
#endif
