#include <map>
#include <vector>
#include "helpers/Sequence.hpp"
#include "helpers/FileIndex.hpp"
#include "common/constants.hpp"
#include "common/GeoTypes.hpp"
#include "Coordinates.hpp"
#include <math.h>


using namespace fidx;
using namespace std;


struct field_descriptor
{
    char name[33];
    char field_type;
    unsigned char field_length;
    unsigned char field_decimal_count;
    //unsigned char mdx_field;
};

struct dbf_header
{
    unsigned char version;
    unsigned char upd_year;
    unsigned char upd_month;
    unsigned char upd_day;
    uint32_t      nbr_of_records;
    uint16_t      record_size;
    unsigned char filler1[56];
    field_descriptor field_descriptors[1024];
    uint32_t field_count;
   int read(FILE* f)
   {
    unsigned char tmp[4096];
    if(fread(&version, 1, 1, f) != 1) return -1;
    if(fread(&upd_year, 1, 1, f) != 1) return -1;
    if(fread(&upd_month, 1, 1, f) != 1) return -1;
    if(fread(&upd_day, 1, 1, f) != 1) return -1;

    if(fread(tmp, 4, 1, f) != 1)
    {
       return -1;
    }
    else
    {
        nbr_of_records = tmp[0] + 256 * tmp[1] + 256*256*tmp[2]  + 256*256*256*tmp[3];
    }
    if(fread(tmp, 2, 1, f) != 1)
    {
       return -1;
    }
    else
    {
        record_size = tmp[0] + tmp[1] * 0xFF;
    }
    
    if(fread(&filler1, 22, 1, f) != 1) return -1;

    if(fread(tmp, 1, 1, f) != 1) return -1;
    unsigned int count = 0;
    while(tmp[0] != 0x0d)
    {
        unsigned char tmp2[128];
        field_descriptors[count].name[0] = tmp[0];
        if(fread(field_descriptors[count].name + 1, 10, 1, f)!=1) return -1;
        field_descriptors[count].name[11] = 0;
        if(fread(&field_descriptors[count].field_type, 1, 1, f)!=1) return -1;
        if(fread(tmp2, 4, 1, f)!=1) return -1;
        if(fread(&field_descriptors[count].field_length, 1, 1, f)!=1) return -1;
        if(fread(&field_descriptors[count].field_decimal_count, 1, 1, f)!=1) return -1;
        if(fread(tmp2, 14, 1, f)!=1) return -1;
        count++;
        if(fread(tmp, 1, 1, f) != 1) return -1;
    } 
    field_count = count;
    return 0;
   }
   int read_record(FILE* f, FileRawVarData<GeoString>*  baliseTags)
   {
       char del;
       if(fread(&del, 1, 1, f)!=1) return -1;
       for(unsigned int i = 0; i < field_count; i++)
       {
           char value[1024];
           memset(value,0,1024);
           if(fread(value, field_descriptors[i].field_length, 1, f) != 1) return -1;
           char value2[1024];
           int index = 0;
           while (value[index]==' ' && index < 1023) index++;
           int index2 = 1023;
           while((value[index2]==0 || value[index2] == ' ') && index2 > 0) index2--;
           memset (value2,0,1024);
           if(index2 >= index)
              memcpy(value2, value + index, (index2 - index + 1));
            GeoString s;
            s.value = field_descriptors[i].name ;
            baliseTags->append(s);
            s.value = value2;
            baliseTags->append(s);
       }
       return 0;
   }
};




struct shp_file
{
    std::string name;
    std::string tag;
    std::string value;
    std::string projection;
};

struct ShpVisitor
{
    std::vector<shp_file> shpFiles;
    void log(uint64_t done){};
    void startTag(std::vector<SeqBalise*>& tagStack, SeqBalise* b)
    {
        if (b->baliseName == "shp_file")
        {
             shpFiles.push_back({b->keyValues["name"], b->keyValues["tag"], b->keyValues["value"], b->keyValues["projection"]});
        }

    }
    void endTag(std::vector<SeqBalise*>& tagStack, SeqBalise* b)
    {
    }
    
    void stringNode(const std::vector<SeqBalise*>& tagStack, std::string& s)
    {
	}
};



double getDouble(unsigned char* c)
{
    uint64_t sign, exp, mant;		
    sign = (c[7] & 0b10000000) >> 7;
    exp  = c[7] & 0b01111111;
    exp <<= 4;
    exp += (c[6] & 0b11110000) >> 4;
    mant = c[6] & 0b00001111;
    mant  <<= 8;
    mant += c[5];
    mant  <<= 8;
    mant += c[4];
    mant  <<= 8;
    mant += c[3];
    mant  <<= 8;
    mant += c[2];
    mant  <<= 8;
    mant += c[1];
    mant  <<= 8;
    mant += c[0];
    if( sign==0 && exp==0 && mant==0) return 0;
    uint64_t decalage = 1023;	
    double result = 1.0 + mant/(double)(0b10000000000000000000000000000000000000000000000000000);
    if (sign) result *= -1;
    while (exp > decalage) { result *= 2; exp--; }
    while (exp < decalage) { result /= 2; exp++; }
    return result;
}

uint32_t getUnsignedInteger32(unsigned char* c, bool isBigEndian)
{
	uint32_t result = 0;
	if(!isBigEndian)
	{
		result = c[3];
		result <<= 8;
		result += c[2];
		result <<= 8;
		result += c[1];
		result <<= 8;
		result += c[0];
	}
	else
	{
		result = c[0];
		result <<= 8;
		result += c[1];
		result <<= 8;
		result += c[2];
		result <<= 8;
		result += c[3];
	}
	return result;
}


struct XmlVisitor
{
    uint64_t relid;
    uint64_t wayid;
    uint64_t nodid;

    bool isRel = false;;
    bool isWay = false;
    bool isNod = false;

    FileRawData<GeoPoint>*  wayPoints;
    FileRawData<GeoMember>* relMembers;
    FileRawVarData<GeoString>*  baliseTags;

    FileRawIndex<GeoIndex>      *relationIndex;
    FileRawIndex<GeoWayIndex>   *wayIndex;
    FileRawIndex<GeoPointIndex> *nodeIndex;


    FileIndex<uint64_t, uint64_t> *relationIdIndex;
    FileIndex<uint64_t, uint64_t> *wayIdIndex;
    FileIndex<GeoPointNumberIndex, uint64_t> *nodeIdIndex;
    BaliseType curBalise;

    XmlVisitor(std::string rep)
    {

        relationIdIndex = new FileIndex<uint64_t,uint64_t>(rep + "/relationIdIndex", false);
        wayIdIndex      = new FileIndex<uint64_t,uint64_t>(rep + "/wayIdIndex",      false);
        nodeIdIndex     = new FileIndex<GeoPointNumberIndex,uint64_t>(rep + "/nodeIdIndex",     false);

        relationIndex   = new FileRawIndex<GeoIndex>(rep + "/relationIndex",  true);
        wayIndex        = new FileRawIndex<GeoWayIndex>(rep + "/wayIndex",       true);
        nodeIndex       = new FileRawIndex<GeoPointIndex>(rep + "/nodeIndex", true);

        wayPoints  = new FileRawData<GeoPoint>(rep + "/wayPoints", true);
        relMembers = new FileRawData<GeoMember>(rep + "/relMembers", true);
        baliseTags = new FileRawVarData<GeoString>(rep + "/baliseTags", true);

    }

    ~XmlVisitor()
    {

        relationIndex->flush();
        wayIndex->flush();
        nodeIndex->flush();

        wayPoints->flush();
        relMembers->flush();
        baliseTags->flush();

        delete relationIndex;
        delete wayIndex;
        delete nodeIndex;

        delete relationIdIndex;
        delete wayIdIndex;
        delete nodeIdIndex;

        delete wayPoints;
    }

    void log(uint64_t done)
    {
        std::cerr << " done " << (done >> UINT64_C(20)) << "Mio.\t relations " << relationIndex->itemCount << "\tways " << wayIndex->itemCount << "\tnodes " << nodeIndex->itemCount<< "\n" << std::flush;
    }

    void stringNode(const std::vector<SeqBalise*>& tagStack, std::string& s)
    {
	}

    void startTag(const std::vector<SeqBalise*>& tagStack, SeqBalise* b)
    {
        if ((b->baliseName == BALISENAME_TAG)&&(isNod || isWay || isRel))
        {
            GeoString s;
            s.value = b->keyValues["k"];
            baliseTags->append(s);
            s.value = b->keyValues["v"];
            baliseTags->append(s);
        }
        else if (b->baliseName == BALISENAME_NODE)
        {
            curBalise = BaliseType::point;
            baliseTags->startBatch();
            isNod = true;
        }
        else if (b->baliseName == BALISENAME_ND)
        {
            uint64_t ref = atoll(b->keyValues["ref"].c_str());
            fidx::Record<GeoPointNumberIndex, uint64_t> recp;
            bool res = nodeIdIndex->find(ref, &recp);
            if (res)
            {
                GeoPoint p;
                p.x = recp.value.x;
                p.y = recp.value.y;
                wayPoints->append(p);
            }
        }
        else if (b->baliseName == BALISENAME_WAY)
        {
            curBalise = BaliseType::way;
            wayPoints->startBatch();
            baliseTags->startBatch();
            isWay = true;
        }
        else if (b->baliseName == BALISENAME_MEMBER)
        {
            uint64_t ref = atoll(b->keyValues["ref"].c_str());
            GeoMember m;
            m.type = unknown;
            m.id=UINT64_C(0xFFFFFFFFFFFFFFFF);
            if(b->keyValues["type"] == "node")
            {
                m.type = point;
                uint64_t ref = atoll(b->keyValues["ref"].c_str());
                fidx::Record<GeoPointNumberIndex, uint64_t> recp;
                bool res = nodeIdIndex->find(ref, &recp);
                if(res)
                {
                    m.id = recp.value.number;
                }
                else
                {
                   // std::cerr << "data inconsistent !\n";
                   // exit(1);
                }

            }
            else if(b->keyValues["type"] == "way")
            {
                m.type = way;
                fidx::Record<uint64_t, uint64_t> recp;
                bool res = wayIdIndex->find(ref, &recp);
                if(res)
                {
                    m.id = recp.value;
                }
                else
                {
                   // std::cerr << "data inconsistent !\n";
                   // exit(1);
                }
            }
            else if(b->keyValues["type"] == "relation")
            {   
                m.type = relation;
                fidx::Record<uint64_t, uint64_t> recp;
                bool res = relationIdIndex->find(ref, &recp);
                if(res)
                {
                    m.id = recp.value;
                }
                else
                {
                    //std::cerr << "data inconsistent !\n";
                    //exit(1);
                }
            }
            if(m.id != UINT64_C(0xFFFFFFFFFFFFFFFF))
                relMembers->append(m);
        }
        else if (b->baliseName == BALISENAME_RELATION)
        {
            isRel = true;
            curBalise = BaliseType::relation;
            baliseTags->startBatch();
            relMembers->startBatch();
        }
    }

    void endTag(const std::vector<SeqBalise*>& tagStack, SeqBalise* b)
    {
        if (b->baliseName == BALISENAME_TAG)
        {
        }
        else if (b->baliseName == BALISENAME_NODE)
        {
            isNod = false;
            GeoPointIndex nodeRecord;
            nodeRecord.x = Coordinates::toNormalizedLon(b->keyValues["lon"]);
            nodeRecord.y = Coordinates::toNormalizedLat(b->keyValues["lat"]);
            nodeRecord.tstart = baliseTags->startCount;
            nodeRecord.tsize = baliseTags->itemCount  - baliseTags->startCount;
            nodeIndex->append(nodeRecord);
        }
        else if (b->baliseName == BALISENAME_ND)
        {
        }
        else if (b->baliseName == BALISENAME_WAY)
        {
            isWay = false;
            GeoWayIndex wayRecord;
            wayRecord.pstart = wayPoints->startCount;
            wayRecord.psize = wayPoints->itemCount  - wayPoints->startCount;
            wayRecord.tstart = baliseTags->startCount;
            wayRecord.tsize = baliseTags->itemCount  - baliseTags->startCount;
            wayIndex->append(wayRecord);
        }
        else if (b->baliseName == BALISENAME_MEMBER)
        {
        }
        else if (b->baliseName == BALISENAME_RELATION)
        {
            isRel = false;
            GeoIndex relationRecord;
            relationRecord.tstart = baliseTags->startCount;
            relationRecord.tsize = baliseTags->itemCount  - baliseTags->startCount;
            relationRecord.mstart = relMembers->startCount;
            relationRecord.msize = relMembers->itemCount  - relMembers->startCount;
            relationIndex->append(relationRecord);
        }
    }

    void addShape(std::string filename, std::string tag, std::string value, std::string projection)
    {
    std::string filename_shp = filename+".shp";
    std::string filename_dbf = filename+".dbf";
    FILE* shp = fopen(filename_shp.c_str(),"r");
    FILE* dbf = fopen(filename_dbf.c_str(),"r");
    if(dbf == NULL)
    {
       std::cerr << "dbf file missing \n";
       return;
    } 
    
    if(shp == NULL)
    {
       std::cerr << "shp file missing \n";
       return;
    } 
    
    unsigned char header[100], recordheader[8];
    uint64_t len = fread(header,100,1,shp);


    dbf_header dbfheader;
    if(dbfheader.read(dbf) != 0) std::cerr << "error while reading dbf file !\n";;



    if(len > 0)
    {
        while(fread(recordheader, 8, 1, shp) > 0)
        {
            uint32_t recordnumber = getUnsignedInteger32(recordheader, true); 		
            uint32_t recordlength = getUnsignedInteger32(recordheader + 4, true);
            if((recordnumber & 0xFF) == 0) std::cout << "record n° : " <<  recordnumber << "\n";
            unsigned char* recordcontent = (unsigned char*) malloc(recordlength*2);
            uint64_t len = fread(recordcontent,2, recordlength, shp);
            if(len == 0) std::cerr << "read error !\n";
            uint64_t shapetype = getUnsignedInteger32(recordcontent, false);
            if(shapetype == 5 || shapetype == 3)
            {

                baliseTags->startBatch();
                relMembers->startBatch();
                baliseTags->startBatch();
                GeoString s;
                s.value = tag;
                baliseTags->append(s);
                s.value = value;
                baliseTags->append(s);

                if(dbfheader.read_record(dbf, baliseTags) != 0) std::cout << "error while reading dbf file !\n";;

                uint64_t numparts  = getUnsignedInteger32(recordcontent + 36, false);
                uint64_t numpoints = getUnsignedInteger32(recordcontent + 40, false);
                for(uint64_t i = 0; i < numparts; i++)
                {

                    wayPoints->startBatch();

                    uint64_t firstindex = getUnsignedInteger32(recordcontent + (44 + 4*i), false);
                    uint64_t lastindex = numpoints;
                    if(i != (numparts - 1)) lastindex = getUnsignedInteger32(recordcontent + (48 + 4*i), false);
                    for(unsigned int j = firstindex; j < lastindex; j++)
                    {
                        double x,y;
                        uint64_t offset = (44 + 4 * numparts) + j * 16;
                        if(projection == "3857")
                        {
                            x = getDouble(recordcontent + offset) * 180.0 / 20037508.34;
                            y = getDouble(recordcontent + offset + 8);
                            y = atan(exp(y * M_PI / 20037508.34)) * (360 / M_PI) - 90;
                        }
                        else
                        { 
                            x = getDouble(recordcontent + offset);
                            y = getDouble(recordcontent + offset + 8);
                        }
                        GeoPoint p;
                        
                        p.x = Coordinates::toNormalizedLon(std::to_string(x));
                        p.y = Coordinates::toNormalizedLat(std::to_string(y));
                        wayPoints->append(p);

                    }

                    GeoWayIndex wayRecord;
                    wayRecord.pstart = wayPoints->startCount;
                    wayRecord.psize = wayPoints->itemCount  - wayPoints->startCount;
                    wayRecord.tstart = 0;
                    wayRecord.tsize = 0;
                    wayIndex->append(wayRecord);
                    GeoMember m;
                    m.type = way;
                    m.id = wayIndex->itemCount - 1;
                    relMembers->append(m);

                }

                GeoIndex relationRecord;
                relationRecord.tstart = baliseTags->startCount;
                relationRecord.tsize = baliseTags->itemCount - baliseTags->startCount;
                relationRecord.mstart = relMembers->startCount;
                relationRecord.msize = relMembers->itemCount  - relMembers->startCount;
                relationIndex->append(relationRecord);

            }
            free(recordcontent);
        }
    }
    fclose(shp);
    fclose(dbf);
    baliseTags->startBatch();
    }

};




int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cerr << "path argument is missing\n";
        exit(1);
    }
    
    std::string rep = std::string(argv[1]);

    XmlVisitor* v = new XmlVisitor(rep);
    XmlFileParser<XmlVisitor>::parseXmlFile(stdin,*v);
    ShpVisitor v2;
    FILE* f =fopen((rep + "/config.xml").c_str(),"r");
    if(f == NULL)
    {
        std::cout << "couldn't open "<< rep + "/config.xml" << "\n";
        exit(1);
    }
    XmlFileParser<ShpVisitor>::parseXmlFile(f,v2);
    for(shp_file f : v2.shpFiles)
    {
        std::cout << "load shp file : " << rep + "/" + f.name << " with tag : " << f.tag << " value : " << f.value << "\n";
        v->addShape(rep + "/" + f.name, f.tag, f.value, f.projection);
    }
    fclose(f);
    delete v;
 }
