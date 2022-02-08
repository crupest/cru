#include "cru/win/graphics/direct/Resource.h"

#include "cru/win/graphics/direct/Factory.h"

namespace cru::platform::graphics::win::direct {
String DirectResource::kPlatformId = u"Windows Direct";

DirectGraphicsResource::DirectGraphicsResource(DirectGraphicsFactory* factory)
    : factory_(factory) {
  Expects(factory);
}

IGraphicsFactory* DirectGraphicsResource::GetGraphicsFactory() {
  return factory_;
}
}  // namespace cru::platform::graphics::win::direct
