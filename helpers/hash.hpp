#ifndef HASH_HPP
#define HASH_HPP
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
namespace hh
{


class THashIntegerTable
{
public :
    unsigned int hashMe(uint64_t s);
    bool addIfUnique(uint64_t key);
    bool removeIfExists(uint64_t key);
    bool get(uint64_t key);
    THashIntegerTable(int aCellsCount);
    virtual ~THashIntegerTable();
    uint64_t** cells;
    uint64_t* cellsCounter;
    int cellsCount;
    uint64_t itemCount, collisions;
};

}
#endif
