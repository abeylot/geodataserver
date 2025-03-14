/*
 * File:   HttpEncoder.cpp
 *
 */

#include <sstream>
#include <iostream>
#include "HttpEncoder.hpp"


HttpEncoder::HttpEncoder()
{
}

HttpEncoder::~HttpEncoder()
{
}

std::string HttpEncoder::urlDecode(const std::string& in)
{
    int awaitingHex = 0;
    unsigned char curchar = 0;
    std::string out = "";
    for(size_t i = 0; i < in.length(); i++)
    {
        if (in.c_str()[i] == '%') awaitingHex = 2;
        else if (in.c_str()[i] == '+') out += ' ';
        else if (awaitingHex)
        {
            curchar <<= 4;
            if(in.c_str()[i] == '1') curchar += 1;
            else if(in.c_str()[i] == '2') curchar += 2;
            else if(in.c_str()[i] == '3') curchar += 3;
            else if(in.c_str()[i] == '4') curchar += 4;
            else if(in.c_str()[i] == '5') curchar += 5;
            else if(in.c_str()[i] == '6') curchar += 6;
            else if(in.c_str()[i] == '7') curchar += 7;
            else if(in.c_str()[i] == '8') curchar += 8;
            else if(in.c_str()[i] == '9') curchar += 9;
            else if(in.c_str()[i] == 'a') curchar += 10;
            else if(in.c_str()[i] == 'A') curchar += 10;
            else if(in.c_str()[i] == 'b') curchar += 11;
            else if(in.c_str()[i] == 'B') curchar += 11;
            else if(in.c_str()[i] == 'c') curchar += 12;
            else if(in.c_str()[i] == 'C') curchar += 12;
            else if(in.c_str()[i] == 'd') curchar += 13;
            else if(in.c_str()[i] == 'D') curchar += 13;
            else if(in.c_str()[i] == 'e') curchar += 14;
            else if(in.c_str()[i] == 'E') curchar += 14;
            else if(in.c_str()[i] == 'f') curchar += 15;
            else if(in.c_str()[i] == 'F') curchar += 15;
            awaitingHex--;
            if(!awaitingHex){
                out += curchar;
                curchar = 0;
           }
        }
        else
        {
            out += in.c_str()[i];
        }
    }
    return out;
}

std::shared_ptr<Msg> HttpEncoder::encode(std::string* in)
{
    auto res = std::make_shared<Msg>();
    if(*in == "") return res;
    //printf("[%s]\n",in->c_str());
    Record *rcd = new Record();
    Record *rcdParms = new Record();
    res->addRecord(rcd);

    std::string sHead="";
    std::string sBody="";

    size_t iFound = in->find(std::string("\r\n\r\n"));
    if(iFound == std::string::npos)
    {
        sHead = *in;
        sBody="";
    }
    else
    {
        sHead = in->substr(0,iFound+2);
        sBody = in->substr(iFound+4);
    }

    // add method to first record
    std::string method;
    size_t iEndLine = sHead.find("\r\n");
    if(iEndLine == std::string::npos)  iEndLine = sHead.size();
    size_t iDone = 0;
    std::string sLine = sHead.substr(iDone,iEndLine - iDone);
    std::cout << "<--- " << sLine << "\n";
    iDone = iEndLine;
    size_t pSep = sLine.find("?");
    size_t iSep = sLine.find(" ");
    if (pSep != std::string::npos)
    {
        //retrieve parameters
        std::string parameterString = sLine.substr(pSep +1);
        size_t lSep = parameterString.find(" ");
        if(lSep != std::string::npos) parameterString = parameterString.substr(0,lSep);
        size_t jSep;
        do
        {
            jSep = parameterString.find("&");
            std::string myParm;
            if(jSep == std::string::npos)
            {
                myParm = parameterString;
            }
            else
            {
                myParm = parameterString.substr(0, jSep);
                parameterString = parameterString.substr(jSep+1);
            }
            rcdParms->addBlock(myParm);
        }
        while (jSep != std::string::npos);
    }
    if(iSep == std::string::npos)  iSep = sHead.size();
    method = sLine.substr(0,iSep);
    std::string sBlock = "Method=" + method;
    rcd->addBlock(sBlock);


    // add URL to first record.
    size_t iUrlEnd = sLine.find(" ",iSep+1);
    if (pSep != std::string::npos) iUrlEnd = pSep;
    if(iUrlEnd != std::string::npos)
    {
        std::string sURL = sLine.substr(iSep+1,iUrlEnd - (iSep+1));
        sBlock = "URL=" + sURL;
    }
    else
    {
        iUrlEnd = sLine.size();
    }
    rcd->addBlock(sBlock);

    // add Http Version to first record
    std::string sVersion = sLine.substr(iUrlEnd,sLine.length()-(iUrlEnd));
    sBlock = "HTTPVersion=" + sVersion;
    rcd->addBlock(sBlock);


    // add tags

    iDone +=1;
    while((iEndLine = sHead.find("\r\n",iDone)) != std::string::npos)
    {
        sLine = sHead.substr(iDone+1,iEndLine - (iDone+1));
        iSep = sLine.find(": ");
        if(iSep != std::string::npos)
        {
            sBlock = sLine.substr(0,iSep) + "=" + sLine.substr(iSep+2);
            rcd->addBlock(sBlock);
        }
        iDone = iEndLine+1;
    }

    // get data
    Record* rcdBody = new Record();
    if(
        (rcd->getNamedValue("content-type") == "application/x-www-form-urlencoded")
        || (rcd->getNamedValue("Content-Type") == "application/x-www-form-urlencoded")
        || (rcd->getNamedValue("Content-type") == "application/x-www-form-urlencoded")
        )
    {
        iDone = 0;
        while((iSep = sBody.find("&",iDone)) != std::string::npos)
        {
            size_t iSep2 = sBody.find("=");
            if(iSep2 != std::string::npos)
            {
                sBlock = sBody.substr(iDone,iSep - iDone);
                rcdBody->addBlock(sBlock);
            }
            iDone = iSep+1;
        }
        sBlock = sBody.substr(iDone);
        rcdBody->addBlock(sBlock);

    }
    else
    {
        rcdBody->addBlock(sBody);
    }

    res->addRecord(rcdBody);
    res->addRecord(rcdParms);

    return res;

}

std::string* HttpEncoder::decode(std::shared_ptr<Msg> outMsg) const
{
    std::string* res = new std::string();
    if (outMsg->getRecordCount() > 1)
    {
        // build http header
        std::string sVersion = outMsg->getRecord(0)->getNamedValue("HTTPVersion");
        if(sVersion.length() == 0)
        {
            *res += std::string("HTTP/1.1 ");
        }
        else
        {
            *res += sVersion + " ";
        }
        std::string sStatus = outMsg->getRecord(0)->getNamedValue("HTTPStatus");
        if(sStatus.length() == 0)
        {
            *res += std::string("200 OK \r\n");
        }
        else
        {
            *res += sStatus + "\r\n";
        }
        std::cout << "---> " << *res;
        std::string sSession = outMsg->getRecord(0)->getNamedValue("HTTPSession");
        if(sSession.length() != 0)
        {
            *res += std::string("set-cookie: session=")+ sSession +std::string(";path=/; \r\n");
        }

        std::string sEncoding = outMsg->getRecord(0)->getNamedValue("HTTPEncoding");
        if(sEncoding.length() != 0)
        {
            *res += std::string("content-encoding: ")+ sEncoding + "\r\n";
        }

        std::string sContentType = outMsg->getRecord(0)->getNamedValue("HTTPContentType");
        if(sContentType.length() != 0)
        {
            *res += std::string("content-type: ")+ sContentType + "\r\n";
        }

        std::string sLocation = outMsg->getRecord(0)->getNamedValue("HTTPLocation");
        if(sLocation.length() != 0)
        {
            *res += std::string("Location: ")+ sLocation + "\r\n";
        }

        std::string sEtag = outMsg->getRecord(0)->getNamedValue("HTTPEtag");
        if(sEtag.length() != 0)
        {
            *res += std::string("ETag: ")+ sEtag + "\r\n";
        }

        const std::string* sData = outMsg->getRecord(1)->getBlock(0);


        std::stringstream strSt;
        strSt << sData->length();

        *res += std::string("content-length: ")+ strSt.str() + std::string("\r\n");

        *res += std::string("\r\n") + *sData;
    }
    return res;
}

std::string HttpEncoder::getMessageID(std::shared_ptr<Msg> msg) const
{
    std::string s = "";
    if (msg->getRecordCount() != 0)
    {
        Record* rec = msg->getRecord(0);
        s = rec->getNamedValue("ID");
    }
    return s;
}


void HttpEncoder::build200Header(std::shared_ptr<Msg> msg,const std::string& contentType)
{
    Record* rcd = new Record;
    msg->addRecord(rcd);
    rcd->addBlock("HTTPVersion=HTTP/1.1");
    rcd->addBlock("HTTPStatus=200");
    rcd->addBlock("HTTPContentType="+contentType);
}



void HttpEncoder::build303Header(std::shared_ptr<Msg> msg, const std::string& URL)
{
    Record* rcd = new Record;
    msg->addRecord(rcd);
    rcd->addBlock("HTTPVersion=HTTP/1.1");
    rcd->addBlock("HTTPStatus=303");
    rcd->addBlock("HTTPLocation="+URL);
}


void HttpEncoder::build404Header(std::shared_ptr<Msg> msg)
{
    Record* rcd = new Record;
    msg->addRecord(rcd);
    rcd->addBlock("HTTPVersion=HTTP/1.1");
    rcd->addBlock("HTTPStatus=404");
}

void HttpEncoder::build304Header(std::shared_ptr<Msg> msg)
{
    Record* rcd = new Record;
    msg->addRecord(rcd);
    rcd->addBlock("HTTPVersion=HTTP/1.1");
    rcd->addBlock("HTTPStatus=304");
}

void HttpEncoder::build500Header(std::shared_ptr<Msg> msg)
{
    Record* rcd = new Record;
    msg->addRecord(rcd);
    rcd->addBlock("HTTPVersion=HTTP/1.1");
    rcd->addBlock("HTTPStatus=500");
}

void HttpEncoder::addContent(std::shared_ptr<Msg> msg,const std::string &content)
{
    if(msg->getRecordCount() != 1)
    {
        return;
    }
    Record* rcd = new Record;
    msg->addRecord(rcd);
    rcd->addBlock(content);
    char sLength[80];
    snprintf(sLength,80,"%d",(int) content.length());
    std::string ctLength = std::string("content-length: ") + sLength;
    msg->getRecord(0)->addBlock(ctLength);
}


