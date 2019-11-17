#include "helpers/FileIndex.hpp"


using namespace fidx;
struct key
{
    char val[1];
};

bool operator<(key const& a, key const& b)
{
    return (strncmp(a.val, b.val, 1) <0);
}

bool operator>(key const& a, key const& b)
{
    return (strncmp(a.val, b.val, 1) >0);
}

struct value
{
    char value[1];
};

FileIndex<value,key>* testIndex;

int main(int argc, char *argv[])
{
    testIndex = new FileIndex<value,key>("testIndex", true);
    fwrite("A,",2,1,testIndex->pFile);
    fwrite("B,",2,1,testIndex->pFile);
    fwrite("C,",2,1,testIndex->pFile);
    fwrite("D,",2,1,testIndex->pFile);
    fwrite("E,",2,1,testIndex->pFile);
    fwrite("F,",2,1,testIndex->pFile);
    fwrite("G,",2,1,testIndex->pFile);
    fwrite("H,",2,1,testIndex->pFile);
    fwrite("I,",2,1,testIndex->pFile);
    fwrite("J,",2,1,testIndex->pFile);
    fwrite("K,",2,1,testIndex->pFile);
    testIndex->fileSize = 11;
    testIndex->sort();
}
