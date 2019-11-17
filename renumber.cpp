#define MAIN
#include "helpers/Sequence.hpp"
#include "common/GeoTypes.hpp"
#include "Coordinates.hpp"
#include "helpers/FileIndex.hpp"

#define BALISENAME_RELATION  "relation"
#define BALISENAME_NODE      "node"
#define BALISENAME_WAY       "way"

using namespace fidx;
struct XmlVisitor
{
    uint64_t relid;
    uint64_t wayid;
    uint64_t nodid;

    FileIndex<uint64_t, uint64_t> *relationIdIndex;
    FileIndex<uint64_t, uint64_t> *wayIdIndex;
    FileIndex<GeoPointNumberIndex, uint64_t> *nodeIdIndex;

    XmlVisitor()
    {
        relid = 0;
        wayid = 0;
        nodid = 0;
        relationIdIndex = new FileIndex<uint64_t,uint64_t>("relationIdIndex", true);
        wayIdIndex      = new FileIndex<uint64_t,uint64_t>("wayIdIndex", true);
        nodeIdIndex     = new FileIndex<GeoPointNumberIndex, uint64_t>("nodeIdIndex", true);
    }

    ~XmlVisitor()
    {
        relationIdIndex->flush();
        wayIdIndex->flush();
        nodeIdIndex->flush();
        relationIdIndex->sort();
        wayIdIndex->sort();
        nodeIdIndex->sort();
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
    void endTag(const std::vector<SeqBalise*>& tagStack, SeqBalise* b)
    {
        if (b->baliseName == BALISENAME_RELATION)
        {
            //Record<uint64_t, uint64_t> record;
            //record.key = atoll((b->keyValues["id"]).c_str());
            //record.value = relid;
            //relationIdIndex->append(record.key, record.value);
            //relid++;
            relationIdIndex->append(atoll((b->keyValues["id"]).c_str()), relid++);
        }
        else if (b->baliseName == BALISENAME_WAY)
        {
            //Record<uint64_t, uint64_t> record;
            //record.key = atoll((b->keyValues["id"]).c_str());
            //record.value = wayid;
            //wayIdIndex->append(record.key, record.value);
            //wayid++;
            wayIdIndex->append(atoll((b->keyValues["id"]).c_str()), wayid++);
        }
        else if (b->baliseName == BALISENAME_NODE)
        {
            //Record<GeoPointNumberIndex, uint64_t> record;
            //record.key = atoll((b->keyValues["id"]).c_str());
            //record.value.number = nodid;
            //record.value.x = Coordinates::toNormalizedLon(b->keyValues["lon"]);
            //record.value.y = Coordinates::toNormalizedLat(b->keyValues["lat"]);
            //nodeIdIndex->append(record.key, record.value);
            //nodid++;
            nodeIdIndex->append(atoll((b->keyValues["id"]).c_str()),
                (GeoPointNumberIndex){ nodid++, Coordinates::toNormalizedLon(b->keyValues["lon"]), Coordinates::toNormalizedLat(b->keyValues["lat"])}
            );
        }
    }
};


int main(int argc, char *argv[])
{
    XmlVisitor v;
    XmlFileParser<XmlVisitor>::parseXmlFile(stdin,v);
}
