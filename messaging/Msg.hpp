/*
 * Msg.hpp
 *
 */
#ifndef Msg_hpp
#define Msg_hpp


#include <string>
#include <vector>
#include <stdio.h>
#include <inttypes.h>
#include <memory>
#include "../helpers/TcpConnection.hpp"



class Record
{
private:
    static constexpr int RCD_INITIAL_SIZE = 0x10;
    static constexpr int RCD_COMP = 0x0F;
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
        socket = nullptr;
    }

    virtual ~Msg()
    {
        for (uint64_t i=0; i<Records.size(); i++)
        {
            delete Records[i];
        }
        Records.clear();
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
    std::shared_ptr<Msg>  duplicate() const;

};




#endif
