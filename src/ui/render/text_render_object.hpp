#pragma once
#include "pre.hpp"

#include "system_headers.hpp"

#include "render_object.hpp"

namespace cru::ui::render {
class TextRenderObject : public RenderObject {
 public:
  TextRenderObject(Microsoft::WRL::ComPtr<ID2D1Brush> brush,
                   Microsoft::WRL::ComPtr<IDWriteTextFormat> format,
                   Microsoft::WRL::ComPtr<ID2D1Brush> selection_brush);
  TextRenderObject(const TextRenderObject& other) = delete;
  TextRenderObject(TextRenderObject&& other) = delete;
  TextRenderObject& operator=(const TextRenderObject& other) = delete;
  TextRenderObject& operator=(TextRenderObject&& other) = delete;
  ~TextRenderObject() override = default;

  String GetText() const { return text_; }
  void SetText(String new_text) { text_ = std::move(new_text); }

  Microsoft::WRL::ComPtr<ID2D1Brush> GetBrush() const { return brush_; }
  void SetBrush(Microsoft::WRL::ComPtr<ID2D1Brush> new_brush) {
    brush_ = std::move(new_brush);
  }

  Microsoft::WRL::ComPtr<IDWriteTextFormat> GetTextFormat() const {
    return text_format_;
  }
  void SetTextFormat(
      Microsoft::WRL::ComPtr<IDWriteTextFormat> new_text_format) {
    text_format_ = std::move(new_text_format);
  }

  std::optional<TextRange> GetSelectionRange() const {
    return selection_range_;
  }
  void SetSelectionRange(std::optional<TextRange> new_range) {
    selection_range_ = std::move(new_range);
  }

  Microsoft::WRL::ComPtr<ID2D1Brush> GetSelectionBrush() const {
    return selection_brush_;
  }
  void SetSelectionBrush(Microsoft::WRL::ComPtr<ID2D1Brush> new_brush) {
    selection_brush_ = std::move(new_brush);
  }

  void Draw(ID2D1RenderTarget* render_target) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  void RecreateTextLayout();

 private:
  String text_;

  Microsoft::WRL::ComPtr<ID2D1Brush> brush_;
  Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format_;
  Microsoft::WRL::ComPtr<IDWriteTextLayout> text_layout_;

  std::optional<TextRange> selection_range_ = std::nullopt;
  Microsoft::WRL::ComPtr<ID2D1Brush> selection_brush_;
};
}  // namespace cru::ui::render
