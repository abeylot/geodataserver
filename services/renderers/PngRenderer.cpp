#include "PngRenderer.hpp"
#include <sstream>
#include <png++/png.hpp>


std::string PngRenderer::render(const Rectangle& rect, uint32_t sizex, uint32_t sizey)
{
    png::image< png::rgba_pixel> image(256,256);
    for(unsigned int i = 0; i < sizey; i++)
    {
        for(unsigned int j = 0; j < sizex; j++)
        {
            image[i][j] = png::rgba_pixel(0,0,0,1);
        }
    }


    double my_lat_min = Coordinates::fromNormalizedLat(rect.y1);
    double my_lat_max = Coordinates::fromNormalizedLat(rect.y0);
    double my_lon_min = Coordinates::fromNormalizedLon(rect.x0);
    double my_lon_max = Coordinates::fromNormalizedLon(rect.x1);

    for(auto img : _imageList)
    {
       if(
       (my_lat_max >= img.lat_min) and
       (my_lat_min <= img.lat_max) and
       (my_lon_max >= img.lon_min) and
       (my_lon_min <= img.lon_max)
       )
       {
       png::image<png::rgba_pixel>* myImg = new png::image<png::rgba_pixel>(_imagePath + img.filename);
       for(unsigned int i = 0; i < sizex; i++){
               double posx = Coordinates::fromNormalizedLon(rect.x0*(1.0 - i/((double)sizex)) + rect.x1*(i/(double)sizex));
               if((posx >= img.lon_min) && (posx < img.lon_max))
               {
               uint32_t posxInImg = ((posx - img.lon_min) * myImg->get_width()) / (img.lon_max - img.lon_min);
               for(unsigned int j = 0; j < 256; j++){
                   double posy = Coordinates::fromNormalizedLat(rect.y0*(1.0 - j/(double)sizey) + rect.y1*(j/(double)sizey));
                   if((posy >= img.lat_min) && (posy < img.lat_max))
                   {
                       uint32_t posyInImg = ((posy - img.lat_max) * myImg->get_height()) / (img.lat_min - img.lat_max);
                       if(posyInImg >= myImg->get_width()) posyInImg = myImg->get_width() - 1;
                       if(posxInImg >= myImg->get_height()) posxInImg = myImg->get_height() - 1;
                       image[j][i] = (*myImg)[posyInImg][posxInImg];
                   }
               }
           }
       }
       delete myImg;
       }
    }
    std::stringstream sstream;
    image.write_stream(sstream);
    return std::move(sstream.str());
}

