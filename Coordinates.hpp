#ifndef COORDINATES_HPP
#define COORDINATES_HPP
#include <stdint.h>
#include <string>
/**
 * @brief class to perform internal  external coordinates transformations.
 *
 */
class Coordinates
{
public :
    /**
     * @brief  converts string coordinates to internal format.
     *
     * @param coord  longitude in degrees + decimals.
     * @return uint32_t  internal integer;
     */
    static uint32_t toNormalizedLon(const std::string& coord);
    static uint32_t toNormalizedLon(double coord);

    static double fromNormalizedLon(const uint32_t coord);
    /**
     * @brief converts string coordinates to internal format.
     *
     * @param coord latitude in degrees + decimals.
     * @return uint32_t internal integer.
     */
    static uint32_t toNormalizedLat(const std::string& coord);
    static uint32_t toNormalizedLat(double coord);

    static double fromNormalizedLat(const uint32_t coord);
    /**
     * @brief Converts uint32_t to hex.
     *
     * @param coord int to convert.
     * @return std::string hex result as string.
     */
    static std::string toHex(const uint32_t coord);
    /**
     * @brief Converts hex string  to uint32_t
     *
     * @return uint32_t the result.
     */
    static uint32_t fromHex(const std::string&);

private:
    static uint32_t encode(uint64_t intCoord, uint32_t maxRange);
};
#endif
