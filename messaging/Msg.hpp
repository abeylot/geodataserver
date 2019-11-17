/*
 * Msg.hpp
 *
 *  Created on: 18 aug 2011
 *      Author: Alain Beylot
 *
 *
 *
 *
 */
#ifndef Msg_hpp
#define Msg_hpp

#define RECORD_SEPARATOR    (char)0x1E
#define BLOCK_SEPARATOR     (char)0x1D
#define RCD_INITIAL_SIZE          0x10
#define RCD_COMP                  0x0F


const char identO_hpp[] = "$Id: Msg.hpp,v 1.18 2013/10/08 08:06:54 aby Exp $";

#include <string>
#include <vector>
#include <stdio.h>
#include <inttypes.h>
#include "../helpers/TcpConnection.hpp"



class Record
{
private:
    std::vector<std::string> Blocks;
    uint64_t size;

public:

    virtual ~Record()
    {
    }

    Record()
    {
        size=0;
    }

    void addBlock(const std::string&);
    void addBlock(const char*, uint64_t length);
    const std::string* getBlock(uint64_t i) const;
    uint64_t getBlockCount() const;

    inline std::vector<std::string> getBlocks()
    {
        return Blocks;
    };

    Record*  duplicate() const;

    std::string getNamedValue(const std::string& key) const;

};

class Msg
{
private:
    std::vector<Record*> Records;
    TcpConnection* socket;

public:

    Msg()
    {
        socket = NULL;
    }

    virtual ~Msg()
    {
        for (uint64_t i=0; i<Records.size(); i++)
        {
            delete Records[i];
        }
        Records.clear();
        //if (socket) delete socket;
    }

    void setConnection(TcpConnection* s)
    {
        socket = s;
    }

    TcpConnection* getConnection() const
    {
        return socket;
    }

    void addRecord(Record*);
    Record* getRecord(uint64_t i) const;
    uint64_t getRecordCount() const;
    Msg*  duplicate() const;

};




#endif
