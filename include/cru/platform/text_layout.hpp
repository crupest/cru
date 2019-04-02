#pragma once
#include "cru/common/base.hpp"

#include "cru/common/ui_base.hpp"

#include <vector>

namespace cru::platform {
struct TextLayout : virtual Interface {
  virtual void SetMaxWidth(float max_width) = 0;
  virtual void SetMaxHeight(float max_height) = 0;
  virtual ui::Rect GetTextBounds() = 0;
  virtual std::vector<ui::Rect> TextRangeRect(const ui::TextRange& text_range) = 0;
};
}  // namespace cru::platform
