#ifndef PNGRENDERER_HPP
#define PNGRENDERER_HPP

#include "../../helpers/config.hpp"
#include <vector>
#include <string>

class PngRenderer
{
private:
    std::vector<PngImage> _imageList;
    std::string _imagePath;

public:
    explicit PngRenderer(const std::vector<PngImage>& imageList, const std::string& imagePath):_imageList(imageList), _imagePath(imagePath)
    {
    }
    std::string render(const Rectangle& rect, uint32_t sizex, uint32_t sizey);
};

#endif
