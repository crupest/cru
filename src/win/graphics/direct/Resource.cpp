#include "cru/win/graphics/direct/Resource.hpp"

#include "cru/win/graphics/direct/Factory.hpp"

namespace cru::platform::graphics::win::direct {
DirectGraphResource::DirectGraphResource(DirectGraphFactory* factory)
    : factory_(factory) {
  Expects(factory);
}

IGraphFactory* DirectGraphResource::GetGraphFactory() { return factory_; }
}  // namespace cru::platform::graphics::win::direct
