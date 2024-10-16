#include "hash.hpp"
namespace hh
{

inline unsigned int THashIntegerTable::hashMe(uint64_t s)
{
    return (unsigned int)s;
}

bool THashIntegerTable::addIfUnique(uint64_t key)
{
    int hash;
    bool found;
    //unsigned int i;

    found = false;
    hash = hashMe(key) % cellsCount;
    if (cells[hash] != nullptr)
    {
        uint64_t *current, *last;
        current = cells[hash];
        last = current + cellsCounter[hash];
        for(; current < last; current++)
        {
            if(*current == key)
            {
                found = true;
                break;
            }
        }
    }
    if (!found)
    {
        if(cells[hash] == nullptr) cells[hash] = (uint64_t*) (malloc(sizeof(uint64_t)));
        else cells[hash] = (uint64_t*) (realloc(cells[hash],sizeof(uint64_t)*(cellsCounter[hash] +1)));
        cellsCounter[hash]++;
        cells[hash][cellsCounter[hash] - 1] = key;
        itemCount++;
        return true;
    }
    return false;
}

bool THashIntegerTable::removeIfExists(uint64_t key)
{
    int hash;
    hash = hashMe(key) % cellsCount;
    if (cells[hash] != nullptr)
    {
        bool found;
        found = false;
        unsigned int i;
        for(i=0; i < cellsCounter[hash]; i++)
        {
            if(cells[hash][i] == key)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            for(unsigned int j=i; j < cellsCounter[hash]; j++)
            {
                cells[hash][j] = cells[hash][j+1];
            }
            if(cellsCounter[hash] > 1)
            {
                cells[hash] = (uint64_t*) realloc(cells[hash],sizeof(uint64_t)*(cellsCounter[hash] -1));
            }
            else
            {
                free(cells[hash]);
                cells[hash] = nullptr;
            }
            cellsCounter[hash]--;
            itemCount --;
            return true;
        }
    }
    return false;
}

bool THashIntegerTable::get(uint64_t key)
{
    int hash;
    hash = hashMe(key) % cellsCount;
    if (cells[hash] != nullptr)
    {
        for(unsigned int i=0; i < cellsCounter[hash]; i++)
        {
            if(cells[hash][i] == key)
            {
                return true;
            }
        }
    }
    return false;
}

THashIntegerTable::THashIntegerTable(int aCellsCount)
{
    cellsCount = aCellsCount;
    cells = (uint64_t**) malloc(cellsCount * sizeof(uint64_t*));
    memset(cells,0,cellsCount * sizeof(uint64_t*));
    cellsCounter = (uint64_t*) malloc(cellsCount * sizeof(uint64_t));
    memset (cellsCounter,0,cellsCount * sizeof(uint64_t));
    itemCount = 0;
    collisions = 0;
}

THashIntegerTable::~THashIntegerTable()
{
        uint64_t ** current  = cells;
        uint64_t ** last     = cells + cellsCount;

    for (;current < last; current++)
    {
        if(*current) free(*current);
    }
    free(cells);
    free(cellsCounter);

}
}
