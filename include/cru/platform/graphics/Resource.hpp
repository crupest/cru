#pragma once
#include "Base.hpp"

namespace cru::platform::graphics {
struct IGraphFactory;

struct IGraphResource : virtual IPlatformResource {
  virtual IGraphFactory* GetGraphFactory() = 0;
};
}  // namespace cru::platform::graphics
