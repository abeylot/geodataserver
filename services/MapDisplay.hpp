#include "ServiceInterface.hpp"
#include "../CompiledDataManager.hpp"
#include "../messaging/HttpEncoder.hpp"

#ifndef MAPDISPLAY_HPP
#define MAPDISPLAY_HPP


class MapDisplay : public ServiceInterface
{
private:
    HttpEncoder encoder;
    bool _enabledRasterImageService;

public:
    explicit MapDisplay(bool RasterImageService_enabled);
    ~MapDisplay();
    std::shared_ptr<Msg> processRequest(std::shared_ptr<Msg> request, CompiledDataManager& mger) override;

};

#endif // MAPDISPLAY_HPP
