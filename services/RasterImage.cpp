#include "RasterImage.hpp"
#include "renderers/PngRenderer.hpp"
#include <math.h>
#include <sstream>
#include <png++/png.hpp>
#include <filesystem>


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

    bool filefound = false;
    char filename[250];
    std::string res="";
    FILE* in;
    
    if(_z <= _cachelevel)
    {
        snprintf(filename,250,"%s/cache/%ld/%ld/%ld.png",mger.path.c_str(),_z,_x,_y);
        in = fopen(filename, "r");
        if(in != NULL)
        {
            filefound=true;
            char buffer[4097];
            size_t len = 0;
            while( (len = fread(buffer,1,4096,in)) )
            {
                buffer[len] = 0;
                res.append(buffer, len);
            }
        }
    }
    if(filefound)
    {
    //printf("USING CACHE !\n");
        fclose(in);
        encoder.addContent(rep,res);
    }else{
        std::string dir1 = mger.path + "/cache/" + std::to_string(_z);
        std::string dir2 = dir1 + "/" + std::to_string(_x);

        res = r.render(rect,256,256);
        //std::cout << "cache level " << _cachelevel << "\n";
        if(_z <= _cachelevel && res.length() > 2048)
        {
            std::filesystem::path p1(dir1);
            std::filesystem::path p2(dir2);

            if(! std::filesystem::exists(p1))
            {
                std::filesystem::create_directory(p1);
            }

            if(! std::filesystem::exists(p2))
            {
                std::filesystem::create_directory(p2);
            }
            {
                std::lock_guard<std::mutex> guard(file_mtx);
                FILE* out = fopen(filename, "w");
                if(out != NULL)
                {
                    fwrite(res.c_str(), res.size(),1,out);
                    fclose(out);
                }
                else
                {
                    printf("UNABLE TO CACHE %s !\n", filename);
                }
            }
        }
    }
    encoder.addContent(rep,res);
    return rep;
}
