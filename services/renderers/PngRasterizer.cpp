#include "PngRasterizer.hpp"
#include <librsvg/rsvg.h>
#include <cairo.h>
#include <cstdio>

static cairo_status_t appendToString(void* closure, const unsigned char* data, unsigned int length)
{
    static_cast<std::string*>(closure)->append(reinterpret_cast<const char*>(data), length);
    return CAIRO_STATUS_SUCCESS;
}

// Accepts "#rrggbb"; anything else gives white.
static void parseColor(const std::string& color, double& r, double& g, double& b)
{
    r = g = b = 1.0;
    if (color.size() != 7 || color[0] != '#') return;
    unsigned int value = 0;
    if (sscanf(color.c_str() + 1, "%6x", &value) != 1) return;
    r = ((value >> 16) & 0xFF) / 255.0;
    g = ((value >> 8) & 0xFF) / 255.0;
    b = (value & 0xFF) / 255.0;
}

bool rasterizeSvgToPng(const std::string& svg, uint32_t width, uint32_t height,
                       const std::string& background, std::string& pngOut)
{
    GError* error = nullptr;
    RsvgHandle* handle = rsvg_handle_new_from_data(
        reinterpret_cast<const guint8*>(svg.data()), svg.size(), &error);
    if (handle == nullptr)
    {
        g_error_free(error);
        return false;
    }

    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t* cr = cairo_create(surface);

    double r, g, b;
    parseColor(background, r, g, b);
    cairo_set_source_rgb(cr, r, g, b);
    cairo_paint(cr);

    RsvgRectangle viewport = {0.0, 0.0, double(width), double(height)};
    bool ok = rsvg_handle_render_document(handle, cr, &viewport, &error);
    if (!ok) g_error_free(error);

    ok = ok && cairo_surface_write_to_png_stream(surface, appendToString, &pngOut) == CAIRO_STATUS_SUCCESS;

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    g_object_unref(handle);
    return ok;
}
