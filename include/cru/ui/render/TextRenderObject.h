#pragma once
#include "RenderObject.h"

#include <memory>
#include <string>

namespace cru::ui::render {
// Layout logic:
// 1. If preferred width is set then it is taken to do a text measure. If not
// set, then max width is taken to do that.
// 2. If the actual width of text after measure exceeds the required width,
// coerced value is returned and an error is reported. If preferred width is set
// and actual width is smaller than that, then preferred width is used. If
// preferred width is not set, then the same thing is applied to min width.
// 3. If actual height of text is bigger than max height, an error is reported
// and coerced value is returned. Or height is lifted up to be at least
// preferred size if set or min height.
//
// If the result layout box is bigger than actual text box, then text is center
// aligned.
class CRU_UI_API TextRenderObject : public RenderObject {
 private:
  constexpr static auto kLogTag = "cru::ui::render::TextRenderObject";

 public:
  static constexpr auto kRenderObjectName = "TextRenderObject";
  constexpr static float default_caret_width = 2;

 public:
  TextRenderObject(std::shared_ptr<platform::graphics::IBrush> brush,
                   std::shared_ptr<platform::graphics::IFont> font,
                   std::shared_ptr<platform::graphics::IBrush> selection_brush,
                   std::shared_ptr<platform::graphics::IBrush> caret_brush);

  std::string GetText();
  void SetText(std::string new_text);

  std::shared_ptr<platform::graphics::IBrush> GetBrush() { return brush_; }
  void SetBrush(std::shared_ptr<platform::graphics::IBrush> new_brush);

  std::shared_ptr<platform::graphics::IFont> GetFont();
  void SetFont(std::shared_ptr<platform::graphics::IFont> font);

  bool IsEditMode();
  void SetEditMode(bool enable);

  Index GetLineCount();
  Index GetLineIndexFromCharIndex(Index char_index);
  float GetLineHeight(Index line_index);
  std::vector<Rect> TextRangeRect(const TextRange& text_range);
  Rect TextSinglePoint(Index position, bool trailing);
  platform::graphics::TextHitTestResult TextHitTest(const Point& point);

  std::optional<TextRange> GetSelectionRange() { return selection_range_; }
  void SetSelectionRange(std::optional<TextRange> new_range);

  std::shared_ptr<platform::graphics::IBrush> GetSelectionBrush() {
    return selection_brush_;
  }
  void SetSelectionBrush(std::shared_ptr<platform::graphics::IBrush> new_brush);

  bool IsDrawCaret() { return draw_caret_; }
  void SetDrawCaret(bool draw_caret);
  void ToggleDrawCaret() { SetDrawCaret(!IsDrawCaret()); }

  // Caret position can be any value. When it is negative, 0 is used. When it
  // exceeds the size of the string, the size of the string is used.
  Index GetCaretPosition() { return caret_position_; }
  void SetCaretPosition(Index position);

  // Lefttop relative to content lefttop.
  Rect GetCaretRectInContent();
  // Lefttop relative to render object lefttop.
  Rect GetCaretRect();

  std::shared_ptr<platform::graphics::IBrush> GetCaretBrush() {
    return caret_brush_;
  }
  void GetCaretBrush(std::shared_ptr<platform::graphics::IBrush> brush);

  float GetCaretWidth() { return caret_width_; }
  void SetCaretWidth(float width);

  bool IsMeasureIncludingTrailingSpace() {
    return is_measure_including_trailing_space_;
  }
  void SetMeasureIncludingTrailingSpace(bool including);

  RenderObject* HitTest(const Point& point) override;

 protected:
  // See remarks of this class.
  Size OnMeasureContent(const MeasureRequirement& requirement) override;
  void OnLayoutContent(const Rect& content_rect) override;
  void OnDraw(RenderObjectDrawContext& context) override;

 private:
  std::shared_ptr<platform::graphics::IBrush> brush_;
  std::shared_ptr<platform::graphics::IFont> font_;
  std::unique_ptr<platform::graphics::ITextLayout> text_layout_;

  std::optional<TextRange> selection_range_ = std::nullopt;
  std::shared_ptr<platform::graphics::IBrush> selection_brush_;

  bool draw_caret_ = false;
  Index caret_position_ = 0;
  std::shared_ptr<platform::graphics::IBrush> caret_brush_;
  float caret_width_ = default_caret_width;

  bool is_measure_including_trailing_space_ = false;
};
}  // namespace cru::ui::render
