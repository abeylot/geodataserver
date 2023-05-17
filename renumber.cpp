#include "helpers/Sequence.hpp"
#include "common/GeoTypes.hpp"
#include "Coordinates.hpp"
#include "helpers/FileIndex.hpp"

#define BALISENAME_RELATION  "relation"
#define BALISENAME_NODE      "node"
#define BALISENAME_WAY       "way"
#define BALISENAME_tag       "tag"

using namespace fidx;
struct XmlVisitor
{
    uint64_t relid;
    uint64_t wayid;
    uint64_t nodid;
    uint64_t tags;

    FileIndex<uint64_t, uint64_t> *relationIdIndex;
    FileIndex<uint64_t, uint64_t> *wayIdIndex;
    FileIndex<GeoPointNumberIndex, uint64_t> *nodeIdIndex;

    XmlVisitor(std::string rep)
    {
        rep += "/";
        relid = 0;
        wayid = 0;
        nodid = 0;
        tags = 0;
        relationIdIndex = new FileIndex<uint64_t,uint64_t>((rep + "relationIdIndex") .c_str(), true);
        wayIdIndex      = new FileIndex<uint64_t,uint64_t>((rep +"wayIdIndex").c_str(), true);
        nodeIdIndex     = new FileIndex<GeoPointNumberIndex, uint64_t>((rep + "nodeIdIndex").c_str(), true);
    }

    ~XmlVisitor()
    {
        relationIdIndex->flush();
        wayIdIndex->flush();
        nodeIdIndex->flush();
        if(!relationIdIndex->isSorted()) relationIdIndex->sort();
        if(!wayIdIndex->isSorted()) wayIdIndex->sort();
        if(!wayIdIndex->isSorted()) nodeIdIndex->sort();
        delete relationIdIndex;
        delete wayIdIndex;
        delete nodeIdIndex;
    }




    void log(uint64_t done)
    {
        std::cerr << " done " << (done >> UINT64_C(20)) << "Mio.\t relations " << relid << "\tways " << wayid << "\tnodes " << nodid<< "\n" << std::flush;
    }
    void startTag(const std::vector<SeqBalise*>& tagStack, SeqBalise* b)
    {
    }
    void stringNode(const std::vector<SeqBalise*>& tagStack, std::string& s)
    {
    }
    void endTag(const std::vector<SeqBalise*>& tagStack, SeqBalise* b)
    {
        if (b->baliseName == BALISENAME_RELATION)
        {

            relationIdIndex->append(atoll((b->keyValues["id"]).c_str()), relid++);
            tags=0;
        }
        else if (b->baliseName == BALISENAME_WAY)
        {

            wayIdIndex->append(atoll((b->keyValues["id"]).c_str()), wayid++);
            tags=0;
        }
        
        else if (b->baliseName == BALISENAME_TAG)
        {

            tags++;;
        }
        
        else if (b->baliseName == BALISENAME_NODE)
        {

            nodeIdIndex->append(atoll((b->keyValues["id"]).c_str()),
                (GeoPointNumberIndex){ nodid, Coordinates::toNormalizedLon(b->keyValues["lon"]), Coordinates::toNormalizedLat(b->keyValues["lat"])}
            );
            if(tags) nodid++;
            tags = 0;
        }
    }
};


int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cerr << "path argument is missing\n";
        exit(1);
    }
    XmlVisitor* v = new XmlVisitor(std::string(argv[1]));
    XmlFileParser<XmlVisitor>::parseXmlFile(stdin,*v);
    delete v;
}
