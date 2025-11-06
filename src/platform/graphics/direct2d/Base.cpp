#include "cru/platform/graphics/direct2d/Base.h"

#include "cru/platform/graphics/direct2d/Factory.h"

namespace cru::platform::graphics::direct2d {

DirectGraphicsResource::DirectGraphicsResource(DirectGraphicsFactory* factory)
    : factory_(factory) {
  Expects(factory);
}

std::string DirectGraphicsResource::GetPlatformId() const {
  return kPlatformId;
}

IGraphicsFactory* DirectGraphicsResource::GetGraphicsFactory() {
  return factory_;
}
}  // namespace cru::platform::graphics::direct2d
