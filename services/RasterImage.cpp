#include "RasterImage.hpp"
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
    std::string res = "";
    // iterate on png images, and build png result
    //
    png::image< png::rgba_pixel> image(256,256);
    for(int i = 0; i < 256; i++)
    {
        for(int j = 0; j < 256; j++)
        {
            image[i][j] = png::rgba_pixel(0,0,0,1);
        }
    }
    for(auto img : _imageList)
    {
        Rectangle rect2;
        rect2.x0 = rect2.x1 = img.quadrilateral[0].x;
        rect2.y0 = rect2.y1 = img.quadrilateral[0].y;

        rect2.addPoint(img.quadrilateral[1].x,img.quadrilateral[1].y);
        rect2.addPoint(img.quadrilateral[2].x,img.quadrilateral[2].y);
        rect2.addPoint(img.quadrilateral[3].x,img.quadrilateral[3].y);

        if((rect*rect2).isValid())
        {
          //png::image<png::ga_pixel> myImg(6000,6000);
          //myImg.read(mger.path + "/images/" + img.filename);
           png::image<png::rgba_pixel>* myImg = new png::image<png::rgba_pixel>(mger.path + "/images/" + img.filename);
          // std::cout << "rect  " << rect.x0 << ',' << rect.y0 << ',' << rect.x1 << ',' << rect.y1 << ',' << "\n";
           //std::cout << "rect2 " << rect2.x0 << ',' << rect2.y0 << ',' << rect2.x1 << ',' << rect2.y1 << ',' << "\n";
           //for(int i = 0; i < 256; i++) for(int j = 0; j < 256; j++) image[i][j] = png::ga_pixel(i,50);
           for(int i = 0; i < 256; i++){
                   long posx = rect.x0*(1.0 - i/256.0) + rect.x1*(i/256.0);
                   if((posx >= rect2.x0) && (posx < rect2.x1))
                   {
                   uint32_t posxInImg = ((posx - (long)(img.quadrilateral[0].x)) * myImg->get_width()) / ((long)(img.quadrilateral[1].x) -(long)(img.quadrilateral[0].x));
                   for(int j = 0; j < 256; j++){
                       long posy = rect.y0*(1.0 - j/256.0) + rect.y1*(j/256.0);
                       if((posy >= rect2.y0) && (posy < rect2.y1))
                       {
                           uint32_t posyInImg = ((Coordinates::fromNormalizedLat(posy) - Coordinates::fromNormalizedLat(img.quadrilateral[0].y)) * myImg->get_height()) / (Coordinates::fromNormalizedLat(img.quadrilateral[2].y) -Coordinates::fromNormalizedLat(img.quadrilateral[0].y)) + 1;
                           image[j][i] = (*myImg)[myImg->get_height() - posyInImg][posxInImg];
                       }
                   }
                }
           }
           delete myImg;
       }
    }
    std::stringstream sstream;
    image.write_stream(sstream);
    encoder.addContent(rep,sstream.str());
    return rep;
}
