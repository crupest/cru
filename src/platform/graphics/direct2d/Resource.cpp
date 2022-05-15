#include "cru/platform/graphics/direct2d/Resource.h"

#include "cru/platform/graphics/direct2d/Factory.h"

namespace cru::platform::graphics::direct2d {
String DirectResource::kPlatformId = u"Windows Direct";

DirectGraphicsResource::DirectGraphicsResource(DirectGraphicsFactory* factory)
    : factory_(factory) {
  Expects(factory);
}

IGraphicsFactory* DirectGraphicsResource::GetGraphicsFactory() {
  return factory_;
}
}  // namespace cru::platform::graphics::direct2d
