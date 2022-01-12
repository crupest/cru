#pragma once
#include "Resource.hpp"

#include "Brush.hpp"
#include "Font.hpp"
#include "Geometry.hpp"
#include "TextLayout.hpp"

namespace cru::platform::graphics {
// Entry point of the graphics module.
struct CRU_PLATFORM_GRAPHICS_API IGraphicsFactory : virtual IPlatformResource {
  virtual std::unique_ptr<ISolidColorBrush> CreateSolidColorBrush() = 0;

  virtual std::unique_ptr<IGeometryBuilder> CreateGeometryBuilder() = 0;

  virtual std::unique_ptr<IFont> CreateFont(String font_family,
                                            float font_size) = 0;

  virtual std::unique_ptr<ITextLayout> CreateTextLayout(
      std::shared_ptr<IFont> font, String text) = 0;

  std::unique_ptr<ISolidColorBrush> CreateSolidColorBrush(const Color& color) {
    std::unique_ptr<ISolidColorBrush> brush = CreateSolidColorBrush();
    brush->SetColor(color);
    return brush;
  }
};
}  // namespace cru::platform::graphics
