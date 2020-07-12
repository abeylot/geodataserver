#ifndef GEOTYPES_HPP
#define GEOTYPES_HPP
#pragma pack (push, 1)
enum BaliseType {relation, point, way, unknown };

/**
 * @brief Structure to store geographic data position in indexes.
 * 
 */
struct GeoIndex
{
    // start position of tags in tag file
    uint64_t tstart;
    // size of tags data in tag file.
    uint32_t tsize;
    // size of members data
    uint32_t msize;
    // start position of members
    uint64_t mstart;
};
/**
 * @brief structure to store geographic poins dat in indexes.
 * 
 */
struct GeoPointIndex
{
    // internal format  longitude
    uint32_t x;
    // internal format lattitude
    uint32_t y;
    // start position of tags in tag file
    uint64_t tstart;
    // size of tags data in tag file.
    uint32_t tsize;
};
/**
 * @brief structure to store way data in indexes file
 * 
 */
struct GeoWayIndex
{
    // start position of points data in point data file
    uint64_t pstart;
    // size of points data in point data file
    uint32_t psize;
    // size of tags data in tag file.
    uint32_t tsize;
    // start position of tags in tag file
    uint64_t tstart;
};
/**
 * @brief Points data structure.
 * 
 */
struct GeoPoint
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
struct GeoMember
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
struct GeoString
{
    std::string value;
    inline uint64_t size()
    {
        return value.length() ;
    }
    inline const char* buffer()
    {
        return value.c_str();
    }
};

/**
 * @brief struct to store geopoint data with it's id
 * 
 */
struct GeoPointNumberIndex
{
    uint64_t number;
    uint32_t x;
    uint32_t y;
};
#pragma pack (pop)
#endif
