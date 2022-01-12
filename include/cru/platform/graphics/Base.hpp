#pragma once
#include "../Color.hpp"
#include "../GraphicsBase.hpp"
#include "../Matrix.hpp"
#include "../Resource.hpp"

#include <memory>

#ifdef CRU_PLATFORM_WINDOWS
#ifdef CRU_PLATFORM_GRAPHICS_EXPORT_API
#define CRU_PLATFORM_GRAPHICS_API __declspec(dllexport)
#else
#define CRU_PLATFORM_GRAPHICS_API __declspec(dllimport)
#endif
#else
#define CRU_PLATFORM_GRAPHICS_API
#endif


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
