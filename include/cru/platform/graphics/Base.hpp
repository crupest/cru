#pragma once
#include "../Color.hpp"
#include "../GraphicsBase.hpp"
#include "../Matrix.hpp"
#include "../Resource.hpp"

#include <memory>

namespace cru::platform::graphics {
// forward declarations
struct IGraphicsFactory;
struct IBrush;
struct ISolidColorBrush;
struct IFont;
struct IGeometry;
struct IGeometryBuilder;
struct IPainter;
struct ITextLayout;

struct TextHitTestResult {
  gsl::index position;
  bool trailing;
  bool inside_text;
};
}  // namespace cru::platform::graphics
