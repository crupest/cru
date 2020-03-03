#pragma once
#include "resource.hpp"

namespace cru::platform::graph {
struct IFont : virtual IGraphResource {
  virtual float GetFontSize() = 0;
};
}  // namespace cru::platform::graph
