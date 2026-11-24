#ifndef PNGRASTERIZER_HPP
#define PNGRASTERIZER_HPP
#include <string>
#include <cstdint>

// Rasterizes an SVG document to PNG bytes. Returns false on failure.
bool rasterizeSvgToPng(const std::string& svg, uint32_t width, uint32_t height,
                       const std::string& background, std::string& pngOut);
#endif
