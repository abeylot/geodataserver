#define _FILE_OFFSFET_BITS 64
#ifndef FILEINDEX_HPP
#define FILEINDEX_HPP
#define FILEINDEX_RAWFLUSHSIZE   1000ULL
#define FILEINDEX_CACHELEVEL 24
#include <string>
#include <unordered_map>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <iostream>

#ifdef __linux__
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <map>
#include <assert.h>
#include <thread>
#include <mutex>
#include <sys/sysinfo.h>

struct GeoFile
{
    int fh = -1;
    std::string name;
    void open(const std::string& fname, bool create)
    {
        name = fname;
        if(create)
        {
            fh = open64(name.c_str(),O_CREAT|O_TRUNC|O_RDWR| O_NOATIME, S_IWUSR | S_IREAD );
        } else {
            fh = open64(name.c_str(),O_RDONLY | O_NOATIME);
            if(!fh)
            {
                std::cout << name << " not found !"<< "\n";
                exit(1);
            }
            posix_fadvise(fh, 0, 0, POSIX_FADV_RANDOM | POSIX_FADV_NOREUSE);
        }
        if (fh < 0)
        {
            throw std::runtime_error("Unable to open file : " + name);
        }
    }

    virtual ~GeoFile(){if(fh >=0) close(fh);}

    void owrite(char* buffer, uint64_t offset, uint64_t length)
    {
        uint64_t got = 0;
        while(length)
        {
            int64_t count = pwrite64(fh, &buffer[got], length, offset+got);
            if(count <= 0)
            {
                throw std::runtime_error("Unable to write file : " + name);
            } else {
                length -= count;
                got += count;
            }
        }
    }

    void oread(char* buffer, uint64_t offset, uint64_t length)
    {
        uint64_t got = 0;
        while(length)
        {
            int64_t count = pread64(fh, &buffer[got], length, offset+got);
            if(count <= 0)
            {
                throw std::runtime_error("Unable to read file : " + name);
            } else {
                length -= count;
                got += count;
            }
        }
    }

    void append(char* buffer, uint64_t length)
    {
        int64_t len = lseek64(fh, 0, SEEK_END);
        if( len < 0 )
        {
            throw std::runtime_error("Unable to seek file : " + name);
        }

        uint64_t got = 0;
        while(length)
        {
            int64_t count = write(fh, &buffer[got], length);
            if(count <= 0)
            {
                 throw std::runtime_error("Unable to write file : " + name);
            } else {
                length -= count;
                got += count;
            }
        }
    }

    uint64_t length()
    {
        int64_t len = lseek64(fh, 0, SEEK_END);
        if( len < 0 )
        {
            throw std::runtime_error("Unable to seek file : " + name);
        }

        return len;
    }
};
#else
struct GeoFile
{
    FILE* fh = -1;
    std::string name;
    void open(const std::string& fname, bool create)
    {
        name = fname;
        if(create)
        {
            fh = fopen64(name.c_str(),"wb+");
        } else {
            fh = fopen64 (name.c_str(),"rb");

        }
        if (fh == nullptr)
        {
            throw std::runtime_error("Unable to open file : " + name);
        }
    }

    virtual ~GeoFile(){if(fh !=0) close(fh);}

    void owrite(char* buffer, uint64_t offset, uint64_t length)
    {
        uint64_t got = 0;
        if(fseeko(fh, offset, SEEK_SET))
        {
            throw std::runtime_error("Unable to write file : " + name);
        }
        while(length)
        {
            int64_t count = fwrite(buffer + got, 1, length, fh);
            if(count <= 0)
            {
                throw std::runtime_error("Unable to write file : " + name);
            } else {
                length -= count;
                got += count;
            }
        }
    }

    void oread(char* buffer, uint64_t offset, uint64_t length)
    {
        uint64_t got = 0;
        if(fseeko(fh, offset, SEEK_SET))
        {
            throw std::runtime_error("Unable to seek file : " + name);
        }
        while(length)
        {
            int64_t count = fread(buffer + got, 1, length, fh);
            if(count <= 0)
            {
                   throw std::runtime_error("Unable to read file : " + name);
            } else {
                length -= count;
                got += count;
            }
        }
    }

    void append(char* buffer, uint64_t length)
    {
        uint64_t got = 0;
        if(fseeko(fh, 0, SEEK_END))
        {
            throw std::runtime_error("Unable to seek file : " + name);
        }
        while(length)
        {
            int64_t count = fwrite(buffer + got, 1, length, fh);
            if(count <= 0)
            {
                throw std::runtime_error("Unable to write file : " + name);
            } else {
                length -= count;
                got += count;
            }
        }
    }

    uint64_t length()
    {
        if(fseeko(fh, 0, SEEK_END))
        {
            throw std::runtime_error("Unable to seek file : " + name);
        }

        return ftello(fh);
    }
};
#endif

// IMPORTANT KEY et ITEM doivent être des types 'triviaux'
// i.e. entier, double, char ou structure et tableaux de ces types.
namespace fidx
{



   inline uint64_t makeLexicalKey(const char* value_, size_t value_size_, const std::map<std::string, std::string>&substitutions)
   {
        char value[128];
        size_t value_size = 0;
        for(unsigned int i = 0; i < 128 && i < value_size_;)
        {
            size_t utf_8_len = 1;
            if((value_[i] & 0b11110000) ==  0b11110000)     utf_8_len = 4;
            else if ((value_[i] & 0b11110000) ==  0b11100000) utf_8_len = 3;
            else if ((value_[i] & 0b11110000) ==  0b11000000) utf_8_len = 2;
            std::string in = "";
            for(unsigned int j = i; (j < (utf_8_len + i)) && j < value_size_ ; j++)
            {
               in += value_[j];
            }
            auto it = substitutions.find(in);
            std::string out = in;
            i += in.length();
            if(it != substitutions.end())
            {
                 out = it->second;
            }
            for(size_t k = 0; k < out.length();k++)    value[value_size++] = tolower(out[k]);
        }
        uint64_t key = 0;
        /*if(value_size > 0) { key += tolower(value[0]);}
        key <<= 3;
        if(value_size > 1) { key += tolower(value[1]);}
        key <<= 3;
        if(value_size > 2) { key += tolower(value[2]);}
        key <<= 3;
        if(value_size > 3) { key += tolower(value[3]);}
        key <<= 3;
        if(value_size > 4) { key += tolower(value[4]);}
        key <<= 3;
        if(value_size > 5) { key += tolower(value[5]);}
        key <<= 3;
        if(value_size > 6) { key += tolower(value[6]);}
        key <<= 3;
        if(value_size > 7) { key += tolower(value[7]);}*/
        char* key_c = (char*) &key;
        for(unsigned int i = 0; i < value_size; i++)
        {
            key_c[i%8] = key_c[i%8] ^ value[i];
        }
        return key;
   }


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
} ;

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
    if( ga->key > gb->key ) return -1;
    else if( ga->key < gb->key ) return 1;
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
    bool sorted;
    KEY last_inserted{};
    std::string filename;
    ITEM* itemBuffer;
    KEY*  keyBuffer;
    unsigned long bufferCount;
    uint64_t itemSize;
    uint64_t keySize;
    uint64_t sortedSize;
    //std::unordered_map<uint64_t, Record<ITEM, KEY>> cache;
    std::unordered_map<uint64_t, KEY> cacheKey;
    std::mutex cache_mutex;
    FileIndex(const FileIndex&);
    FileIndex& operator = (const FileIndex&);
public:
    uint64_t fileSize;
    GeoFile itemFile;
    GeoFile keyFile;

    bool isSorted(){return sorted;}

    /**
     * @brief Construct a new File Index object.
     *
     * @param filename_  name of the fiile.
     * @param replace    true if file shall be created.
     */
    FileIndex(const std::string& filename_, bool replace) : filename(filename_)
    {

        itemFile.open(filename, replace);
        keyFile.open(filename + "_key", replace);

        itemBuffer = nullptr;
        keyBuffer  = nullptr;

        if (replace)
        {
            itemBuffer = new ITEM[FILEINDEX_RAWFLUSHSIZE];
            keyBuffer  = new KEY[FILEINDEX_RAWFLUSHSIZE];
        }
        bufferCount = 0;
        itemSize = sizeof(ITEM);
        keySize  = sizeof(KEY);
        fileSize = itemFile.length()/itemSize;
        sortedSize = 0;
        sorted = true;
    }

    /**
     * @brief Destroy the File Index object
     *
     */
    virtual ~FileIndex()
    {
        if(keyBuffer != nullptr) delete[] keyBuffer;
        if(itemBuffer != nullptr) delete[] itemBuffer;
    }

/**
 * @brief append item to index.
 *
 * @param key
 * @param item
 */
    void append(const KEY& key, const ITEM& item)
    {
        //Record<ITEM,KEY> record;
        //record.key = key;
        //record.value = item;

        itemBuffer[bufferCount] = item;
        keyBuffer[bufferCount] = key;
        bufferCount ++;

        if (bufferCount == FILEINDEX_RAWFLUSHSIZE)
        {
            flush();
        }
        if((fileSize) && (key < last_inserted)) sorted = false;
        last_inserted = key;
    }


/**
 * @brief flush index file to disk.
 *
 */
    void flush()
    {
        itemFile.append((char*)&itemBuffer[0], itemSize * bufferCount);
        keyFile.append((char*)&keyBuffer[0], keySize * bufferCount);
        bufferCount = 0;
        fileSize = itemFile.length()/itemSize;
    }
/**
 * @brief swap items in index
 *
 * @param item1
 * @param item2
 */
    void swap(uint64_t n1, uint64_t n2)
    {
        ITEM  item1, item2;
        KEY   key1, key2;

        itemFile.oread((char*)&item1, n1 * itemSize, itemSize);
        itemFile.oread((char*)&item2, n2 * itemSize, itemSize);
        itemFile.owrite((char*)&item2, n1 * itemSize, itemSize);
        itemFile.owrite((char*)&item1, n2 * itemSize, itemSize);

        keyFile.oread((char*)&key1, n1 * keySize, keySize);
        keyFile.oread((char*)&key2, n2 * keySize, keySize);
        keyFile.owrite((char*)&key2, n1 * keySize, keySize);
        keyFile.owrite((char*)&key1, n2 * keySize, keySize);

    }

    /**
     * @brief sort index
     *
     */

    void sort()
    {
        struct sysinfo info;
        int res = sysinfo(&info);
        uint64_t max_mem = 500000000ULL; // 500 mbytes
        if(res == 0)
        {
            max_mem = (((unsigned long) info.freeram) * (( unsigned long) info.mem_unit)) / 2;
        }

        std::cerr << "ram :" << (((unsigned long) info.freeram) * (( unsigned long) info.mem_unit)) / (1048 * 1048) << "mb, sorting  " << fileSize << " elements \n";
        if(!fileSize) return;
        sortedSize = 0;

        uint64_t buffer_size = (fileSize + 3) / 3;

        uint64_t max_bsize = max_mem / ((sizeof(ITEM) + sizeof(KEY))*3);
        if(buffer_size > max_bsize) buffer_size = max_bsize;

        ITEM* item_sort_buffer = nullptr;
        KEY*  key_sort_buffer = nullptr;

        bool resized = false;
        while(item_sort_buffer == nullptr || key_sort_buffer == nullptr)
        {
            try
            {
              item_sort_buffer = new ITEM[buffer_size*3];
              key_sort_buffer  = new KEY[buffer_size*3];
            } catch(std::bad_alloc& ba)
            {
                std::cout << "buffer_size : " << buffer_size << "too big\n";
                buffer_size /= 2;
                resized = true;
            }
            if(resized == true)
            {
                delete[] item_sort_buffer;
                delete[] key_sort_buffer;
                buffer_size /= 2;
                item_sort_buffer = new ITEM[buffer_size*3];
                key_sort_buffer  = new KEY[buffer_size*3];
            }
        }
        sort(0,fileSize-1, key_sort_buffer, item_sort_buffer, buffer_size);
        sorted = true;
        delete[] key_sort_buffer;
        delete[] item_sort_buffer;
    }

    void swapItem(ITEM* a, ITEM* b)
    {
        ITEM c;
        memcpy(&c, a, itemSize);
        memcpy(a, b, itemSize);
        memcpy(b, &c, itemSize);
    }

    void swapKey(KEY* a, KEY* b)
    {
        KEY c;
        memcpy(&c, a, keySize);
        memcpy(a, b, keySize);
        memcpy(b, &c, keySize);
    }

    void memsort(int64_t begin, int64_t end, KEY* key_sort_buffer, ITEM* item_sort_buffer)
    {
        //std::cout << "memsort " << begin <<" "<< end <<"\n";
        if((end - begin) < 1) return;
        if((end - begin) == 1)
        {
            if ( key_sort_buffer[end] < key_sort_buffer[begin] )
            {
                swapKey(key_sort_buffer + end, key_sort_buffer + begin);
                swapItem(item_sort_buffer + end, item_sort_buffer + begin);
            }
            return;
        }
        else
        {
            int64_t lessers = 0;
            int64_t biggers = 0;
            int64_t total = end - begin;
            int64_t ipivot = begin;
            while(total > (biggers + lessers))
            {
                while((total > (biggers + lessers))
                && (begin + 1 + lessers <= end)
                && (key_sort_buffer[begin + 1 + lessers] < key_sort_buffer[begin])) lessers++;
                while((total > (biggers + lessers))
                && (end - biggers >= begin + 1)
                && (key_sort_buffer[end - biggers ] > key_sort_buffer[begin])) biggers++;
                if(total > (biggers + lessers + 1))
                {
                    swapKey(key_sort_buffer + (begin+1+lessers), key_sort_buffer + (end - biggers));
                    swapItem(item_sort_buffer + (begin+1+lessers), item_sort_buffer + (end - biggers));

                    biggers++;
                    lessers++;
                } else if (total == (biggers + lessers + 1)) {
                    if(key_sort_buffer[begin + 1 + lessers] < key_sort_buffer[begin]) lessers++;
                    else biggers++;
                }
            }
            ipivot = begin + lessers;
            swapKey(key_sort_buffer + ipivot, key_sort_buffer + begin);
            swapItem(item_sort_buffer + ipivot, item_sort_buffer + begin);

            if((ipivot + 1)< end) memsort(ipivot + 1, end, key_sort_buffer, item_sort_buffer);
            if(ipivot > (begin + 1)) memsort(begin, ipivot - 1, key_sort_buffer, item_sort_buffer);
        }
    }


    /**
     * @brief sort index part.
     *
     * @param begin
     * @param end
     * @param sort_buffer
     * @param buffer_size
     */

    void sort(uint64_t begin, uint64_t end, KEY* key_sort_buffer, ITEM* item_sort_buffer, uint64_t buffer_size)
    {
        std::cerr << "sort " << begin <<" "<< end <<"\n";
        uint64_t count = (end - begin) + 1;
        if((end - begin + 1) < (3ULL*buffer_size))
        {
            std::cout << "enough memory, performing qsort \n";
            std::cout << "read data \n";
            itemFile.oread((char*)item_sort_buffer, begin * itemSize, itemSize * count);
            keyFile.oread((char*)key_sort_buffer, begin * keySize, keySize * count);
            std::cout << "sort data \n";

            //std::qsort(&sort_buffer[0], count, recSize,fileIndexComp<ITEM,KEY>);
            memsort(0, count - 1, key_sort_buffer, item_sort_buffer);

            std::cout << "write data \n";
            keyFile.owrite((char*)key_sort_buffer, begin * keySize, keySize * count );
            itemFile.owrite((char*)item_sort_buffer, begin * itemSize, itemSize * count );
            sortedSize += count;
            std::cerr << "*** *** *** *** sorted " << sortedSize << " out of " << fileSize << "\n";
            return;
        }
        if((end - begin) > 2)
        {
            uint64_t i = (end + begin) / 2;
            swap(i,end);
        }

        ITEM* item_plusPetits = item_sort_buffer;
        ITEM* item_plusGrands = item_sort_buffer + buffer_size;
        ITEM* item_autres = item_sort_buffer + 2ULL*buffer_size;

        KEY* key_plusPetits = key_sort_buffer;
        KEY* key_plusGrands = key_sort_buffer + buffer_size;
        KEY* key_autres = key_sort_buffer + 2ULL*buffer_size;

        KEY curKey;
        ITEM curItem;


        //Record<ITEM,KEY> tmp;
        getKey(end, &curKey);
        getItem(end, &curItem);
        uint64_t autresCount = end - begin;
        uint64_t plusGrandsCount = 0;
        uint64_t plusPetitsCount = 0;
        while(autresCount > 0)
        {
            uint64_t plusPetitsBufferCount=0;
            uint64_t plusGrandsBufferCount=0;
            uint64_t autresBufferCount=0;
            uint64_t toRead = buffer_size;
            //std::cout << "reading " << toRead << "items from file \n";
            if(toRead > autresCount) toRead = autresCount;

            itemFile.oread((char*)item_autres, (end - (toRead  + plusGrandsCount)) * itemSize, itemSize * toRead );
            keyFile.oread((char*)key_autres, (end - (toRead  + plusGrandsCount)) * keySize, keySize * toRead );

            autresBufferCount = toRead;
            plusPetitsBufferCount = 0;
            plusGrandsBufferCount = 0;
            autresCount -= toRead;
            //std::cout << "dividing items \n";
            while(autresBufferCount > 0)
            {
                autresBufferCount --;
                if(key_autres[autresBufferCount] >  curKey)
                {
                    key_plusGrands[plusGrandsBufferCount] = key_autres[autresBufferCount];
                    item_plusGrands[plusGrandsBufferCount] = item_autres[autresBufferCount];
                    plusGrandsBufferCount++;
                }
                else if(key_autres[autresBufferCount] <  curKey)
                {
                    key_plusPetits[plusPetitsBufferCount] = key_autres[autresBufferCount];
                    item_plusPetits[plusPetitsBufferCount] = item_autres[autresBufferCount];
                    plusPetitsBufferCount++;
                }
                else
                {
                    if(plusGrandsBufferCount> plusPetitsBufferCount)
                    {
                        key_plusPetits[plusPetitsBufferCount] = key_autres[autresBufferCount];
                        item_plusPetits[plusPetitsBufferCount] = item_autres[autresBufferCount];
                        plusPetitsBufferCount++;
                    }
                    else
                    {
                        key_plusGrands[plusGrandsBufferCount] = key_autres[autresBufferCount];
                        item_plusGrands[plusGrandsBufferCount] = item_autres[autresBufferCount];
                        plusGrandsBufferCount ++;
                    }
                }
            }
            //std::cout << "divided items \n";

            if(plusGrandsBufferCount)
            {
                //std::cout << plusGrandsBufferCount << " items where bigger writing them to file\n";
                itemFile.owrite((char*)item_plusGrands, (end - (plusGrandsCount + plusGrandsBufferCount - 1)) * itemSize, itemSize * plusGrandsBufferCount);
                keyFile.owrite((char*)key_plusGrands, (end - (plusGrandsCount + plusGrandsBufferCount - 1)) * keySize, keySize * plusGrandsBufferCount);
            }
            plusGrandsCount += plusGrandsBufferCount;
            //std::cout << plusPetitsBufferCount << " items where smaller writing them to file\n";
            if(autresCount  < plusPetitsBufferCount)
            {
                std::cout << " cache " << autresCount << " items \n";

                itemFile.oread((char*)item_autres, (begin + plusPetitsCount) * itemSize, itemSize * autresCount);
                keyFile.oread((char*)key_autres, (begin + plusPetitsCount) * keySize, keySize * autresCount);

                if(plusPetitsBufferCount)
                {
                    //std:: cout << "write smallers \n";
                    itemFile.owrite((char*)item_plusPetits, (begin + plusPetitsCount) * itemSize, itemSize * plusPetitsBufferCount);
                    keyFile.owrite((char*)key_plusPetits, (begin + plusPetitsCount) * keySize, keySize * plusPetitsBufferCount);
                    plusPetitsCount += plusPetitsBufferCount;
                }
                if(autresCount)
                {
                    //std:: cout << "write cached \n";
                    itemFile.owrite((char*)item_autres, (begin+plusPetitsCount) * itemSize, itemSize * autresCount);
                    keyFile.owrite((char*)key_autres, (begin+plusPetitsCount) * keySize, keySize * autresCount);
                }

            }
            else
            {
                if(plusPetitsBufferCount)
                {

                    //std::cout << " cache " << plusPetitsCount << " items \n";
                    itemFile.oread((char*)item_autres, (begin + plusPetitsCount) * itemSize, itemSize * plusPetitsBufferCount);
                    keyFile.oread((char*)key_autres, (begin + plusPetitsCount) * keySize, keySize * plusPetitsBufferCount);
                    //std:: cout << "write smallers \n";
                    itemFile.owrite((char*)item_plusPetits, (begin + plusPetitsCount) * itemSize, itemSize * plusPetitsBufferCount);
                    keyFile.owrite((char*)key_plusPetits, (begin + plusPetitsCount) * keySize, keySize * plusPetitsBufferCount);

                    plusPetitsCount += plusPetitsBufferCount;
                    //std:: cout << "write cached \n";
                    itemFile.owrite((char*)item_autres, (end - (plusGrandsCount + plusPetitsBufferCount)) * itemSize, itemSize * plusPetitsBufferCount);
                    keyFile.owrite((char*)key_autres, (end - (plusGrandsCount + plusPetitsBufferCount)) * keySize, keySize * plusPetitsBufferCount);
                }
            }
            //std:: cout << "write pivot at the right place \n";
            itemFile.owrite((char*)&(curItem), (end  - (plusGrandsCount)) * itemSize, itemSize);
            keyFile.owrite((char*)&(curKey), (end  - (plusGrandsCount)) * keySize, keySize);

        }
        sortedSize++;
        std::cerr << "*** *** *** *** sorted " << sortedSize << " out of " << fileSize << "\n";
        if(plusGrandsCount > 1) sort(end - (plusGrandsCount - 1), end, key_sort_buffer, item_sort_buffer, buffer_size);
        else if(!plusGrandsCount) sortedSize++;
        if(plusPetitsCount > 1) sort(begin, begin + plusPetitsCount - 1, key_sort_buffer, item_sort_buffer, buffer_size);
        else if(!plusPetitsCount) sortedSize++;
    }

/**
 * @brief Get And Cache object
 *
 * @param pos
 * @param result
 * @return true
 * @return false
 */

    bool  getAndCacheKey(const uint64_t pos, KEY* result)
    {
#ifndef DISCARD_MUTEX
        std::lock_guard<std::mutex> guard(cache_mutex);
#endif
        auto it = cacheKey.find(pos);
        if(it == cacheKey.end())
        {
            if(getKey(pos, result))
            {
                cacheKey[pos] = *result;
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

    inline bool  getItem(uint64_t pos, ITEM* result )
    {
        if( pos >= getSize())
        {
            //std::cerr << "object not found ! " << (int) pos << (int) getSize() << "/n";
            return false;
        }
        itemFile.oread((char*)result, pos*itemSize, itemSize);
        return true;
    }

    inline bool  getKey(const uint64_t pos, KEY* result )
    {
        if( pos >= getSize())
        {
            //std::cerr << "key not found ! " << (int) pos << (int) getSize() << "/n";
            return false;
        }
        keyFile.oread((char*)result, pos*keySize, keySize);
        return true;
    }



    inline bool  get(uint64_t pos, Record<ITEM,KEY>* result )
    {
        if( pos >= getSize())
        {
            return false;
        }
        itemFile.oread((char*)(&(result->value)), pos*itemSize, itemSize);
        keyFile.oread((char*)(&(result->key)), pos*keySize, keySize);
        return true;
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
    bool find(KEY key, ITEM* result, std::unordered_map<uint64_t, KEY>& local_cache)
    {
        uint64_t iMin = 0;
        uint64_t iMax = getSize() - 1;
        short level = 0;
        KEY myKey;

        getAndCacheKey(iMin, &myKey);
        if( myKey == key)
        {
            getItem(iMin, result);
            return true;
        }
        if( myKey > key)
        {
            return false;
        }
        getAndCacheKey(iMax, &myKey);
        if( myKey == key)
        {
            getItem(iMax, result);
            return true;
        }
        if( myKey < key)
        {
            return false;
        }


        while((iMax - iMin) > 1)
        {
            level++;
            uint64_t iPivot = (iMin + iMax) >> 1;
            if(level < FILEINDEX_CACHELEVEL) getAndCacheKey(iPivot,&myKey);
            else
            {
                auto it = local_cache.find(iPivot);
                if(it == local_cache.end())
                {
                    getKey(iPivot, &myKey);
                    local_cache[iPivot] = myKey;
                } else {
                    myKey = it->second;
                }
            }
            if(myKey > key)
            {
                iMax = iPivot;
            }
            else if(myKey == key)
            {
                getItem(iPivot, result);
                return true;
            }
            else
            {
                iMin = iPivot;
            }
        }
        return false;
    };


/**
 * @brief search key in index.
 *
 * @param key
 * @param result
 * @return true
 * @return false
 */
    bool find(KEY key, ITEM* result)
    {
        uint64_t iMin = 0;
        uint64_t iMax = getSize() - 1;
        short level = 0;
        KEY myKey = key;

        getAndCacheKey(iMin, &myKey);
        if( myKey == key)
        {
            getItem(iMin, result);
            return true;
        }
        if( myKey > key)
        {
            return false;
        }
        getAndCacheKey(iMax, &myKey);
        if( myKey == key)
        {
            getItem(iMax, result);
            return true;
        }
        if( myKey < key)
        {
            return false;
        }


        while((iMax - iMin) > 1)
        {
            level++;
            uint64_t iPivot = (iMin + iMax) >> 1;
            if(level < FILEINDEX_CACHELEVEL) getAndCacheKey(iPivot,&myKey);
            else getKey(iPivot, &myKey);
            if(myKey > key)
            {
                iMax = iPivot;
            }
            else if(myKey == key)
            {
                getItem(iPivot, result);
                return true;
            }
            else
            {
                iMin = iPivot;
            }
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
        short level = 0;
        getAndCacheKey(iMin, &(result.key));
        if( result.key == key)
        {
            return true;
        }
        if( result.key > key)
        {
            iMin = 0;
            return true;
        }

        getAndCacheKey(iMax, &(result.key));
        if( result.key < key)
        {
            iMin = iMax;
            return true;
        }

        while((iMax - iMin) > 1)
        {
            level++;
            uint64_t iPivot = (iMin + (iMax -iMin)/2);
            if(level < FILEINDEX_CACHELEVEL) getAndCacheKey(iPivot,&(result.key));
            else getKey(iPivot, &(result.key));
            if(result.key >= key) iMax = iPivot;
            else iMin = iPivot;
        }
        return true;
    };


    bool findLastLesser(KEY key, uint64_t& iMin, std::unordered_map<uint64_t, KEY>& local_cache)
    {
        Record<ITEM,KEY> result;
        iMin = 0;
        uint64_t iMax = getSize() - 1;
        short level = 0;
        getAndCacheKey(iMin, &(result.key));
        if( result.key == key)
        {
            return true;
        }
        if( result.key > key)
        {
            iMin = 0;
            return true;
        }

        getAndCacheKey(iMax, &(result.key));
        if( result.key < key)
        {
            iMin = iMax;
            return true;
        }

        while((iMax - iMin) > 1)
        {
            level++;
            uint64_t iPivot = (iMin + (iMax -iMin)/2);
            if(level < FILEINDEX_CACHELEVEL) getAndCacheKey(iPivot,&(result.key));

            else
            {
                auto it = local_cache.find(iPivot);
                if(it == local_cache.end())
                {
                    getKey(iPivot, &(result.key));
                    local_cache[iPivot] = result.key;
                } else {
                   result.key = it->second;
                }
            }
            if(result.key >= key) iMax = iPivot;
            else iMin = iPivot;
        }
        return true;
    };


// -- vectorized search
#define IDX_NOT_FOUND 0xFFFFFFFFFFFFFFFFULL

void findKeysIterate(const std::vector<std::pair<KEY, uint64_t*>>& sortedKeys, uint64_t kd, uint64_t kf, uint64_t iMin, uint64_t iMax, uint64_t level)
{
    //assert(kd < sortedKeys.size());
    //assert(kf < sortedKeys.size());
    level++;
    if (iMax < iMin+2)
    {
        for(uint64_t ind = kd; ind <= kf; ind++)
        {
            *(sortedKeys[ind].second) = IDX_NOT_FOUND;
        }
        return;
    }

    uint64_t iPivot = ( iMax + iMin ) / 2ULL;
    KEY key = sortedKeys[0].first;
    if(level < FILEINDEX_CACHELEVEL) getAndCacheKey(iPivot,&key);
    else getKey(iPivot, &key);
    if(key == sortedKeys[kd].first)
    {
        while(sortedKeys[kd].first == key)
        {
            *(sortedKeys[kd].second) = iPivot;
            if(kd >= sortedKeys.size() -1) return;
            kd++;
        }
        if(kd <= kf) findKeysIterate(sortedKeys, kd, kf, iMin, iMax, level);
    }
    else if(key == sortedKeys[kf].first)
    {
        while(sortedKeys[kf].first == key)
        {
            *(sortedKeys[kf].second) = iPivot;
            if(kf == 0) return;
            kf--;
        }
        if(kd <= kf) findKeysIterate(sortedKeys, kd, kf, iMin, iMax, level);
    }
    else if(key < sortedKeys[kd].first)
    {
        findKeysIterate(sortedKeys, kd, kf, iPivot, iMax, level);
    }
    else if(key > sortedKeys[kf].first)
    {
        findKeysIterate(sortedKeys, kd, kf, iMin, iPivot, level);
    }
    else
    {
        /*
        uint64_t kp = kd;
        while(key > sortedKeys[kp].first) kp++;
        uint64_t k0 = kp - 1;
        uint64_t k1 = kp;
        */
        uint64_t k0 = kd;
        uint64_t k1 = kf;
        while(k1 > k0+1)
        {
            uint64_t k2 = (k0+k1) / 2;
            if(key <= sortedKeys[k2].first) k1 = k2;
            else k0 = k2;
        }

        while(key == sortedKeys[k1].first)
        {
            *(sortedKeys[k1].second) = iPivot;
            k1++;
        }
        if(k0 >= kd) findKeysIterate(sortedKeys, kd, k0, iMin, iPivot, level);
        if(k1 <= kf) findKeysIterate(sortedKeys, k1, kf, iPivot, iMax, level);
    }
}

std::vector<uint64_t> findKeys(const std::vector<KEY>& keys)
{
    std::vector<std::pair<KEY,uint64_t*>> sortedKeys(keys.size());
    std::vector<uint64_t> results(keys.size());
    for(unsigned int i = 0; i < keys.size();i++)
    {
        sortedKeys[i] = {keys[i],&(results[i])};
    }
    // sort keys
    std::sort(sortedKeys.begin(), sortedKeys.end(), [](const std::pair<KEY, uint64_t*> &a, const std::pair<KEY, uint64_t*>  &b)
    {
        return a.first < b.first;
    });
//    std::sort(sortedKeys.begin(), sortedKeys.end(),);
    uint64_t iMin = 0;
    uint64_t iMax = getSize() - 1;
    short level = 0;
    KEY myKey;
    getAndCacheKey(iMin, &myKey);
    uint64_t kd = 0;
    uint64_t kf = keys.size() -1;

    while(sortedKeys[kd].first < myKey)
    {
        if(kd == sortedKeys.size() -1) return results;
        kd++;
    }
    while(sortedKeys[kd].first == myKey)
    {
        *(sortedKeys[kd].second) = iMin;
        if(kd == sortedKeys.size() -1) return results;
        kd++;
    }

    getAndCacheKey(iMax, &myKey);

    while(sortedKeys[kf].first > myKey)
    {
        if(kf == 0) return results;
        kf--;
    }
    while(sortedKeys[kf].first == myKey)
    {
        std::cout << iMax << "--\n";
        *(sortedKeys[kf].second) = iMax;
        if(kf == 0) return results;
        kf--;
    }
    if(kd <= kf) findKeysIterate(sortedKeys, kd, kf, iMin, iMax, level);
    return results;
}
// --
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
    GeoFile  pFile;
    ITEM *buffer;
    unsigned long bufferCount;
    uint64_t recSize;
    uint64_t fileSize;
    FileRawIndex(const FileRawIndex&);
    FileRawIndex& operator = (const FileRawIndex&);
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
        std::cout << "file name : " << this->filename << ":" << filename << "\n";

        pFile.open(filename, replace);
        buffer = nullptr;
        if (replace)  buffer = new ITEM[FILEINDEX_RAWFLUSHSIZE];
        bufferCount = 0;
        recSize = sizeof(ITEM);
        fileSize = pFile.length()/recSize;
        itemCount = 0;
    }
    /**
     * @brief Destroy the File Raw Index object
     *
     */
    virtual ~FileRawIndex()
    {
        if(buffer != nullptr) delete[] buffer;
    }
/**
 * @brief append record.
 *
 * @param record
 */
    void append(const ITEM& record)
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
        pFile.append((char*)&buffer[0], recSize * bufferCount);
        bufferCount = 0;
        fileSize = pFile.length()/recSize;

    }

    bool  get(uint64_t pos, ITEM* result )
    {
        if( pos >= getSize())
        {
            return false;
        }
        pFile.oread((char*)result, pos*recSize, recSize);
        return true;
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
private:
    FileRawData(const FileRawData&);
    FileRawData& operator = (const FileRawData&);
public:
    uint64_t startPos;
    GeoFile pFile;
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
        std::cout << "file name : " <<  filename << "\n";

        pFile.open(filename, replace);
        buffer = nullptr;
        if (replace)  buffer = new ITEM[FILEINDEX_RAWFLUSHSIZE];
        bufferCount = 0;
        recSize = sizeof(ITEM);
        fileSize = pFile.length()/recSize;
        startPos = 0;
        itemCount = 0;
        startCount = 0;

    }
    virtual ~FileRawData()
    {
        if(buffer != nullptr) delete[] buffer;
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
        if(count == 0) return nullptr;
        ITEM* result = reinterpret_cast<ITEM*>(malloc(sizeof(ITEM)*count));
        pFile.oread((char*)result, start * recSize, recSize * count);
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
        pFile.append((char*)&buffer[0], recSize * bufferCount);
        bufferCount = 0;
        fileSize = pFile.length()/recSize;
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
 * @brief storage for variable length records.
 *
 * @tparam ITEM
 */
template<class ITEM> class FileRawVarData
{
private:
    FileRawVarData(const FileRawVarData&);
    FileRawVarData& operator = (const FileRawVarData&);
public:
    uint64_t startPos;
    GeoFile pFile;
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
        if(replace) buffer = new char[FILEINDEX_RAWFLUSHSIZE];
        else buffer = nullptr;

        pFile.open(filename, replace);
        bufferCount = 0;
        itemCount = 0;
        startCount = 0;
        startPos = 0;
    }

    virtual ~FileRawVarData()
    {
        if(buffer != nullptr) delete[] buffer;
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
        if(count == 0) return nullptr;
        char* result = (char*) malloc(count);
        pFile.oread(result, start,  count);
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
        pFile.append(buffer,bufferCount);
        bufferCount = 0;
    }


};

/***********************************************************************/
template<class KEY> class KeyIndex
{
    private:
    bool sorted;
    KEY last_inserted{};
    std::string filename;
    KEY*  keyBuffer;
    unsigned long bufferCount;
    uint64_t keySize;
    uint64_t sortedSize;
    std::unordered_map<uint64_t, KEY> cacheKey;
    std::mutex cache_mutex;
    KeyIndex(const KeyIndex&);
    KeyIndex& operator = (const KeyIndex&);
public:
    uint64_t fileSize;
    GeoFile keyFile;

    bool isSorted(){return sorted;}

    /**
     * @brief Construct a new File Index object.
     *
     * @param filename_  name of the fiile.
     * @param replace    true if file shall be created.
     */
    KeyIndex(const std::string& filename_, bool replace) : filename(filename_)
    {

        keyFile.open(filename + "_key", replace);

        keyBuffer  = nullptr;

        if (replace)
        {
            keyBuffer  = new KEY[FILEINDEX_RAWFLUSHSIZE];
        }
        bufferCount = 0;
        keySize  = sizeof(KEY);
        fileSize = keyFile.length()/keySize;
        sortedSize = 0;
        sorted = true;
    }

    /**
     * @brief Destroy the File Index object
     *
     */
    virtual ~KeyIndex()
    {
        if(keyBuffer != nullptr) delete[] keyBuffer;
    }

/**
 * @brief append item to index.
 *
 * @param key
 */
    void append(const KEY& key)
    {
        keyBuffer[bufferCount] = key;
        bufferCount ++;

        if (bufferCount == FILEINDEX_RAWFLUSHSIZE)
        {
            flush();
        }
        if((fileSize) && (key < last_inserted)) sorted = false;
        last_inserted = key;
    }


/**
 * @brief flush index file to disk.
 *
 */
    void flush()
    {
        keyFile.append((char*)&keyBuffer[0], keySize * bufferCount);
        bufferCount = 0;
        fileSize = keyFile.length()/keySize;
    }
/**
 * @brief swap items in index
 *
 * @param item1
 * @param item2
 */
    void swap(uint64_t n1, uint64_t n2)
    {
        KEY   key1, key2;

        keyFile.oread((char*)&key1, n1 * keySize, keySize);
        keyFile.oread((char*)&key2, n2 * keySize, keySize);
        keyFile.owrite((char*)&key2, n1 * keySize, keySize);
        keyFile.owrite((char*)&key1, n2 * keySize, keySize);

    }

    /**
     * @brief sort index
     *
     */

    void sort()
    {
        struct sysinfo info;
        int res = sysinfo(&info);
        uint64_t max_mem = 500000000ULL; // 500 mbytes
        if(res == 0)
        {
            max_mem = (((unsigned long) info.freeram) * (( unsigned long) info.mem_unit)) / 2;
        }

        std::cerr << "ram :" << (((unsigned long) info.freeram) * (( unsigned long) info.mem_unit)) / (1048 * 1048) << "mb, sorting  " << fileSize << " elements \n";
        if(!fileSize) return;
        sortedSize = 0;

        uint64_t buffer_size = (fileSize + 3) / 3;

        uint64_t max_bsize = max_mem / (sizeof(KEY)*3);
        if(buffer_size > max_bsize) buffer_size = max_bsize;

        KEY*  key_sort_buffer = nullptr;

        bool resized = false;
        while(key_sort_buffer == nullptr)
        {
            try
            {
              key_sort_buffer  = new KEY[buffer_size*3];
            } catch(std::bad_alloc& ba)
            {
                std::cout << "buffer_size : " << buffer_size << "too big\n";
                buffer_size /= 2;
                resized = true;
            }
            if(resized == true)
            {
                delete[] key_sort_buffer;
                buffer_size /= 2;
                key_sort_buffer  = new KEY[buffer_size*3];
            }
        }
        sort(0,fileSize-1, key_sort_buffer, buffer_size);
        sorted = true;
        delete[] key_sort_buffer;
    }


    void swapKey(KEY* a, KEY* b)
    {
        KEY c;
        memcpy(&c, a, keySize);
        memcpy(a, b, keySize);
        memcpy(b, &c, keySize);
    }

    void memsort(int64_t begin, int64_t end, KEY* key_sort_buffer)
    {
        //std::cout << "memsort " << begin <<" "<< end <<"\n";
        if((end - begin) < 1) return;
        if((end - begin) == 1)
        {
            if ( key_sort_buffer[end] < key_sort_buffer[begin] )
            {
                swapKey(key_sort_buffer + end, key_sort_buffer + begin);
            }
            return;
        }
        else
        {
            int64_t lessers = 0;
            int64_t biggers = 0;
            int64_t total = end - begin;
            int64_t ipivot = begin;
            while(total > (biggers + lessers))
            {
                while((total > (biggers + lessers))
                && (begin + 1 + lessers <= end)
                && (key_sort_buffer[begin + 1 + lessers] < key_sort_buffer[begin])) lessers++;
                while((total > (biggers + lessers))
                && (end - biggers >= begin + 1)
                && (key_sort_buffer[end - biggers ] > key_sort_buffer[begin])) biggers++;
                if(total > (biggers + lessers + 1))
                {
                    swapKey(key_sort_buffer + (begin+1+lessers), key_sort_buffer + (end - biggers));

                    biggers++;
                    lessers++;
                } else if (total == (biggers + lessers + 1)) {
                    if(key_sort_buffer[begin + 1 + lessers] < key_sort_buffer[begin]) lessers++;
                    else biggers++;
                }
            }
            ipivot = begin + lessers;
            swapKey(key_sort_buffer + ipivot, key_sort_buffer + begin);

            if((ipivot + 1)< end) memsort(ipivot + 1, end, key_sort_buffer);
            if(ipivot > (begin + 1)) memsort(begin, ipivot - 1,  key_sort_buffer);
        }
    }


    /**
     * @brief sort index part.
     *
     * @param begin
     * @param end
     * @param sort_buffer
     * @param buffer_size
     */

    void sort(uint64_t begin, uint64_t end, KEY* key_sort_buffer, uint64_t buffer_size)
    {
        std::cerr << "sort " << begin <<" "<< end <<"\n";
        uint64_t count = (end - begin) + 1;
        if((end - begin + 1) < (3ULL*buffer_size))
        {
            std::cout << "enough memory, performing qsort \n";
            std::cout << "read data \n";
            keyFile.oread((char*)key_sort_buffer, begin * keySize, keySize * count);
            std::cout << "sort data \n";

            //std::qsort(&sort_buffer[0], count, recSize,fileIndexComp<ITEM,KEY>);
            memsort(0, count - 1, key_sort_buffer);

            std::cout << "write data \n";
            keyFile.owrite((char*)key_sort_buffer, begin * keySize, keySize * count );
            sortedSize += count;
            std::cerr << "*** *** *** *** sorted " << sortedSize << " out of " << fileSize << "\n";
            return;
        }
        if((end - begin) > 2)
        {
            uint64_t i = (end + begin) / 2;
            swap(i,end);
        }

        KEY* key_plusPetits = key_sort_buffer;
        KEY* key_plusGrands = key_sort_buffer + buffer_size;
        KEY* key_autres = key_sort_buffer + 2ULL*buffer_size;

        KEY curKey;


        //Record<ITEM,KEY> tmp;
        getKey(end, &curKey);
        uint64_t autresCount = end - begin;
        uint64_t plusGrandsCount = 0;
        uint64_t plusPetitsCount = 0;
        while(autresCount > 0)
        {
            uint64_t plusPetitsBufferCount=0;
            uint64_t plusGrandsBufferCount=0;
            uint64_t autresBufferCount=0;
            uint64_t toRead = buffer_size;
            //std::cout << "reading " << toRead << "items from file \n";
            if(toRead > autresCount) toRead = autresCount;

            keyFile.oread((char*)key_autres, (end - (toRead  + plusGrandsCount)) * keySize, keySize * toRead );

            autresBufferCount = toRead;
            plusPetitsBufferCount = 0;
            plusGrandsBufferCount = 0;
            autresCount -= toRead;
            //std::cout << "dividing items \n";
            while(autresBufferCount > 0)
            {
                autresBufferCount --;
                if(key_autres[autresBufferCount] >  curKey)
                {
                    key_plusGrands[plusGrandsBufferCount] = key_autres[autresBufferCount];
                    plusGrandsBufferCount++;
                }
                else if(key_autres[autresBufferCount] <  curKey)
                {
                    key_plusPetits[plusPetitsBufferCount] = key_autres[autresBufferCount];
                    plusPetitsBufferCount++;
                }
                else
                {
                    if(plusGrandsBufferCount> plusPetitsBufferCount)
                    {
                        key_plusPetits[plusPetitsBufferCount] = key_autres[autresBufferCount];
                        plusPetitsBufferCount++;
                    }
                    else
                    {
                        key_plusGrands[plusGrandsBufferCount] = key_autres[autresBufferCount];
                        plusGrandsBufferCount ++;
                    }
                }
            }
            //std::cout << "divided items \n";

            if(plusGrandsBufferCount)
            {
                //std::cout << plusGrandsBufferCount << " items where bigger writing them to file\n";
                keyFile.owrite((char*)key_plusGrands, (end - (plusGrandsCount + plusGrandsBufferCount - 1)) * keySize, keySize * plusGrandsBufferCount);
            }
            plusGrandsCount += plusGrandsBufferCount;
            //std::cout << plusPetitsBufferCount << " items where smaller writing them to file\n";
            if(autresCount  < plusPetitsBufferCount)
            {
                std::cout << " cache " << autresCount << " items \n";

                keyFile.oread((char*)key_autres, (begin + plusPetitsCount) * keySize, keySize * autresCount);

                if(plusPetitsBufferCount)
                {
                    //std:: cout << "write smallers \n";
                    keyFile.owrite((char*)key_plusPetits, (begin + plusPetitsCount) * keySize, keySize * plusPetitsBufferCount);
                    plusPetitsCount += plusPetitsBufferCount;
                }
                if(autresCount)
                {
                    //std:: cout << "write cached \n";
                    keyFile.owrite((char*)key_autres, (begin+plusPetitsCount) * keySize, keySize * autresCount);
                }

            }
            else
            {
                if(plusPetitsBufferCount)
                {

                    //std::cout << " cache " << plusPetitsCount << " items \n";
                    keyFile.oread((char*)key_autres, (begin + plusPetitsCount) * keySize, keySize * plusPetitsBufferCount);
                    //std:: cout << "write smallers \n";
                    keyFile.owrite((char*)key_plusPetits, (begin + plusPetitsCount) * keySize, keySize * plusPetitsBufferCount);

                    plusPetitsCount += plusPetitsBufferCount;
                    //std:: cout << "write cached \n";
                    keyFile.owrite((char*)key_autres, (end - (plusGrandsCount + plusPetitsBufferCount)) * keySize, keySize * plusPetitsBufferCount);
                }
            }
            //std:: cout << "write pivot at the right place \n";
            keyFile.owrite((char*)&(curKey), (end  - (plusGrandsCount)) * keySize, keySize);

        }
        sortedSize++;
        std::cerr << "*** *** *** *** sorted " << sortedSize << " out of " << fileSize << "\n";
        if(plusGrandsCount > 1) sort(end - (plusGrandsCount - 1), end, key_sort_buffer, buffer_size);
        else if(!plusGrandsCount) sortedSize++;
        if(plusPetitsCount > 1) sort(begin, begin + plusPetitsCount - 1, key_sort_buffer, buffer_size);
        else if(!plusPetitsCount) sortedSize++;
    }

/**
 * @brief Get And Cache object
 *
 * @param pos
 * @param result
 * @return true
 * @return false
 */

    bool  getAndCacheKey(const uint64_t pos, KEY* result)
    {
#ifndef DISCARD_MUTEX
        std::lock_guard<std::mutex> guard(cache_mutex);
#endif
        auto it = cacheKey.find(pos);
        if(it == cacheKey.end())
        {
            if(getKey(pos, result))
            {
                cacheKey[pos] = *result;
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


    inline bool  getKey(const uint64_t pos, KEY* result )
    {
        if( pos >= getSize())
        {
            //std::cerr << "key not found ! " << (int) pos << (int) getSize() << "/n";
            return false;
        }
        keyFile.oread((char*)result, pos*keySize, keySize);
        return true;
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
    bool find(KEY key, uint64_t* pos)
    {
        uint64_t iMin = 0;
        uint64_t iMax = getSize() - 1;
        short level = 0;
        KEY myKey;

        getAndCacheKey(iMin, &myKey);
        if( myKey == key)
        {
            *pos = iMin;
            return true;
        }
        if( myKey > key)
        {
            return false;
        }
        getAndCacheKey(iMax, &myKey);
        if( myKey == key)
        {
            *pos = iMax;
            return true;
        }
        if( myKey < key)
        {
            return false;
        }


        while((iMax - iMin) > 1)
        {
            level++;
            uint64_t iPivot = (iMin + iMax) >> 1;
            if(level < FILEINDEX_CACHELEVEL) getAndCacheKey(iPivot,&myKey);
            else getKey(iPivot, &myKey);
            if(myKey > key)
            {
                iMax = iPivot;
            }
            else if(myKey == key)
            {
                *pos = iPivot;
                return true;
            }
            else
            {
                iMin = iPivot;
            }
        }
        return false;
    }
};

/***********************************************************************/

}
#endif
