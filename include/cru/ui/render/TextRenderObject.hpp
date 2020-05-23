#pragma once
#include "RenderObject.hpp"

#include <string>

namespace cru::ui::render {
class TextRenderObject : public RenderObject {
 public:
  constexpr static float default_caret_width = 2;

 public:
  TextRenderObject(std::shared_ptr<platform::graph::IBrush> brush,
                   std::shared_ptr<platform::graph::IFont> font,
                   std::shared_ptr<platform::graph::IBrush> selection_brush,
                   std::shared_ptr<platform::graph::IBrush> caret_brush);
  TextRenderObject(const TextRenderObject& other) = delete;
  TextRenderObject(TextRenderObject&& other) = delete;
  TextRenderObject& operator=(const TextRenderObject& other) = delete;
  TextRenderObject& operator=(TextRenderObject&& other) = delete;
  ~TextRenderObject() override;

  std::string GetText() const;
  void SetText(std::string new_text);

  std::shared_ptr<platform::graph::IBrush> GetBrush() const { return brush_; }
  void SetBrush(std::shared_ptr<platform::graph::IBrush> new_brush);

  std::shared_ptr<platform::graph::IFont> GetFont() const;
  void SetFont(std::shared_ptr<platform::graph::IFont> font);

  std::vector<Rect> TextRangeRect(const TextRange& text_range);
  Point TextSinglePoint(gsl::index position, bool trailing);
  platform::graph::TextHitTestResult TextHitTest(const Point& point);

  std::optional<TextRange> GetSelectionRange() const {
    return selection_range_;
  }
  void SetSelectionRange(std::optional<TextRange> new_range);

  std::shared_ptr<platform::graph::IBrush> GetSelectionBrush() const {
    return selection_brush_;
  }
  void SetSelectionBrush(std::shared_ptr<platform::graph::IBrush> new_brush);

  bool IsDrawCaret() const { return draw_caret_; }
  void SetDrawCaret(bool draw_caret);
  void ToggleDrawCaret() { SetDrawCaret(!IsDrawCaret()); }

  // Caret position can be any value. When it is negative, 0 is used. When it
  // exceeds the size of the string, the size of the string is used.
  gsl::index GetCaretPosition() const { return caret_position_; }
  void SetCaretPosition(gsl::index position);

  std::shared_ptr<platform::graph::IBrush> GetCaretBrush() const {
    return caret_brush_;
  }
  void GetCaretBrush(std::shared_ptr<platform::graph::IBrush> brush);

  float GetCaretWidth() const { return caret_width_; }
  void SetCaretWidth(float width);

  void Draw(platform::graph::IPainter* painter) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

  void OnAfterLayout() override;

 private:
  std::shared_ptr<platform::graph::IBrush> brush_;
  std::shared_ptr<platform::graph::IFont> font_;
  std::unique_ptr<platform::graph::ITextLayout> text_layout_;

  std::optional<TextRange> selection_range_ = std::nullopt;
  std::shared_ptr<platform::graph::IBrush> selection_brush_;

  bool draw_caret_ = false;
  gsl::index caret_position_ = 0;
  std::shared_ptr<platform::graph::IBrush> caret_brush_;
  float caret_width_ = default_caret_width;
};
}  // namespace cru::ui::render