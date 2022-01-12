#pragma once
#include "Resource.hpp"

#include <string>
#include <vector>

namespace cru::platform::graphics {
// Requirement:
// All text must be left-top aligned.
struct CRU_PLATFORM_GRAPHICS_API ITextLayout : virtual IGraphicsResource {
  virtual String GetText() = 0;
  virtual void SetText(String new_text) = 0;

  virtual std::shared_ptr<IFont> GetFont() = 0;
  virtual void SetFont(std::shared_ptr<IFont> font) = 0;

  virtual void SetMaxWidth(float max_width) = 0;
  virtual void SetMaxHeight(float max_height) = 0;

  virtual bool IsEditMode() = 0;
  virtual void SetEditMode(bool enable) = 0;

  virtual Index GetLineIndexFromCharIndex(Index char_index) = 0;
  virtual Index GetLineCount() = 0;
  virtual float GetLineHeight(Index line_index) = 0;

  virtual Rect GetTextBounds(bool includingTrailingSpace = false) = 0;
  virtual std::vector<Rect> TextRangeRect(const TextRange& text_range) = 0;
  // Width is always 0, height is line height.
  virtual Rect TextSinglePoint(Index position, bool trailing) = 0;
  virtual TextHitTestResult HitTest(const Point& point) = 0;
};
}  // namespace cru::platform::graphics
