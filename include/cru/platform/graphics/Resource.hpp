#pragma once
#include "Base.hpp"

namespace cru::platform::graphics {
struct IGraphicsFactory;

struct IGraphicsResource : virtual IPlatformResource {
  virtual IGraphicsFactory* GetGraphicsFactory() = 0;
};
}  // namespace cru::platform::graphics
