#include "cru/platform/graphics/cairo/CairoResource.h"
#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"

namespace cru::platform::graphics::cairo {
static const String kCairoGraphicsPlatformId(u"cairo");

CairoResource::CairoResource(CairoGraphicsFactory* factory)
    : factory_(factory) {}

CairoResource::~CairoResource() {}

String CairoResource::GetPlatformId() const { return kCairoGraphicsPlatformId; }

IGraphicsFactory* CairoResource::GetGraphicsFactory() { return factory_; }
}  // namespace cru::platform::graphics::cairo
