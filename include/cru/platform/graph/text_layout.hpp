#pragma once
#include "resource.hpp"

#include <memory>
#include <string>
#include <vector>

namespace cru::platform::graph {
struct IFont;

struct TextHitTestResult {
  int position;
  bool trailing;
  bool insideText;
};

struct ITextLayout : virtual IGraphResource {
  virtual std::string GetText() = 0;
  virtual void SetText(std::string new_text) = 0;

  virtual std::shared_ptr<IFont> GetFont() = 0;
  virtual void SetFont(std::shared_ptr<IFont> font) = 0;

  virtual void SetMaxWidth(float max_width) = 0;
  virtual void SetMaxHeight(float max_height) = 0;

  virtual Rect GetTextBounds() = 0;
  virtual std::vector<Rect> TextRangeRect(const TextRange& text_range) = 0;
  virtual Point TextSingleRect(int position, bool trailing) = 0;
  virtual TextHitTestResult HitTest(const Point& point) = 0;
};
}  // namespace cru::platform::graph
