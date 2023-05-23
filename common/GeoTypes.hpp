#ifndef GEOTYPES_HPP
#define GEOTYPES_HPP
enum __attribute__((packed)) BaliseType {relation, point, way, unknown };

/**
 * @brief Structure to store geographic data Relation in indexes.
 * 
 */
struct __attribute__((packed)) GeoIndex
{
    // start position of tags in tag file
    uint64_t tstart;
    // size of tags data in tag file.
    uint16_t tsize;
    // size of members data
    uint16_t msize;
    // start position of members
    uint64_t mstart;
};
/**
 * @brief structure to store geographic Point data in indexes.
 * 
 */
struct __attribute__((packed)) GeoPointIndex 
{
    // internal format  longitude
    uint32_t x;
    // internal format lattitude
    uint32_t y;
    // start position of tags in tag file
    uint64_t tstart;
    // size of tags data in tag file.
    uint16_t tsize;
};
/**
 * @brief structure to store Way data in indexes file
 * 
 */
struct __attribute__((packed)) GeoWayIndex 
{
    // start position of points data in point data file
    uint64_t pstart;
    // size of points data in point data file
    uint16_t psize;
    // size of tags data in tag file.
    uint16_t tsize;
    // start position of tags in tag file
    uint64_t tstart;
};
/**
 * @brief Points data structure.
 * 
 */
struct __attribute__((packed)) GeoPoint 
{
    // internal format  longitude
    uint32_t x;
    // internal format  latitude
    uint32_t y;
};

/**
 * @brief geopoint equality operator.
 * 
 * @param a point a
 * @param b point b
 * @return true if a == b
 * @return false otherwise
 */
inline bool operator==(GeoPoint const& a, GeoPoint const& b)
{
    //return ((a.x==b.x) && (a.y == b.y));
    return (!memcmp(&a,&b,sizeof(GeoPoint)));
}

/**
 * @brief geographic member data.
 * 
 */
struct __attribute__((packed)) GeoMember
{
    // type of data
    BaliseType type;
    //id
    uint64_t id;
};

/**
 * @brief struct to store tag strings.
 * 
 */
struct __attribute__((packed)) GeoString 
{
    char _buffer[256];
    unsigned char _length;
    
    GeoString & operator=(const std::string &s)
    {
        if(s.length() <= 255) _length = s.length();
        else _length = 255;
        memcpy(_buffer, s.c_str(),_length);
        return *this;
    }
    
    GeoString()
    {
        _length=0;
    }
    
    inline uint64_t size()
    {
        return _length;
    }
    inline const char* buffer()
    {
        return _buffer;
    }
};

/**
 * @brief struct to store geopoint data with it's id
 * 
 */
struct __attribute__((packed)) GeoPointNumberIndex 
{
    uint64_t number;
    uint32_t x;
    uint32_t y;
};
#endif
