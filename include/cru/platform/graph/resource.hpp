#pragma once
#include "Base.hpp"

namespace cru::platform::graph {
struct IGraphFactory;

struct IGraphResource : virtual INativeResource {
  virtual IGraphFactory* GetGraphFactory() = 0;
};
}  // namespace cru::platform::graph
