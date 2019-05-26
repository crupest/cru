#pragma once
#include "cru/common/base.hpp"

#include "cru/common/auto_delete.hpp"
#include "cru/common/ui_base.hpp"

#include "brush.hpp"
#include "font.hpp"
#include "geometry.hpp"
#include "text_layout.hpp"

#include <memory>
#include <string>
#include <string_view>

namespace cru::platform::graph {
// Entry point of the graph module.
// If you create a IUiApplication instance, then you should not create
// IGraphFactory manually. IUiApplication will call
// IGraphFactory::CreateInstance and set auto-delete to true.
// The manual creation method of IGraphFactory provides a you a way to use graph
// related tools without interact with actual ui like window system.
struct IGraphFactory : virtual Interface, virtual IAutoDelete {
  // Create a platform-specific instance and save it as the global instance.
  // Do not create the instance twice. Implements should assert for that.
  // After creating, get the instance by GetInstance.
  static IGraphFactory* CreateInstance();

  // Get the global instance. If it is not created, then return nullptr.
  static IGraphFactory* GetInstance();

  virtual ISolidColorBrush* CreateSolidColorBrush(const ui::Color& color) = 0;
  virtual IGeometryBuilder* CreateGeometryBuilder() = 0;
  virtual IFontDescriptor* CreateFontDescriptor(
      const std::wstring_view& font_family, float font_size) = 0;
  virtual ITextLayout* CreateTextLayout(std::shared_ptr<IFontDescriptor> font,
                                        std::wstring text) = 0;
};
}  // namespace cru::platform::graph
