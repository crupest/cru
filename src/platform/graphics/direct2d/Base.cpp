#include "cru/platform/graphics/direct2d/Base.h"

#include "cru/platform/graphics/direct2d/Factory.h"

#include <cassert>

namespace cru::platform::graphics::direct2d {

DirectGraphicsResource::DirectGraphicsResource(DirectGraphicsFactory* factory)
    : factory_(factory) {
  assert(factory);
}

std::string DirectGraphicsResource::GetPlatformId() const {
  return kPlatformId;
}

IGraphicsFactory* DirectGraphicsResource::GetGraphicsFactory() {
  return factory_;
}
}  // namespace cru::platform::graphics::direct2d
