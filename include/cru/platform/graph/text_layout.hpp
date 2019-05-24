#pragma once
#include "cru/common/base.hpp"

#include "cru/common/ui_base.hpp"

#include <memory>
#include <string>
#include <vector>

namespace cru::platform::graph {
struct IFontDescriptor;

struct ITextLayout : virtual Interface {
  virtual std::wstring GetText() = 0;
  virtual void SetText(std::wstring new_text) = 0;
  virtual std::shared_ptr<IFontDescriptor> GetFont() = 0;
  virtual void SetFont(std::shared_ptr<IFontDescriptor> font) = 0;
  virtual void SetMaxWidth(float max_width) = 0;
  virtual void SetMaxHeight(float max_height) = 0;
  virtual ui::Rect GetTextBounds() = 0;
  virtual std::vector<ui::Rect> TextRangeRect(
      const ui::TextRange& text_range) = 0;
};
}  // namespace cru::platform
