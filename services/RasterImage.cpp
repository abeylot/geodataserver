#include "RasterImage.hpp"
#include "renderers/PngRenderer.hpp"
#include <math.h>
#include <sstream>
#include <png++/png.hpp>

inline double tilex2long(int x, int z)
{
    int div = 1 << z;
    double result =  (double)x / (double)div * 360.0 - 180;
    if (result >= 179.999) result = 179.999;
    return result;
}

inline double tiley2lat(int y, int z)
{
    double n = M_PI - 2.0 * M_PI * y / pow(2.0, z);
    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}


Msg* RasterImage::processRequest([[maybe_unused]] Msg* request, [[maybe_unused]] CompiledDataManager& mger)
{

    double lon1, lon2, lat1, lat2;
    static std::mutex file_mtx;

    lon1 = tilex2long(_x, _z);
    lon2 = tilex2long(_x+1, _z);

    lat1 = tiley2lat(_y, _z);
    lat2 = tiley2lat(_y+1, _z);



    Rectangle rect;

    rect.x0 = Coordinates::toNormalizedLon(std::to_string(lon1));
    rect.y0 = Coordinates::toNormalizedLat(std::to_string(lat1));

    rect.x1 = Coordinates::toNormalizedLon(std::to_string(lon2));
    rect.y1 = Coordinates::toNormalizedLat(std::to_string(lat2));


    Msg* rep = new Msg;
    HttpEncoder encoder;
    encoder.build200Header(rep, "image/png");
    PngRenderer r(_imageList, mger.path + "/images/");
    encoder.addContent(rep,r.render(rect,256,256));
    return rep;
}
