#pragma once
#include "render_object.hpp"

#include <memory>
#include <string>

// forward declarations
namespace cru::platform {
struct Brush;
struct FontDescriptor;
struct TextLayout;
}  // namespace cru::platform

namespace cru::ui::render {
class TextRenderObject : public RenderObject {
 public:
  TextRenderObject(std::shared_ptr<platform::Brush> brush,
                   std::shared_ptr<platform::FontDescriptor> font,
                   std::shared_ptr<platform::Brush> selection_brush);
  TextRenderObject(const TextRenderObject& other) = delete;
  TextRenderObject(TextRenderObject&& other) = delete;
  TextRenderObject& operator=(const TextRenderObject& other) = delete;
  TextRenderObject& operator=(TextRenderObject&& other) = delete;
  ~TextRenderObject() override;

  std::wstring GetText() const;
  void SetText(std::wstring new_text);

  std::shared_ptr<platform::Brush> GetBrush() const { return brush_; }
  void SetBrush(std::shared_ptr<platform::Brush> new_brush) {
    new_brush.swap(brush_);
  }

  std::shared_ptr<platform::FontDescriptor> GetFont() const;
  void SetFont(std::shared_ptr<platform::FontDescriptor> font);

  std::optional<TextRange> GetSelectionRange() const {
    return selection_range_;
  }
  void SetSelectionRange(std::optional<TextRange> new_range) {
    selection_range_ = std::move(new_range);
  }

  std::shared_ptr<platform::Brush> GetSelectionBrush() const {
    return selection_brush_;
  }
  void SetSelectionBrush(std::shared_ptr<platform::Brush> new_brush) {
    new_brush.swap(selection_brush_);
  }

  void Draw(platform::Painter* painter) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  void OnSizeChanged(const Size& old_size, const Size& new_size) override;

  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  std::shared_ptr<platform::Brush> brush_;
  std::shared_ptr<platform::FontDescriptor> font_;
  std::shared_ptr<platform::TextLayout> text_layout_;

  std::optional<TextRange> selection_range_ = std::nullopt;
  std::shared_ptr<platform::Brush>  selection_brush_;
};
}  // namespace cru::ui::render
