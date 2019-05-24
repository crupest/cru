#pragma once
#include "render_object.hpp"

#include <memory>
#include <string>

// forward declarations
namespace cru::platform::graph {
struct IBrush;
struct IFontDescriptor;
struct ITextLayout;
}  // namespace cru::platform::graph

namespace cru::ui::render {
class TextRenderObject : public RenderObject {
 public:
  TextRenderObject(std::shared_ptr<platform::graph::IBrush> brush,
                   std::shared_ptr<platform::graph::IFontDescriptor> font,
                   std::shared_ptr<platform::graph::IBrush> selection_brush);
  TextRenderObject(const TextRenderObject& other) = delete;
  TextRenderObject(TextRenderObject&& other) = delete;
  TextRenderObject& operator=(const TextRenderObject& other) = delete;
  TextRenderObject& operator=(TextRenderObject&& other) = delete;
  ~TextRenderObject() override = default;

  std::wstring GetText() const;
  void SetText(std::wstring new_text);

  std::shared_ptr<platform::graph::IBrush> GetBrush() const { return brush_; }
  void SetBrush(std::shared_ptr<platform::graph::IBrush> new_brush) {
    new_brush.swap(brush_);
  }

  std::shared_ptr<platform::graph::IFontDescriptor> GetFont() const;
  void SetFont(std::shared_ptr<platform::graph::IFontDescriptor> font);

  std::optional<TextRange> GetSelectionRange() const {
    return selection_range_;
  }
  void SetSelectionRange(std::optional<TextRange> new_range) {
    selection_range_ = std::move(new_range);
  }

  std::shared_ptr<platform::graph::IBrush> GetSelectionBrush() const {
    return selection_brush_;
  }
  void SetSelectionBrush(std::shared_ptr<platform::graph::IBrush> new_brush) {
    new_brush.swap(selection_brush_);
  }

  void Draw(platform::graph::IPainter* painter) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  void OnSizeChanged(const Size& old_size, const Size& new_size) override;

  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  std::shared_ptr<platform::graph::IBrush> brush_;
  std::shared_ptr<platform::graph::IFontDescriptor> font_;
  std::shared_ptr<platform::graph::ITextLayout> text_layout_;

  std::optional<TextRange> selection_range_ = std::nullopt;
  std::shared_ptr<platform::graph::IBrush> selection_brush_;
};
}  // namespace cru::ui::render
