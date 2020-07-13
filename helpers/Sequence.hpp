#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP
#include <vector>
#include <map>
#include <string>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

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

        memmove(c,&c[1],(N-1));
        c[N-1] = a;

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
    std::map<std::string,std::string> keyValues;
    uint64_t start;
    uint64_t size;
};

template<class VISITOR> class XmlFileParser
{

public:
    static void parseXmlFile(FILE* file, VISITOR& visitor)
    {
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
                else selectXmlFile(seq, visitor, &tagStack);
            }
            len = fread(buffer,1,BUFFLEN,fIn);
        }
        for(int i=0; i < 15; i++)
        {
            seq.append(0);
            selectXmlFile(seq, visitor, &tagStack);
        }

    }







private:
    static void selectXmlFile(Sequence<4> sq, VISITOR& visitor, std::vector<SeqBalise*>* tagStack)
    {
        static int state = STATE_UNKNOWN;
        static int previous_state = STATE_UNKNOWN;

        static char attrName[2048];
        static char attrValue[2048];
        static char baliseName[2048];
        static unsigned int attrNameLen = 0;
        static unsigned int attrValueLen = 0;
        static unsigned int baliseNameLen = 0;
        static SeqBalise* curBalise;
        static std::string stringNode = "";


        static short skip = 0;

        if(skip)
        {
            skip--;
            return;
        }

        switch(state)
        {
        case STATE_UNKNOWN:
            if(sq.check(COMMENT_BEGIN))
            {
                previous_state = state;
                state = STATE_COMMENT;
                skip = 3;
            }
            else if (sq.check(MANIFEST_BEGIN))
            {
                state = STATE_MANIFEST;
                skip = 1;
            }
            else if (sq.check(ENDTAG_BEGIN))
            {
                state = STATE_TAGEND;
                curBalise = tagStack->back();
                tagStack->pop_back();
                visitor.stringNode(*tagStack, stringNode);
                stringNode = "";
                visitor.endTag(*tagStack, curBalise);
                delete(curBalise);
                skip = 1;
            }
            else if (sq.check(STARTTAG_BEGIN))
            {
                state = STATE_TAGNAME;
                curBalise = new SeqBalise;
                baliseNameLen = 0;
                //stringName="";
                visitor.stringNode(*tagStack, stringNode);
                stringNode="";
            }
	        else
            {
                 stringNode += sq.c[0];
                 //std::cout << " -----[" << stringNode << "]\n";
            }
            break;

        case STATE_COMMENT:
            if (sq.check(COMMENT_END))
            {
                state = previous_state;
                skip = 2;
            }
            break;

        case STATE_TAGNAME:
            if(sq.check(STARTTAG_END))
            {
                state = STATE_UNKNOWN;
                baliseName[baliseNameLen] = 0;
                curBalise->baliseName = baliseName;
                visitor.startTag(*tagStack, curBalise);
                tagStack->push_back(curBalise);
            }
            else if(sq.check(ENDTAG_END))
            {
                baliseName[baliseNameLen] = 0;
                curBalise->baliseName = baliseName;
                visitor.startTag(*tagStack, curBalise);
                visitor.endTag(*tagStack, curBalise);
                delete(curBalise);
                skip = 1;
                state = STATE_UNKNOWN;
            }
            else if(isspace(sq.c[0]))
            {
                state = STATE_ATTRNAME;
                attrNameLen = 0;
            }
            else
            {
                if(!isspace(sq.c[0]))
                {
                    if(baliseNameLen < 2047) baliseName[baliseNameLen ++] = sq.c[0];
                }
            }
            break;


        case STATE_TAGEND:
            if(sq.check(STARTTAG_END))
            {
                state = STATE_UNKNOWN;
            }
            break;

        case STATE_ATTRNAME:
            if (sq.check(STARTTAG_END) || sq.check(ENDTAG_END))
            {
                state = STATE_UNKNOWN;
                if(attrNameLen)
                {
                    attrValue[attrValueLen] = 0;
                    attrName[attrNameLen] = 0;
                    curBalise->keyValues[attrName] = attrValue;
                }
                if(sq.check(STARTTAG_END))
                {
                    baliseName[baliseNameLen] = 0;
                    curBalise->baliseName = baliseName;
                    visitor.startTag(*tagStack, curBalise);
                    tagStack->push_back(curBalise);
                }
                else
                {
                    baliseName[baliseNameLen] = 0;
                    curBalise->baliseName = baliseName;
                    visitor.startTag(*tagStack, curBalise);
                    visitor.endTag(*tagStack, curBalise);
                    delete(curBalise);
                    skip=1;
                }
            }
            else if (sq.check(EQUALS))
            {
                state = STATE_ATTRVALUE;
                attrValueLen = 0;
                skip = 1;
            }
            else if (sq.check(EQUALSSPACE))
            {
                state = STATE_ATTRVALUE;
                attrValueLen = 0;
                skip = 2;
            }
            else
            {
                if(!isspace(sq.c[0]))
                {
                    if(attrNameLen < 2047)    attrName[attrNameLen ++] = sq.c[0];
                }
            }
            break;

        case STATE_ATTRVALUE:
            if(sq.c[0] == '"')
            {
                state = STATE_ATTRNAME;
                if(attrNameLen)
                {
                    attrValue[attrValueLen] = 0;
                    attrName[attrNameLen] = 0;
                    curBalise->keyValues[attrName] = attrValue;
                }
                attrNameLen = 0;
                attrValueLen = 0;
            }
            else
            {
                if(attrValueLen < 2048)
                {
                    attrValue[attrValueLen++] = sq.c[0];
                }
            }
            break;


        case STATE_MANIFEST:
            if (sq.check(MANIFEST_END))
            {
                state=STATE_UNKNOWN;
                skip = 1;
            }
            break;

        }
    }




};

#endif
