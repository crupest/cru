#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"
#include "cru/platform/graphics/cairo/CairoResource.h"

namespace cru::platform::graphics::cairo {
CairoGraphicsFactory::CairoGraphicsFactory() : CairoResource(this) {}

CairoGraphicsFactory::~CairoGraphicsFactory() {}
}  // namespace cru::platform::graphics::cairo
