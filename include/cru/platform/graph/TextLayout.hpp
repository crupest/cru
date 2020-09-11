#pragma once
#include "Resource.hpp"

#include <string>
#include <vector>

namespace cru::platform::graph {
struct ITextLayout : virtual IGraphResource {
  virtual std::u16string GetText() = 0;
  virtual std::u16string_view GetTextView() = 0;
  virtual void SetText(std::u16string new_text) = 0;

  virtual std::shared_ptr<IFont> GetFont() = 0;
  virtual void SetFont(std::shared_ptr<IFont> font) = 0;

  virtual void SetMaxWidth(float max_width) = 0;
  virtual void SetMaxHeight(float max_height) = 0;

  virtual Rect GetTextBounds() = 0;
  virtual std::vector<Rect> TextRangeRect(const TextRange& text_range) = 0;
  virtual Point TextSinglePoint(Index position, bool trailing) = 0;
  virtual TextHitTestResult HitTest(const Point& point) = 0;
};
}  // namespace cru::platform::graph
