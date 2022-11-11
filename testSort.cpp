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
    /*fwrite("A,",2,1,testIndex->pFile.fh);
    fwrite("B,",2,1,testIndex->pFile.fh);
    fwrite("C,",2,1,testIndex->pFile.fh);
    fwrite("D,",2,1,testIndex->pFile.fh);
    fwrite("E,",2,1,testIndex->pFile.fh);
    fwrite("F,",2,1,testIndex->pFile.fh);
    fwrite("G,",2,1,testIndex->pFile.fh);
    fwrite("H,",2,1,testIndex->pFile.fh);
    fwrite("I,",2,1,testIndex->pFile.fh);
    fwrite("J,",2,1,testIndex->pFile.fh);
    fwrite("K,",2,1,testIndex->pFile.fh);
    testIndex->fileSize = 11;*/
    testIndex->append({'F'},{' '});
    testIndex->append({'A'},{' '});
    testIndex->append({'H'},{' '});
    testIndex->append({'Z'},{' '});
    testIndex->append({'B'},{' '});
    testIndex->append({'2'},{' '});
    testIndex->append({'Y'},{' '});
    testIndex->append({'P'},{' '});
    testIndex->append({'V'},{' '});
    testIndex->append({'A'},{' '});
    testIndex->flush();
    testIndex->sort();
}
