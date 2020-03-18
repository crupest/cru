#pragma once
#include "../graph_base.hpp"
#include "../matrix.hpp"
#include "../resource.hpp"

#include "fwd.hpp"

#include <memory>

namespace cru::platform::graph {
struct TextHitTestResult {
  int position;
  bool trailing;
  bool insideText;
};
}  // namespace cru::platform::graph
