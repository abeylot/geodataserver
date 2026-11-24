# User Story: PNG tiles rasterized from the SVG tile output

**As a** map-application developer
**I want** `/{z}/{x}/{y}.png` to return the SVG tile for that z/x/y, rasterized to a 256x256 PNG
**So that** any standard raster-tile client can use the server, and the PNG always matches the SVG in data, style and labels.

## Approach (option A: rasterize the SVG string)

1. `.png` routes to the existing `Tile` service with an output-format flag.
2. `Tile` calls `SvgRenderer::renderItems(...)` exactly as it does for `.svg`.
3. The SVG string is rasterized in memory and returned as `image/png`. Nothing is written to disk except the cache file.

## Library choice

The SVGs use CSS classes (`CssClass`), so the rasterizer must support `<style>` and class selectors.

| Library | CSS classes | Footprint | Verdict |
|---|---|---|---|
| **librsvg + cairo** | Full | Pulls in glib and cairo | **Recommended.** Renders our styles correctly; `cairo_surface_write_to_png_stream` writes the PNG directly. |
| resvg (C API) | Good | Rust toolchain needed | Fallback if glib is a problem. |
| nanosvg | Poor, limited `<style>` support | Header-only | Rejected: styling would drift from the SVG. |

## Acceptance criteria

1. **Same content.** The PNG shows the same features, colours, labels and zoom filtering as the SVG for the same z/x/y.
2. **Routing.** `.png` goes to the tile service when `_enabledTileService` is on, and returns 404 when it is off. `.svg` is unchanged.
3. **Validation.** `0 <= z <= 25` and `0 <= x,y < 2^z`, otherwise 404, shared with the SVG parsing.
4. **Output.** `Content-Type: image/png`, 256x256 RGBA, background taken from `_defaultColor`.
5. **Cache.** Same policy as SVG: stored under `cache/{z}/{x}/{y}.png` when `z <= _cacheLevel`, with an ETag and a 304 response. `.png` and `.svg` cache files never collide.
6. **Memory.** The SVG string and the cairo surface are freed per request. No decoded-image caches or persistent buffers; PNG bytes are not gzip-compressed a second time.
7. **Performance.** An uncached z14 complex tile takes no more than about 1.5x the SVG render time.
8. **Example page.** `test.html` has a button that switches the Leaflet tile layer between `/{z}/{x}/{y}.svg` and `/{z}/{x}/{y}.png`, keeping the current map position and zoom. The label shows the format it will switch to.
9. **Build.** `Makefile`, `prepare.sh` and the Docker scripts add the librsvg and cairo dev packages.

## Definition of done

- Integration test on 3 tiles (dense city z14, coastline z8, empty ocean z3): the PNG is 256x256 and non-blank where the SVG has features.
- 404 on out-of-range tiles; cache hit, miss and 304 work for `.png`.
- `README.md` and `docs/index.md` document the endpoint and the new dependency.

## Open question

The old georeferenced-image overlay (`RasterImage` and `PngRenderer`) currently owns the `.png` route. It must move to a separate route such as `/img/{z}/{x}/{y}.png`, or be removed.
