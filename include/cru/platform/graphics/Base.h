#pragma once
#include "../Color.h"
#include "../GraphicsBase.h"
#include "../Matrix.h"
#include "../Resource.h"

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
struct IImage;
struct IImageFactory;
struct IPainter;
struct ITextLayout;

struct TextHitTestResult {
  gsl::index position;
  bool trailing;
  bool inside_text;
};
}  // namespace cru::platform::graphics
