#ifndef RASTERIMAGE_HPP
#define RASTERIMAGE_HPP
#include "../messaging/HttpEncoder.hpp"
#include "../messaging/Msg.hpp"
#include "ServiceInterface.hpp"
#include "../CompiledDataManager.hpp"
class RasterImage : public ServiceInterface
{
private:
    HttpEncoder encoder;
    long _x, _y, _z;
    const std::vector<PngImage>& _imageList;
public:
    Msg* processRequest(Msg* request, CompiledDataManager& mger) override;
    explicit RasterImage(long z, long x, long y, const std::vector<PngImage>& imageList):_x(x), _y(y), _z(z),_imageList(imageList)
    {
    }
};
#endif
