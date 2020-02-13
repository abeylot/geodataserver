/*
 * Msg.cpp
 */



#include "Msg.hpp"



#include <string>
#include <vector>
#include <stdio.h>
#include "../helpers/TcpConnection.hpp"



void Record::addBlock(const char* c, uint64_t length)
{
    if((size & RCD_COMP) == 0)
    {
        Blocks.reserve(((size/RCD_INITIAL_SIZE)+1)*RCD_INITIAL_SIZE);
    }
    Blocks.push_back(std::string(c,length));
    size++;

}


void Record::addBlock( const std::string& str)
{
    if((size & RCD_COMP) == 0)
    {
        Blocks.reserve(((size/RCD_INITIAL_SIZE)+1)*RCD_INITIAL_SIZE);
    }
    Blocks.push_back(str);
    size++;
}

const std::string* Record::getBlock(uint64_t i) const
{
    if (i < Blocks.size()) return &Blocks[i];
    return NULL;
}

std::string Record::getNamedValue(const std::string& key) const
{
    std::string result;
    uint64_t len = key.size() + 1;
    std::string test = key +  "=";
    for (uint64_t i=0; i<size; i++)
    {
        if((Blocks[i].size() >= len) && (test == Blocks[i].substr(0,len)))
        {
            return std::string(Blocks[i].c_str() + len);
        }

    }
    return "";
}


uint64_t Record::getBlockCount() const
{
    return size;
}

Record*  Record::duplicate() const
{

    Record* res = new Record();
    for(unsigned int i = 0 ; i < size; i++)
        res->addBlock(Blocks[i]);
    return res;
}



Record* Msg::getRecord(uint64_t i) const
{
    if (i < Records.size()) return Records[i];
    return NULL;
}

uint64_t Msg::getRecordCount() const
{
    return Records.size();
}

void Msg::addRecord(Record* r)
{
    long long sz = Records.size();
    if(sz%100 == 0)
    {
        Records.reserve(((sz/100)+1)*100);
    }
    Records.push_back(r);
}



Msg*  Msg::duplicate() const
{
    Msg* res = new Msg;
    int max_i = getRecordCount();
    for(int i = 0 ; i < max_i; i++)
    {
        Record* rcd = getRecord(i)->duplicate();
        res->addRecord(rcd);
    }
    return res;
}


