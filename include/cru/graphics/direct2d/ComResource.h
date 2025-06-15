#pragma once
#include "Base.h"

#include <cru/Base.h>

namespace cru::graphics::direct2d {
template <typename TInterface>
struct CRU_WIN_GRAPHICS_DIRECT_API IComResource : virtual Interface {
  virtual TInterface* GetComInterface() const = 0;
};
}  // namespace cru::graphics::direct2d
