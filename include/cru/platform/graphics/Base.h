#pragma once
#include <cru/platform/Base.h>          // IWYU pragma: export
#include <cru/platform/Color.h>         // IWYU pragma: export
#include <cru/platform/GraphicsBase.h>  // IWYU pragma: export
#include <cru/platform/Matrix.h>        // IWYU pragma: export

#ifdef CRU_IS_DLL
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
  Index position;
  bool trailing;
  bool inside_text;
};
}  // namespace cru::platform::graphics
