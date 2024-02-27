#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP
#include <vector>
#include <unordered_map>
#include <string>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#define  COMMENT_BEGIN   "<!--",4
#define  MANIFEST_BEGIN  "<?",2
#define  ENDTAG_BEGIN    "</",2
#define  STARTTAG_BEGIN  "<",1
#define  COMMENT_END     "-->",3
#define  STARTTAG_END    ">",1
#define  ENDTAG_END      "/>",2
#define  EQUALS          "=\"",2
#define  EQUALSSPACE     "= \"",3
#define  MANIFEST_END    "?>",2


template<short N> class Sequence
{
public:
    unsigned char c[N];

    bool check(const std::string& s)
    {
        if (s.length() > N) return false;
        else return  (memcmp(s.c_str(),c,s.length()) == 0);
    }

    bool check(const char* s, const unsigned char l)
    {
        if (l > N) return false;
        else return  (memcmp(s,c,l) == 0);
    }

    void append(const char a)
    {
        static_assert(N > 2);
        if constexpr (N > 2)
        {
            memmove(c,&c[1],(N-1));
            c[N-1] = a;
        }

    }

    Sequence()
    {
        memset(c,0,N);
    }
};



#define STATE_UNKNOWN   1
#define STATE_COMMENT   2
#define STATE_TAGNAME   3
#define STATE_TAGEND    4
#define STATE_ATTRNAME  5
#define STATE_ATTRVALUE 6
#define STATE_MANIFEST  7

#define BUFFLEN 16000


struct SeqBalise
{
    std::string baliseName;
    std::unordered_map<std::string,std::string> keyValues;
    uint64_t start;
    uint64_t size;
};

struct ParseContext
{
        int state = STATE_UNKNOWN;
        int previous_state = STATE_UNKNOWN;

        char attrName[2048];
        char attrValue[2048];
        char baliseName[2048];
        unsigned int attrNameLen = 0;
        unsigned int attrValueLen = 0;
        unsigned int baliseNameLen = 0;
        SeqBalise* curBalise;
        std::string stringNode = "";
        short skip = 0;
};

template<class VISITOR> class XmlFileParser
{

public:
    static void parseXmlFile(FILE* file, VISITOR& visitor)
    {
        ParseContext p;
        unsigned char buffer[BUFFLEN];
        FILE* fIn  = file;
        short initSeq=0;
        uint64_t done = 0;
        uint64_t len = 0;
        Sequence<4> seq;
        //char cData;
        std::vector<SeqBalise*> tagStack;
        unsigned char* c;
        len = fread(buffer,1,BUFFLEN,fIn);
        while(len)
        {
            for(c=buffer; c < (buffer + len) ; c++)
            {
                done++;
                if((done & 0xFFFFFFF) == 0)
                {
                    visitor.log(done);
                }

                seq.append(*c);
                if(initSeq < 3) initSeq++;
                else selectXmlFile(seq, visitor, &tagStack, p);
            }
            len = fread(buffer,1,BUFFLEN,fIn);
        }
        for(int i=0; i < 15; i++)
        {
            seq.append(0);
            selectXmlFile(seq, visitor, &tagStack, p);
        }

    }


    static void parseXmlIstream(std::istream& my_stream, VISITOR& visitor)
    {
        ParseContext p;
        unsigned char buffer[BUFFLEN];
        short initSeq=0;
        uint64_t done = 0;
        uint64_t len = 0;
        Sequence<4> seq;
        std::vector<SeqBalise*> tagStack;
        unsigned char* c;
        //len = fread(buffer,1,BUFFLEN,fIn);
        my_stream.read((char*)buffer,BUFFLEN);
        len = my_stream.gcount();
        while(len)
        {
            for(c=buffer; c < (buffer + len) ; c++)
            {
                done++;
                if((done & 0xFFFFFFF) == 0)
                {
                    visitor.log(done);
                }

                seq.append(*c);
                if(initSeq < 3) initSeq++;
                else selectXmlFile(seq, visitor, &tagStack, p);
            }
            my_stream.read((char*)buffer,BUFFLEN);
            len = my_stream.gcount();
        }
        for(int i=0; i < 15; i++)
        {
            seq.append(0);
            selectXmlFile(seq, visitor, &tagStack, p);
        }

    }







private:
    static void selectXmlFile(Sequence<4> sq, VISITOR& visitor, std::vector<SeqBalise*>* tagStack, ParseContext& p)
    {

        if(p.skip)
        {
            p.skip--;
            return;
        }

        switch(p.state)
        {
        case STATE_UNKNOWN:
            if(sq.check(COMMENT_BEGIN))
            {
                p.previous_state = p.state;
                p.state = STATE_COMMENT;
                p.skip = 3;
            }
            else if (sq.check(MANIFEST_BEGIN))
            {
                p.state = STATE_MANIFEST;
                p.skip = 1;
            }
            else if (sq.check(ENDTAG_BEGIN))
            {
                p.state = STATE_TAGEND;
                p.curBalise = tagStack->back();
                tagStack->pop_back();
                if(p.stringNode.size())
                {
                    visitor.stringNode(*tagStack, p.stringNode);
                    p.stringNode = "";
                }
                if(p.stringNode.size())
                {
                    visitor.stringNode(*tagStack, p.stringNode);
                    p.stringNode = "";
                }
                visitor.endTag(*tagStack, p.curBalise);
                delete(p.curBalise);
                p.skip = 1;
            }
            else if (sq.check(STARTTAG_BEGIN))
            {
                p.state = STATE_TAGNAME;
                p.curBalise = new SeqBalise;
                p.baliseNameLen = 0;
                //stringName="";
                if(p.stringNode.size())
                {
                    visitor.stringNode(*tagStack, p.stringNode);
                    p.stringNode = "";
                }
            }
            else
            {
                 p.stringNode += sq.c[0];
                 //std::cout << " -----[" << stringNode << "]\n";
            }
            break;

        case STATE_COMMENT:
            if (sq.check(COMMENT_END))
            {
                p.state = p.previous_state;
                p.skip = 2;
            }
            break;

        case STATE_TAGNAME:
            if(sq.check(STARTTAG_END))
            {
                p.state = STATE_UNKNOWN;
                p.baliseName[p.baliseNameLen] = 0;
                p.curBalise->baliseName = p.baliseName;
                visitor.startTag(*tagStack, p.curBalise);
                tagStack->push_back(p.curBalise);
            }
            else if(sq.check(ENDTAG_END))
            {
                p.baliseName[p.baliseNameLen] = 0;
                p.curBalise->baliseName = p.baliseName;
                visitor.startTag(*tagStack, p.curBalise);
                visitor.endTag(*tagStack, p.curBalise);
                delete(p.curBalise);
                p.skip = 1;
                p.state = STATE_UNKNOWN;
            }
            else if(isspace(sq.c[0]))
            {
                p.state = STATE_ATTRNAME;
                p.attrNameLen = 0;
            }
            else
            {
                if(!isspace(sq.c[0]))
                {
                    if(p.baliseNameLen < 2047) p.baliseName[p.baliseNameLen ++] = sq.c[0];
                }
            }
            break;


        case STATE_TAGEND:
            if(sq.check(STARTTAG_END))
            {
                p.state = STATE_UNKNOWN;
            }
            break;

        case STATE_ATTRNAME:
            if (sq.check(STARTTAG_END) || sq.check(ENDTAG_END))
            {
                p.state = STATE_UNKNOWN;
                if(p.attrNameLen)
                {
                    p.attrValue[p.attrValueLen] = 0;
                    p.attrName[p.attrNameLen] = 0;
                    p.curBalise->keyValues[p.attrName] = p.attrValue;
                }
                if(sq.check(STARTTAG_END))
                {
                    p.baliseName[p.baliseNameLen] = 0;
                    p.curBalise->baliseName = p.baliseName;
                    visitor.startTag(*    tagStack, p.curBalise);
                    tagStack->push_back(p.curBalise);
                }
                else
                {
                    p.baliseName[p.baliseNameLen] = 0;
                    p.curBalise->baliseName = p.baliseName;
                    visitor.startTag(*tagStack, p.curBalise);
                    visitor.endTag(*tagStack, p.curBalise);
                    delete(p.curBalise);
                    p.skip=1;
                }
            }
            else if (sq.check(EQUALS))
            {
                p.state = STATE_ATTRVALUE;
                p.attrValueLen = 0;
                p.skip = 1;
            }
            else if (sq.check(EQUALSSPACE))
            {
                p.state = STATE_ATTRVALUE;
                p.attrValueLen = 0;
                p.skip = 2;
            }
            else
            {
                if(!isspace(sq.c[0]))
                {
                    if(p.attrNameLen < 2047)    p.attrName[p.attrNameLen ++] = sq.c[0];
                }
            }
            break;

        case STATE_ATTRVALUE:
            if(sq.c[0] == '"')
            {
                p.state = STATE_ATTRNAME;
                if(p.attrNameLen)
                {
                    p.attrValue[p.attrValueLen] = 0;
                    p.attrName[p.attrNameLen] = 0;
                    p.curBalise->keyValues[p.attrName] = p.attrValue;
                }
                p.attrNameLen = 0;
                p.attrValueLen = 0;
            }
            else
            {
                if(p.attrValueLen < 2048)
                {
                    p.attrValue[p.attrValueLen++] = sq.c[0];
                }
            }
            break;


        case STATE_MANIFEST:
            if (sq.check(MANIFEST_END))
            {
                p.state=STATE_UNKNOWN;
                p.skip = 1;
            }
            break;

        }
    }
};



#endif
