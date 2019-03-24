#pragma once
#include "pre.hpp"

#include "render_object.hpp"

// forward declarations
struct ID2D1Brush;
struct IDWriteTextFormat;
struct IDWriteTextLayout;

namespace cru::ui::render {
class TextRenderObject : public RenderObject {
 public:
  TextRenderObject(ID2D1Brush* brush, IDWriteTextFormat* format,
                   ID2D1Brush* selection_brush);
  TextRenderObject(const TextRenderObject& other) = delete;
  TextRenderObject(TextRenderObject&& other) = delete;
  TextRenderObject& operator=(const TextRenderObject& other) = delete;
  TextRenderObject& operator=(TextRenderObject&& other) = delete;
  ~TextRenderObject() override;

  String GetText() const { return text_; }
  void SetText(String new_text) {
    text_ = std::move(new_text);
    RecreateTextLayout();
  }

  ID2D1Brush* GetBrush() const { return brush_; }
  void SetBrush(ID2D1Brush* new_brush);

  IDWriteTextFormat* GetTextFormat() const { return text_format_; }
  void SetTextFormat(IDWriteTextFormat* new_text_format);

  std::optional<TextRange> GetSelectionRange() const {
    return selection_range_;
  }
  void SetSelectionRange(std::optional<TextRange> new_range) {
    selection_range_ = std::move(new_range);
  }

  ID2D1Brush* GetSelectionBrush() const {
    return selection_brush_;
  }
  void SetSelectionBrush(ID2D1Brush* new_brush);

  void Draw(ID2D1RenderTarget* render_target) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  void OnSizeChanged(const Size& old_size, const Size& new_size) override;

  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  void RecreateTextLayout();

 private:
  String text_;

  ID2D1Brush* brush_ = nullptr;
  IDWriteTextFormat* text_format_ = nullptr;
  IDWriteTextLayout* text_layout_ = nullptr;

  std::optional<TextRange> selection_range_ = std::nullopt;
  ID2D1Brush* selection_brush_ = nullptr;
};
}  // namespace cru::ui::render
