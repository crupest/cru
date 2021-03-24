#pragma once
#include "Resource.hpp"

namespace cru::platform::graphics {
struct IFont : virtual IGraphResource {
  virtual float GetFontSize() = 0;
};
}  // namespace cru::platform::graph
