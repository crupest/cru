#pragma once
#include "base.hpp"

#include "brush.hpp"
#include "font.hpp"
#include "geometry.hpp"
#include "text_layout.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace cru::platform::graph {
// Entry point of the graph module.
struct IGraphFactory : virtual INativeResource {
  virtual std::unique_ptr<ISolidColorBrush> CreateSolidColorBrush() = 0;

  virtual std::unique_ptr<IGeometryBuilder> CreateGeometryBuilder() = 0;

  virtual std::unique_ptr<IFont> CreateFont(const std::string_view& font_family,
                                            float font_size) = 0;

  virtual std::unique_ptr<ITextLayout> CreateTextLayout(
      std::shared_ptr<IFont> font, std::string text) = 0;
};
}  // namespace cru::platform::graph
