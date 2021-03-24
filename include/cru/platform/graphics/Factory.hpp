#pragma once
#include "Resource.hpp"

#include "Brush.hpp"
#include "Font.hpp"
#include "Geometry.hpp"
#include "TextLayout.hpp"

#include <string>
#include <string_view>

namespace cru::platform::graphics {
// Entry point of the graph module.
struct IGraphFactory : virtual INativeResource {
  virtual std::unique_ptr<ISolidColorBrush> CreateSolidColorBrush() = 0;

  virtual std::unique_ptr<IGeometryBuilder> CreateGeometryBuilder() = 0;

  virtual std::unique_ptr<IFont> CreateFont(std::u16string font_family,
                                            float font_size) = 0;

  virtual std::unique_ptr<ITextLayout> CreateTextLayout(
      std::shared_ptr<IFont> font, std::u16string text) = 0;

  std::unique_ptr<ISolidColorBrush> CreateSolidColorBrush(const Color& color) {
    std::unique_ptr<ISolidColorBrush> brush = CreateSolidColorBrush();
    brush->SetColor(color);
    return brush;
  }
};
}  // namespace cru::platform::graphics
