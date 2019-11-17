#define _FILE_OFFSFET_BITS 64
#ifndef FILEINDEX_HPP
#define FILEINDEX_HPP
//#define FILEINDEX_SORTBUFFERSIZE 10000000LL
#define FILEINDEX_RAWFLUSHSIZE   10000ULL
#define FILEINDEX_CACHELEVEL 26
#include <string>
#include <unordered_map>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <iostream>

// IMPORTANT KEY et ITEM doivent être des types 'triviaux'
// i.e. entier, double, char ou structure et tableaux de ces types.
namespace fidx
{
 /**
  * @brief Record for index file.
  * 
  * @tparam ITEM item to store.
  * @tparam KEY  key of index.
  */
template <class ITEM,class KEY> struct Record
{
    KEY key;
    ITEM value;
};

// fonction de comparaison
// à spécialiser si l'opérateur de comparaison n'est pas disponible
/**
 * @brief 
 * 
 * @tparam ITEM 
 * @tparam KEY 
 * @param a 
 * @param b 
 * @return int comparison result as memcmp. 
 */
template <class ITEM, class KEY> int fileIndexComp(const void* a, const void* b)
{
    Record<ITEM,KEY>* ga = (Record<ITEM,KEY>*) a;
    Record<ITEM,KEY>* gb = (Record<ITEM,KEY>*) b;
    if( ga->key < gb->key ) return -1;
    else if( ga->key > gb->key ) return 1;
    return 0;
}

/**
 * @brief File index descriptor.
 * 
 * @tparam ITEM 
 * @tparam KEY 
 */
template<class ITEM, class KEY> class FileIndex
{
private:
    std::string filename;
    Record<ITEM, KEY> *buffer;
    unsigned long bufferCount;
    uint64_t recSize;
    uint64_t sortedSize;
    std::unordered_map<uint64_t, Record<ITEM, KEY>> cache;
public:
    uint64_t fileSize;
    FILE * pFile;
    /**
     * @brief Construct a new File Index object.
     * 
     * @param filename_  name of the fiile.
     * @param replace    true if file shall be created.
     */
    FileIndex(std::string filename_, bool replace) : filename(filename_)
    {
	std::cout << "file name : " << this->filename << ":" << filename << "\n";
        buffer = NULL;
        if(!replace) {
            pFile = fopen64 (filename.c_str(),"rb");
	    if(pFile == NULL) exit(1);
	}
        else pFile = NULL;
        if(pFile == NULL)
        {
            //buffer = new Record<ITEM,KEY>[2ULL*FILEINDEX_SORTBUFFERSIZE];
            buffer = new Record<ITEM,KEY>[FILEINDEX_RAWFLUSHSIZE];
            pFile = fopen(filename.c_str(),"wb+");
        }
        bufferCount = 0;
        recSize = sizeof(Record<ITEM,KEY>);
        fseeko(pFile, 0, SEEK_END);
        fileSize = ftello(pFile)/recSize;

    }

    /**
     * @brief Destroy the File Index object
     * 
     */
    virtual ~FileIndex()
    {
        fclose(pFile);
        if(buffer != NULL) delete[] buffer;
    }

/**
 * @brief append item to index.
 * 
 * @param key 
 * @param item 
 */
    void append(KEY key, ITEM item)
    {
        Record<ITEM,KEY> record;
        record.key = key;
        record.value = item;
        buffer[bufferCount] = record;
        bufferCount ++;
        if (bufferCount == FILEINDEX_RAWFLUSHSIZE)
        {
            flush();
        }

    }

    /*void appendRaw(KEY key, ITEM item)
    {
        Record<ITEM,KEY> record;
        record.key = key;
        record.value = item;
        buffer[bufferCount] = record;
        bufferCount ++;
        if (bufferCount == FILEINDEX_RAWFLUSHSIZE)
        {
            fwrite(&buffer[0], recSize, bufferCount, pFile);
            bufferCount = 0;
        }

    }*/

/**
 * @brief flush index file to disk.
 * 
 */
    void flush()
    {
        fseeko(pFile, 0, SEEK_END);
        fwrite(&buffer[0], recSize, bufferCount, pFile);
        bufferCount = 0;
        fileSize = ftello(pFile)/recSize;
    }
/**
 * @brief swap items in index
 * 
 * @param item1
 * @param item2 
 */
    void swap(uint64_t item1, uint64_t item2)
    {
        Record<ITEM,KEY> rec1, rec2;
        fseeko(pFile, item1 * recSize, SEEK_SET);
        uint64_t count = fread((char*)&rec1, recSize, 1, pFile);
        if(count == 0)
        {
            std::cerr << "read error \n";
            return;
        }
        fseeko(pFile, item2 * recSize, SEEK_SET);
        count = fread((char*)&rec2, recSize, 1, pFile);
        if(count == 0)
        {
            std::cerr << "read error \n";
            return;
        }
        fseeko(pFile, item1 * recSize, SEEK_SET);
        count = fwrite((char*)&rec2, recSize, 1, pFile);
        if(count == 0)
        {
            std::cerr << "write error \n";
            return;
        }
        fseeko(pFile, item2 * recSize, SEEK_SET);
        count = fwrite((char*)&rec1, recSize, 1, pFile);
        if(count == 0)
        {
            std::cerr << "write error \n";
            return;
        }

    }

    /*void coutFile(FILE *p) {
    	char outb[10000];
    	memset(outb,0,10000);
    	uint64_t fpos = ftell(p);
    	fseek(p,0,SEEK_SET);
    	fread(outb,1,10000,p);
    	fseek(p,fpos,SEEK_SET);
    	std::cerr <<"___\n"<< outb;
    }*/

    /**
     * @brief sort index
     * 
     */

    void sort()
    {
        std::cerr << "sorting  " << fileSize << " elements \n";
        if(!fileSize) return;
        sortedSize = 0;
        uint64_t buffer_size = 1000000000ULL;
        Record<ITEM,KEY>* sort_buffer = NULL;
        while(sort_buffer == NULL)
        {
			buffer_size /= 2;
			try
			{
				sort_buffer = new Record<ITEM,KEY>[buffer_size*3];
			} catch(std::bad_alloc& ba)
			{
				std::cout << "buffer_size : " << buffer_size << "too big\n";
			}
		}
        sort(0,fileSize-1, sort_buffer, buffer_size);
        delete[] sort_buffer;
    }

    /**
     * @brief soert idex part.
     * 
     * @param begin 
     * @param end 
     * @param sort_buffer 
     * @param buffer_size 
     */
    void sort(uint64_t begin, uint64_t end, Record<ITEM,KEY>* sort_buffer, uint64_t buffer_size)
    {
        std::cerr << "sort " << begin <<" "<< end <<"\n";
        uint64_t count = (end - begin) + 1;
        //coutFile(pFile);
        if((end - begin) < (3ULL*buffer_size))
        {
            std::cout << "enough memory, performing qsort \n";
            fseeko(pFile, begin * recSize, SEEK_SET);
            std::cout << "read data \n";
            count = fread((char*)sort_buffer, recSize, count, pFile);
            std::cout << "sort data \n";
            std::qsort(&sort_buffer[0], count,recSize,fileIndexComp<ITEM,KEY>);
            fseeko(pFile, begin * recSize, SEEK_SET);
            std::cout << "write data \n";
            count = fwrite((char*)sort_buffer, recSize, count, pFile);
            sortedSize += count;
            std::cerr << "*** *** *** *** sorted " << sortedSize << " out of " << fileSize << "\n";
            return;
        }
        if((end - begin) > 2)
        {
            uint64_t i = (end + begin) / 2;
            swap(i,end);
        }
        //coutFile(pFile);
        Record<ITEM,KEY>* plusPetits = sort_buffer;
        Record<ITEM,KEY>* plusGrands = sort_buffer + buffer_size;
        Record<ITEM,KEY>* autres = sort_buffer + 2ULL*buffer_size;
        Record<ITEM,KEY> pivot;
        get(end, &pivot);
        uint64_t plusPetitsBufferCount=0;
        uint64_t plusGrandsBufferCount=0;
        uint64_t autresBufferCount=0;
        uint64_t autresCount = end - begin;
        uint64_t plusGrandsCount = 0;
        uint64_t plusPetitsCount = 0;
        while(autresCount > 0)
        {
            uint64_t toRead = buffer_size;
            std::cout << "reading " << toRead << "items from file \n";
            if(toRead > autresCount) toRead = autresCount;
            fseeko(pFile, (end - (toRead  + plusGrandsCount)) * recSize, SEEK_SET);
            uint64_t count = fread((char*)autres, recSize, toRead, pFile);
            if(!count)
            {
                std::cerr << "File error 1!\n";
            }
            autresBufferCount = toRead;
            plusPetitsBufferCount = 0;
            plusGrandsBufferCount = 0;
            autresCount -= toRead;
            std::cout << "dividing items \n";
            while(autresBufferCount > 0)
            {
                int comp = fileIndexComp<ITEM,KEY>(&autres[autresBufferCount -1], &pivot);
                if(comp > 0)
                {
                    plusGrands[plusGrandsBufferCount] = autres[autresBufferCount - 1];
                    plusGrandsBufferCount++;
                }
                else
                {
                    plusPetits[plusPetitsBufferCount] = autres[autresBufferCount - 1];
                    plusPetitsBufferCount++;
                }
                autresBufferCount --;
            }
            std::cout << "divided items \n";

            if(plusGrandsBufferCount)
            {
                std::cout << plusGrandsBufferCount << " items where bigger writing them to file\n";
                fseeko(pFile, (end - (plusGrandsCount + plusGrandsBufferCount - 1)) * recSize, SEEK_SET);
                count = fwrite((char*)plusGrands, recSize, plusGrandsBufferCount, pFile);
                if(!count) std::cerr << "File error 2!\n";
            }
            plusGrandsCount += plusGrandsBufferCount;
            std::cout << plusPetitsBufferCount << " items where smaller writing them to file\n";
            if(autresCount  < plusPetitsBufferCount)
            {
                std::cout << " cache " << autresCount << " items \n";
                fseeko(pFile, (begin + plusPetitsCount) * recSize, SEEK_SET);
                uint64_t count = fread((char*)autres, recSize, autresCount, pFile);
                if(plusPetitsBufferCount)
                {
                    std:: cout << "write smallers \n";
                    fseeko(pFile, (begin + plusPetitsCount) * recSize, SEEK_SET);
                    count = fwrite((char*)plusPetits, recSize, plusPetitsBufferCount, pFile);
                    if(!count) std::cerr << "File error 3!\n";
                    plusPetitsCount += plusPetitsBufferCount;
                }
                if(autresCount)
                {
                    std:: cout << "write cached \n";
                    fseeko(pFile, (begin+plusPetitsCount) * recSize, SEEK_SET);
                    count = fwrite((char*)autres, recSize, autresCount, pFile);
                    if(!count) std::cerr << "File error 4!\n";
                }

            }
            else
            {
                if(plusPetitsBufferCount)
                {

                    std::cout << " cache " << plusPetitsCount << " items \n";
                    fseeko(pFile, (begin + plusPetitsCount) * recSize, SEEK_SET);
                    uint64_t count = fread((char*)autres, recSize, plusPetitsBufferCount, pFile);
                    if(!count) std::cerr << "File error 5!\n";

                    std:: cout << "write smallers \n";
                    fseeko(pFile, (begin + plusPetitsCount) * recSize, SEEK_SET);
                    count = fwrite((char*)plusPetits, recSize, plusPetitsBufferCount, pFile);
                    if(!count) std::cerr << "File error 6!\n";

                    plusPetitsCount += plusPetitsBufferCount;

                    std:: cout << "write cached \n";
                    fseeko(pFile, (end - (plusGrandsCount + plusPetitsBufferCount)) * recSize, SEEK_SET);
                    count = fwrite((char*)autres, recSize, plusPetitsBufferCount, pFile);
                    if(!count) std::cerr << "File error 7!\n";
                }
            }
            //xxx
            fseeko(pFile, (end  - (plusGrandsCount)) * recSize, SEEK_SET);
            std:: cout << "write pivot at the right place \n";
            count = fwrite((char*)&pivot, recSize, 1, pFile);

        }
        if(!count) std::cerr << "File error 8!\n";
        sortedSize++;
        std::cerr << "*** *** *** *** sorted " << sortedSize << " out of " << fileSize << "\n";
        if(plusGrandsCount > 1) sort(end - (plusGrandsCount - 1), end, sort_buffer, buffer_size);
        else if(!plusGrandsCount) sortedSize++;
        if(plusPetitsCount > 1) sort(begin, begin + plusPetitsCount - 1, sort_buffer, buffer_size);
        else if(!plusPetitsCount) sortedSize++;
    }

/**
 * @brief Get the And Cache object
 * 
 * @param pos 
 * @param result 
 * @return true 
 * @return false 
 */
    bool  getAndCache(uint64_t pos, Record<ITEM,KEY>* result)
    {
        auto it = cache.find(pos);
        if(it == cache.end())
        {
            if(get(pos, result))
            {
                cache[pos] = *result;
                return true;
            }
            else return false;
        }
        else
        {
            *result = it->second;
            return true;
        }
    }

/**
 * @brief get object from index.
 * 
 * @param pos 
 * @param result 
 * @return true 
 * @return false 
 */
    inline bool  get(uint64_t pos, Record<ITEM,KEY>* result )
    {
        if( pos >= getSize())
        {
            return false;
        }
        fseeko(pFile, pos*recSize, SEEK_SET);
        uint64_t count = fread(result, recSize, 1, pFile);
        if(count)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

/**
 * @brief get index size.
 * 
 * @return uint64_t 
 */
    uint64_t getSize()
    {
        return fileSize;
    }

/**
 * @brief search key in index.
 * 
 * @param key 
 * @param result 
 * @return true 
 * @return false 
 */
    bool find(KEY key, Record<ITEM,KEY>* result)
    {
        uint64_t iMin = 0;
        uint64_t iMax = getSize() - 1;
        short level = 0;

        getAndCache(iMin, result);
        if( result->key == key)
        {
            return true;
        }
        if( result->key > key)
        {
            return false;
        }
        getAndCache(iMax, result);
        if( result->key == key)
        {
            return true;
        }
        if( result->key < key)
        {
            return false;
        }

        while((iMax - iMin) > 1)
        {
            level++;
            uint64_t iPivot = (iMin + iMax) >> 1;
            if(level < FILEINDEX_CACHELEVEL) getAndCache(iPivot,result);
            else get(iPivot, result);
            if(result->key > key) iMax = iPivot;
            else if(result->key == key)
            {
                return true;
            }
            else iMin = iPivot;
        }
        return false;
    };

    /**
     * @brief find last index id lesser than key. 
     * 
     * @param key 
     * @param iMin 
     * @return true 
     * @return false 
     */

    bool findLastLesser(KEY key, uint64_t& iMin)
    {
        Record<ITEM,KEY> result;
        iMin = 0;
        uint64_t iMax = getSize() - 1;
        //uint64_t iFound = -1;
        //bool found = false;
        short level = 0;
        getAndCache(iMin, &result);
        if( result.key == key)
        {
            return true;
        }
        if( result.key > key)
        {
			iMin = 0;
            return true;
        }

        getAndCache(iMax, &result);
        //if( result.key == key)
        //{
        //    return true;
        //}
        if( result.key < key)
        {
			iMin = iMax;
            return true;
        }

        while((iMax - iMin) > 1)
        {
            level++;
            uint64_t iPivot = (iMin + (iMax -iMin)/2);
            if(level < FILEINDEX_CACHELEVEL) getAndCache(iPivot,&result);
            else get(iPivot, &result);
            if(result.key >= key) iMax = iPivot;
            else iMin = iPivot;
        }
        //get(iMin, &result);
        /*while((iMin > 0) && (result.key >= key))
        {
            iMin--;
            get(iMin, &result);
        }*/
        return true;
    };

};

/**
 * @brief index without key (key is record number).
 * 
 * @tparam ITEM 
 */
template<class ITEM> class FileRawIndex
{
private:
    std::string filename;
    FILE * pFile;
    ITEM *buffer;
    unsigned long bufferCount;
    uint64_t recSize;
    uint64_t fileSize;
public:
    uint64_t itemCount;
    /**
     * @brief Construct a new FileRawIndex object
     * 
     * @param filename 
     * @param replace 
     */
    FileRawIndex(std::string filename, bool replace) : filename(filename)
    {
        buffer = new ITEM[FILEINDEX_RAWFLUSHSIZE];
        if(!replace)
            pFile = fopen64 (filename.c_str(),"rb");
        else pFile = NULL;
        if(pFile == NULL)
        {
            pFile = fopen(filename.c_str(),"wb+");
        }
        bufferCount = 0;
        itemCount = 0;
        recSize = sizeof(ITEM);
        fseeko(pFile, 0, SEEK_END);
        fileSize = ftello(pFile)/recSize;

    }
    /**
     * @brief Destroy the File Raw Index object
     * 
     */
    virtual ~FileRawIndex()
    {
        fclose(pFile);
        delete[] buffer;
    }
/**
 * @brief append record.
 * 
 * @param record 
 */
    void append(ITEM record)
    {
        buffer[bufferCount] = record;
        bufferCount ++;
        itemCount++;
        if (bufferCount == FILEINDEX_RAWFLUSHSIZE)
        {
            flush();
        }

    }
/**
 * @brief flush index file.
 * 
 */
    void flush()
    {
        fwrite(&buffer[0], recSize, bufferCount, pFile);
        bufferCount = 0;
        fileSize = ftello(pFile)/recSize;
    }

    bool  get(uint64_t pos, ITEM* result )
    {
        if( pos >= getSize())
        {
            return false;
        }
        fseeko(pFile, pos*recSize, SEEK_SET);
        uint64_t count = fread(result, recSize, 1, pFile);
        if(count)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    uint64_t getSize()
    {
        return fileSize;
    }
};

/**
 * @brief file index for variable count of records data
 * 
 * @tparam ITEM 
 */
template<class ITEM> class FileRawData
{
public:
    uint64_t startPos;
    FILE* pFile;
    ITEM* buffer;
    uint64_t bufferCount;
    uint64_t itemCount;
    uint64_t recSize;
    uint64_t fileSize;
    uint64_t startCount;
public:
/**
 * @brief Construct a new File Raw Data object
 * 
 * @param filename 
 * @param replace 
 */
    FileRawData(std::string filename, bool replace)
    {
        buffer = new ITEM[FILEINDEX_RAWFLUSHSIZE];
        if(!replace)
            pFile = fopen64 (filename.c_str(),"rb");
        else pFile = NULL;
        if(pFile == NULL)
        {
            pFile = fopen(filename.c_str(),"wb+");
        }
        bufferCount = 0;
        itemCount = 0;
        recSize = sizeof(ITEM);
        fseeko(pFile, 0, SEEK_END);
        fileSize = ftello(pFile)/recSize;
    }
/**
 * @brief Get data.
 * 
 * @param start 
 * @param count 
 * @return ITEM* 
 */
    ITEM* getData(uint64_t start, uint64_t count)
    {
        if(count == 0) return NULL;
        ITEM* result = (ITEM*) malloc(sizeof(ITEM)*count);
        fseeko(pFile, start*recSize, SEEK_SET);
        count = fread(result, recSize, count, pFile);
        return result;
    }
/**
 * @brief start of batch.
 * 
 */
    void startBatch()
    {
        startCount = itemCount;
    }

    /**
     * @brief append item
     * 
     * @param record 
     */

    void append(ITEM record)
    {
        buffer[bufferCount] = record;
        bufferCount ++;
        itemCount++;
        if (bufferCount == FILEINDEX_RAWFLUSHSIZE)
        {
            flush();
        }
    }
/**
 * @brief flush to file.
 * 
 */
    void flush()
    {
        fwrite(&buffer[0], recSize, bufferCount, pFile);
        bufferCount = 0;
        fileSize = ftello(pFile)/recSize;
    }
/**
 * @brief revert buffer.
 * 
 * @param buf 
 * @param size 
 */
    static void revert(ITEM* buf, unsigned int size)
    {
        unsigned int  imax = size / 2;
        ITEM swap;
        for(unsigned int  i = 0 ; i < imax; i++)
        {
            swap = buf[i];
            buf[i] = buf[size - (i+1)];
            buf[size - (i+1)] = swap;
        }
    }

};

/**
 * @brief storage for variable length recors.
 * 
 * @tparam ITEM 
 */
template<class ITEM> class FileRawVarData
{
public:
    uint64_t startPos;
    FILE* pFile;
    char* buffer;
    uint64_t bufferCount;
    uint64_t itemCount;
    uint64_t startCount;
public:
/**
 * @brief Construct a new File Raw Var Data object
 * 
 * @param filename 
 * @param replace 
 */
    FileRawVarData(std::string filename, bool replace)
    {
        buffer = new char[FILEINDEX_RAWFLUSHSIZE];
        if(!replace)
            pFile = fopen64 (filename.c_str(),"rb");
        else pFile = NULL;
        if(pFile == NULL)
        {
            pFile = fopen(filename.c_str(),"wb+");
        }
        bufferCount = 0;
        itemCount = 0;
        fseeko(pFile, 0, SEEK_END);
    }

/**
 * @brief Get the Data object
 * 
 * @param start 
 * @param count 
 * @return char* 
 */
    char* getData(uint64_t start, uint64_t count)
    {
        if(count == 0) return NULL;
        char* result = (char*) malloc(count);
        fseeko(pFile, start, SEEK_SET);
        count = fread(result, 1, count, pFile);
        return result;
    }
/**
 * @brief start batch.
 * 
 */
    void startBatch()
    {
        startCount = itemCount;
    }
/**
 * @brief append item.
 * 
 * @param record 
 */
    void append(ITEM& record)
    {
        unsigned char sz;
        uint64_t size = record.size();
        const char* buff = record.buffer();
        if(size > 255) size = 255;
        sz = size;
        buffer[bufferCount] = ((char)sz);
        bufferCount ++;
        itemCount++;
        memcpy(buffer + bufferCount, buff, sz);
        bufferCount += sz;
        itemCount += sz;
        if (bufferCount >= (FILEINDEX_RAWFLUSHSIZE - 256))
        {
            flush();
        }
    }
/**
 * @brief flush file
 * 
 */
    void flush()
    {
        fwrite(buffer, 1, bufferCount, pFile);
        bufferCount = 0;
    }


};



}
#endif
