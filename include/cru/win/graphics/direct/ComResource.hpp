#pragma once
#include "Base.hpp"

#include "cru/common/Base.hpp"

namespace cru::platform::graphics::win::direct {
template <typename TInterface>
struct CRU_WIN_GRAPHICS_DIRECT_API IComResource : virtual Interface {
  virtual TInterface* GetComInterface() const = 0;
};
}  // namespace cru::platform::graphics::win::direct
