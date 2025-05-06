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
    unsigned int _cachelevel;
    const std::vector<PngImage>& _imageList;
public:
    std::shared_ptr<Msg> processRequest(std::shared_ptr<Msg> request, CompiledDataManager& mger) override;
    explicit RasterImage(long z, long x, long y, unsigned int cachelevel, const std::vector<PngImage>& imageList):
    _x(x),
    _y(y),
    _z(z),
    _cachelevel(cachelevel),
    _imageList(imageList)
    {
    }
    const std::string get_name() override { return "RasterImage";}
};
#endif
