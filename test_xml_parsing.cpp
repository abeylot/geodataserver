#include "helpers/Sequence.hpp"
//sample of program using xml this parsing header

struct SampleXmlVisitor
{
    SampleXmlVisitor()
    {
    }

   virtual  ~SampleXmlVisitor()
    {
    }

    void log(uint64_t done)
    {
        std::cerr << " done " << (done >> UINT64_C(20)) << "Mio\n" << std::flush;
    }

    void stringNode(const std::vector<SeqBalise*>& tagStack, std::string& s)
    {
       std::cout << "tag contains string : [" << s << "]\n";
	}

    void startTag(const std::vector<SeqBalise*>& tagStack, SeqBalise* b)
    {
        std::cout << "starting tag : " <<  b->baliseName << "\n";
        for (auto attribute : b->keyValues)
        {
            std::cout << "attribute : " << attribute.first << " value : " << attribute.second << "\n";
        }
    }

    void endTag(const std::vector<SeqBalise*>& tagStack, SeqBalise* b)
    {
        std::cout << "ending tag : " <<  b->baliseName << "\n";
    }

};

static void testXmlParsing()
{
   SampleXmlVisitor v;
   std::string s =         "<level1>"
                           "    my string 1 "
                           "    <level2 attr1=\"val1\" attr2=\"val2\"/>"
                           "</level1>";

   std::stringstream my_stream(s);
   XmlFileParser<SampleXmlVisitor>::parseXmlIstream(my_stream, v);
}

 
int main()
{
    testXmlParsing();
}
