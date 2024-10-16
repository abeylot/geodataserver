#include "helpers/Sequence.hpp"
#include "common/GeoTypes.hpp"
#include "Coordinates.hpp"
#include "helpers/FileIndex.hpp"

#define BALISENAME_RELATION  "relation"
#define BALISENAME_NODE      "node"
#define BALISENAME_WAY       "way"
#define BALISENAME_TAG       "tag"

using namespace fidx;
class XmlVisitor
{
private:

    XmlVisitor(const XmlVisitor&);
    XmlVisitor& operator=(const XmlVisitor&);

public:
    uint64_t relid;
    uint64_t wayid;
    uint64_t nodid;
    uint64_t tags;

    FileIndex<uint64_t, uint64_t> *relationIdIndex;
    FileIndex<uint64_t, uint64_t> *wayIdIndex;
    FileIndex<GeoPointNumberIndex, uint64_t> *nodeIdIndex;

    explicit XmlVisitor(const std::string& rep)
    {
        relid = 0;
        wayid = 0;
        nodid = 0;
        tags = 0;
        relationIdIndex = new FileIndex<uint64_t,uint64_t>((rep + "/relationIdIndex") .c_str(), true);
        wayIdIndex      = new FileIndex<uint64_t,uint64_t>((rep +"/wayIdIndex").c_str(), true);
        nodeIdIndex     = new FileIndex<GeoPointNumberIndex, uint64_t>((rep + "/nodeIdIndex").c_str(), true);
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
    void startTag([[maybe_unused]] const std::vector<SeqBalise*>& tagStack, [[maybe_unused]] SeqBalise* b)
    {
    }
    void stringNode([[maybe_unused]] const std::vector<SeqBalise*>& tagStack, [[maybe_unused]] std::string& s)
    {
    }
    void endTag([[maybe_unused]] const std::vector<SeqBalise*>& tagStack, SeqBalise* b)
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

            if(tags)
            {
                GeoPointNumberIndex pt{ nodid++, {Coordinates::toNormalizedLon(b->keyValues["lon"]), Coordinates::toNormalizedLat(b->keyValues["lat"])}};
                nodeIdIndex->append(atoll((b->keyValues["id"]).c_str()),pt);
            } else {
                GeoPointNumberIndex pt{ 0, {Coordinates::toNormalizedLon(b->keyValues["lon"]), Coordinates::toNormalizedLat(b->keyValues["lat"])}};
                nodeIdIndex->append(atoll((b->keyValues["id"]).c_str()),pt);
            }
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
    return 0;
}
