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
           png::image<png::rgba_pixel>* myImg = new png::image<png::rgba_pixel>(_imagePath + img.filename);
           for(unsigned int i = 0; i < sizex; i++){
                   long posx = rect.x0*(1.0 - i/((double)sizex)) + rect.x1*(i/(double)sizex);
                   if((posx >= rect2.x0) && (posx < rect2.x1))
                   {
                   uint32_t posxInImg = ((posx - (long)(img.quadrilateral[0].x)) * myImg->get_width()) / ((long)(img.quadrilateral[1].x) -(long)(img.quadrilateral[0].x));
                   for(unsigned int j = 0; j < 256; j++){
                       long posy = rect.y0*(1.0 - j/(double)sizey) + rect.y1*(j/(double)sizey);
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
    return std::move(sstream.str());
}

