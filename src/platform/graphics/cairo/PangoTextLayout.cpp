#include "cru/platform/graphics/cairo/PangoTextLayout.h"

namespace cru::platform::graphics::cairo {
PangoTextLayout::PangoTextLayout(CairoGraphicsFactory* factory)
    : CairoResource(factory) {}

PangoTextLayout::~PangoTextLayout() {}

}  // namespace cru::platform::graphics::cairo
