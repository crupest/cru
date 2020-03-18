#pragma once
#include "../graph_base.hpp"
#include "../matrix.hpp"
#include "../resource.hpp"

#include <memory>

namespace cru::platform::graph {
// forward declarations
struct IGraphFactory;
struct IBrush;
struct ISolidColorBrush;
struct IFont;
struct IGeometry;
struct IGeometryBuilder;
struct IPainter;
struct ITextLayout;

struct TextHitTestResult {
  int position;
  bool trailing;
  bool insideText;
};
}  // namespace cru::platform::graph
