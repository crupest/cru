#include "cru/platform/graphics/cairo/CairoResource.h"
#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"

namespace cru::platform::graphics::cairo {
static const std::string kCairoGraphicsPlatformId("cairo");

CairoResource::CairoResource(CairoGraphicsFactory* factory)
    : factory_(factory) {}

CairoResource::~CairoResource() {}

std::string CairoResource::GetPlatformId() const {
  return kCairoGraphicsPlatformId;
}

IGraphicsFactory* CairoResource::GetGraphicsFactory() { return factory_; }
}  // namespace cru::platform::graphics::cairo
