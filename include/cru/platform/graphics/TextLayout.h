#pragma once
#include "Base.h"

#include <string>
#include <vector>

namespace cru::platform::graphics {
// Requirement:
// All text must be left-top aligned.
struct CRU_PLATFORM_GRAPHICS_API ITextLayout : virtual IGraphicsResource {
  virtual std::string GetText() = 0;
  virtual void SetText(std::string new_text) = 0;

  virtual std::shared_ptr<IFont> GetFont() = 0;
  virtual void SetFont(std::shared_ptr<IFont> font) = 0;

  virtual void SetMaxWidth(float max_width) = 0;
  virtual void SetMaxHeight(float max_height) = 0;

  virtual bool IsEditMode() = 0;
  virtual void SetEditMode(bool enable) = 0;

  /**
   * @brief Get the line index from the character index.
   * @param char_index The character index in code units.
   * @return The line index.
   * @throws Exception if char_index is out of range.
   */
  virtual Index GetLineIndexFromCharIndex(Index char_index) = 0;
  virtual Index GetLineCount() = 0;
  /**
   * @brief Get the height of a specific line.
   * @param line_index The index of the line.
   * @return The height of the line.
   * @throws Exception if line_index is out of range.
   */
  virtual float GetLineHeight(Index line_index) = 0;

  virtual Rect GetTextBounds(bool includingTrailingSpace = false) = 0;
  virtual std::vector<Rect> TextRangeRect(const TextRange& text_range) = 0;
  // Width is always 0, height is line height.
  virtual Rect TextSinglePoint(Index position, bool trailing) = 0;
  virtual TextHitTestResult HitTest(const Point& point) = 0;
};
}  // namespace cru::platform::graphics
