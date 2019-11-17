#define MAIN
#include <map>
#include <vector>
#include "helpers/Sequence.hpp"
#include "helpers/FileIndex.hpp"
#include "common/constants.hpp"
#include "common/GeoTypes.hpp"
#include "Coordinates.hpp"

#define DUMP(myString) fPos +=fprintf(fOut,"%s",myString.c_str());

FILE* fOut;
size_t fPos = 0;

std::map<std::string, long> relationTags;
std::map<std::string, long> wayTags;
std::map<std::string, long> nodeTags;

using namespace fidx;
using namespace std;

struct XmlFilterVisitor
{
    std::string type;
    std::string key;
    std::string value;
    std::map<std::string, long> * curindex = &nodeTags;


    XmlFilterVisitor()
    {
        type=key=value="";
    }

    ~XmlFilterVisitor()
    {
    }


    void startTag(std::vector<SeqBalise*> tagStack, SeqBalise* b)
    {
        if (b->baliseName == "index")
        {
            type = b->keyValues["type"];
            if(type=="relation")
            {
                cerr << "-------------------------------relation\n";
                curindex = &relationTags;
            }
            else if(type=="way")
            {
                curindex = &wayTags;
                cerr << "-------------------------------way\n";
            }
            else if(type=="node")
            {
                cerr << "-------------------------------node\n";
                curindex = &nodeTags;
            }
        }
        if (b->baliseName == "condition")
        {
            key = b->keyValues["tagKey"];
        }
        if (b->baliseName == "class")
        {
            std::string textField = b->keyValues["textField"];
            if(textField != "")
            {
                (*curindex)[textField+"^default"] = 0;
                cerr << type << ":::" << key << "::::" << "default" << "\n";
            }
            (*curindex)["name^default"] = 0;
            cerr << type << ":::" << "name" << "::::" << "default" << "\n";
            value = b->keyValues["value"];
            //curindex->push_back(key+"^"+value);
            (*curindex)[key+"^"+value] = 0;
            cerr << type << ":::" << key << "::::" << value << "\n";
        }
        if (b->baliseName == "restriction")
        {
        }
    }

    void endTag(std::vector<SeqBalise*> tagStack, SeqBalise* b)
    {
    }

    void log(size_t done)
    {
    }

};

struct XmlVisitor
{
    uint64_t relid;
    uint64_t wayid;
    uint64_t nodid;

    //size_t wayPointsLastPos, wayPointsPos;
    //FILE *wayPoints;
    FileRawData<GeoPoint>* wayPoints;

    bool inWay;
    bool inNode;
    bool inRelation;

    FileRawIndex<GeoIndex>      *relationIndex;
    FileRawIndex<GeoWayIndex>   *wayIndex;
    FileRawIndex<GeoPointIndex> *nodeIndex;


    FileIndex<uint64_t, uint64_t> *relationIdIndex;
    FileIndex<uint64_t, uint64_t> *wayIdIndex;
    FileIndex<GeoPointNumberIndex, uint64_t> *nodeIdIndex;

    //std::vector<size_t> startStack;
    SeqBalise* lastBalise;
    std::map<std::string, long> &curindex = nodeTags;

    XmlVisitor()
    {
        relid = 0;
        wayid = 0;
        nodid = 0;
        //wayPointsLastPos = wayPointsPos = 0;
        wayPoints = new FileRawData<GeoPoint>("wayPoints", true);
        inWay = inNode = inRelation =false;
        relationIdIndex = new FileIndex<uint64_t,uint64_t>("relationIdIndex", false);
        wayIdIndex 		= new FileIndex<uint64_t,uint64_t>("wayIdIndex",      false);
        nodeIdIndex 	= new FileIndex<GeoPointNumberIndex,uint64_t>("nodeIdIndex",     false);

        relationIndex   = new FileRawIndex<GeoIndex>("relationIndex",  true);
        wayIndex 		= new FileRawIndex<GeoWayIndex>("wayIndex",       true);
        nodeIndex 	    = new FileRawIndex<GeoPointIndex>("nodeIndex", true);


        lastBalise = NULL;

    }

    ~XmlVisitor()
    {

        relationIndex->flush();
        wayIndex->flush();
        nodeIndex->flush();
        wayPoints->flush();

        delete relationIndex;
        delete wayIndex;
        delete nodeIndex;

        delete relationIdIndex;
        delete wayIdIndex;
        delete nodeIdIndex;

        delete wayPoints;
    }

    static	bool filterAttr(const std::string s)
    {
        if((s== "ref")||(s=="k")||(s=="v")||(s=="type")) return true;
        return false;
    }

    static std::string translateBname(std::string s)
    {
        if(s == BALISENAME_MEMBER ) return BALISENAME_MEMBERSHORT;
        if(s == BALISENAME_ND ) return BALISENAME_NDSHORT;
        if(s == BALISENAME_NODE ) return BALISENAME_NODESHORT;
        if(s == BALISENAME_RELATION ) return BALISENAME_RELATIONSHORT;
        if(s == BALISENAME_TAG ) return BALISENAME_TAGSHORT;
        if(s == BALISENAME_WAY ) return BALISENAME_WAYSHORT;
        return s;
    }

    std::string translateAttr(SeqBalise* b, std::string k, std::string v)
    {
        bool res;
        if(v=="") v= "999999999999";
        if(k == "id")
        {
            fidx::Record<uint64_t, uint64_t> rec;
            fidx::Record<GeoPointNumberIndex, uint64_t> recp;
            if(b->baliseName == BALISENAME_NODE)
            {
                res = nodeIdIndex->find(atoll(v.c_str()), &recp);
                if (res) return std::to_string(recp.value.number);
            }
            else if(b->baliseName == BALISENAME_WAY)
            {
                res = wayIdIndex->find(atoll(v.c_str()), &rec);
                if (res) return std::to_string(rec.value);
            }
            else if(b->baliseName == BALISENAME_RELATION)
            {
                res = relationIdIndex->find(atoll(v.c_str()), &rec);
                if (res) return std::to_string(rec.value);
            }
            //if (res) return std::to_string(rec.value);
            //else {
            cout << b->baliseName << " " << v << " not found \n";
            exit(-1);
            //}
        }
        else if(k == "ref")
        {
            fidx::Record<uint64_t, uint64_t> rec;
            fidx::Record<GeoPointNumberIndex, uint64_t> recp;
            if(b->baliseName == BALISENAME_ND)
            {
                res = nodeIdIndex->find(atoll(v.c_str()), &recp);
                if (res)
                {
                    if(inWay && res)
                    {
                        GeoPoint p;
                        p.x = recp.value.x;
                        p.y = recp.value.y;
                        wayPoints->append(p);
                    }
                    return std::to_string(recp.value.number);
                }
            }
            else if(b->keyValues["type"] == BALISENAME_NODE)
            {
                res = nodeIdIndex->find(atoll(v.c_str()), &recp);
                if(inWay && res)
                {
                    GeoPoint p;
                    p.x = recp.value.x;
                    p.y = recp.value.y;
                    wayPoints->append(p);
                }
                if (res) return std::to_string(recp.value.number);
            }
            else if(b->keyValues["type"] == BALISENAME_WAY)
            {
                res = wayIdIndex->find(atoll(v.c_str()), &rec);
                if (res) return std::to_string(rec.value);
            }
            else if(b->keyValues["type"] == BALISENAME_RELATION)
            {
                res = relationIdIndex->find(atoll(v.c_str()), &rec);
                if (res) return std::to_string(rec.value);
            }
            else
            {
                std::cout << "!!!" << b->baliseName << ":" << b->keyValues["type"] << "\n";
                exit(-1);
            }
            //if (res) return std::to_string(rec.value);
            //else {
            cerr << b->baliseName << " " << v << " not found \n";
            return std::to_string(9999999999999LL);
            //}
        }
        else return v;
        //	return v;
    }

    std::string dumpBegin(SeqBalise* b)
    {
//        cerr << b->baliseName << "aaab\n";
        if (b->baliseName == BALISENAME_TAG )
        {
//            cerr << "aaa\n";
            std::string toSearch = b->keyValues["k"]+"^"+b->keyValues["v"];
            std::string toSearch2 = b->keyValues["k"]+"^default";
            if((curindex.find(toSearch2) == curindex.end()) && (curindex.find(toSearch) == curindex.end()))
            {
                return "";
            }
        }
        else if (b->baliseName == BALISENAME_NODE )
        {
            //cerr << "switch node";
            curindex = nodeTags;
        }
        else if (b->baliseName == BALISENAME_WAY )
        {
            //cerr << "switch way";
            curindex = wayTags;
        }
        else if (b->baliseName == BALISENAME_RELATION )
        {
            //cerr << "switch relation";
            curindex = relationTags;
        }
        string result = "<" + translateBname(b->baliseName) ;
        map<string,string>::iterator attrIt;
        for (attrIt = b->keyValues.begin(); attrIt != b->keyValues.end(); attrIt ++)
        {
            if(filterAttr(attrIt->first))
                result += " " + attrIt->first + "=" + "\"" + translateAttr(b,attrIt->first,attrIt->second) + "\"";
        }
        result += ">";
        return result;
    }

    std::string dumpFull(SeqBalise* b)
    {
//        cerr << b->baliseName << "aad\n";
        if (b->baliseName == BALISENAME_TAG )
        {
//            cerr << "aac\n";
            std::string toSearch = b->keyValues["k"]+"^"+b->keyValues["v"];
            std::string toSearch2 = b->keyValues["k"]+"^default";
            if((curindex.find(toSearch2) == curindex.end()) && (curindex.find(toSearch) == curindex.end()))
            {
                return "";
            }
        }
        string result = "<" + translateBname(b->baliseName) +" ";
        map<string,string>::iterator attrIt;
        bool hasAttr = false;
        for (attrIt = b->keyValues.begin(); attrIt != b->keyValues.end(); attrIt ++)
        {
            if(filterAttr(attrIt->first))
            {
                result += attrIt->first + "=" + "\"" + translateAttr(b, attrIt->first,attrIt->second) + "\"";
                hasAttr = true;
            }
        }
        if(!hasAttr) return "";
        result += "/>\n";
        if((b->baliseName == BALISENAME_ND)&&(inWay)) return "";
        return result;
    }

    string dumpEnd(SeqBalise* b)
    {
        string result = "</" +translateBname(b->baliseName) +">\n";
        return result;
    }



    void indexBalise(SeqBalise* b, size_t start, size_t size)
    {
        if (b->baliseName == BALISENAME_RELATION)
        {
            GeoIndex record;
            record.start = start;
            record.size = size;
            relationIndex->append(record);
        }
        if (b->baliseName == BALISENAME_WAY)
        {
            GeoWayIndex record;
            record.start = start;
            record.size = size;
            record.pstart = wayPoints->startCount;
            record.psize = wayPoints->itemCount  - wayPoints->startCount;
            wayIndex->append(record);
        }
        if (b->baliseName == BALISENAME_NODE)
        {
            GeoPointIndex record;
            record.start = start;
            record.size = size;
            record.x = Coordinates::toNormalizedLon(b->keyValues["lon"]);
            record.y = Coordinates::toNormalizedLat(b->keyValues["lat"]);
            nodeIndex->append(record);
        }
    }


    void log(size_t done)
    {
        std::cerr << " done " << (done >> 20ULL) << "Mio.\t relations " << relationIndex->itemCount << "\tways " << wayIndex->itemCount << "\tnodes " << nodeIndex->itemCount<< "\r" << std::flush;
    }

    void startTag(std::vector<SeqBalise*> tagStack, SeqBalise* b)
    {
        if(lastBalise)
        {
            lastBalise->start = fPos;
            DUMP(dumpBegin(lastBalise));
        }
        //startStack.push_back(fPos);
        lastBalise = b;
        if(b->baliseName == BALISENAME_NODE)
        {
            inNode = true;
        }
        else if (b->baliseName == BALISENAME_WAY)
        {
            inWay = true;
            wayPoints->startBatch();
        }
        else if (b->baliseName == BALISENAME_RELATION)
        {
            inRelation = true;
        }
    }

    void endTag(std::vector<SeqBalise*> tagStack, SeqBalise* b)
    {
        //size_t start = startStack.back();
        //startStack.pop_back();
        if (lastBalise == b)
        {
            b->start = fPos;
            DUMP(dumpFull(b));
            b->size = fPos - b->start;
        }
        else
        {
            DUMP(dumpEnd(b));
            b->size = fPos - b->start;
        }
        lastBalise = NULL;
        indexBalise(b,b->start, b->size);
        if(b->baliseName == BALISENAME_NODE)
        {
            inNode = false;
        }
        else if (b->baliseName == BALISENAME_WAY)
        {
            inWay = false;
        }
        else if (b->baliseName == BALISENAME_RELATION)
        {
            inRelation = false;
        }
    }

};




int main(int argc, char *argv[])
{
    fOut = stdout;
    XmlFilterVisitor f;
    FILE* conf = fopen("./config.xml","r");
    XmlFileParser<XmlFilterVisitor>::parseXmlFile(conf,f);
    fclose(conf);
    cerr << "node tags\n";
    /*for(std::string s : nodeTags) {
       cerr << s <<  "\n";
    }
    cerr << "way tags\n";
    for(std::string s : wayTags) {
       cerr << s <<  "\n";
    }
    cerr << "relation tags\n";
    for(std::string s : relationTags) {
       cerr << s <<  "\n";
    }*/
    XmlVisitor v;
    XmlFileParser<XmlVisitor>::parseXmlFile(stdin,v);
}
