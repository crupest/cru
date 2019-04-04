#pragma once
#include "cru/common/base.hpp"

#include "cru/common/ui_base.hpp"

#include <memory>
#include <string>
#include <string_view>

namespace cru::platform {
struct SolidColorBrush;
struct GeometryBuilder;
struct FontDescriptor;
struct TextLayout;

struct GraphFactory : virtual Interface {
  virtual SolidColorBrush* CreateSolidColorBrush(const ui::Color& color) = 0;
  virtual GeometryBuilder* CreateGeometryBuilder() = 0;
  virtual FontDescriptor* CreateFontDescriptor(
      const std::wstring_view& font_family, float font_size) = 0;
  virtual TextLayout* CreateTextLayout(std::shared_ptr<FontDescriptor> font,
                                       std::wstring text) = 0;
};
}  // namespace cru::platform
