#include "cru/ui/render/text_render_object.hpp"

#include "../helper.hpp"
#include "cru/platform/graph/factory.hpp"
#include "cru/platform/graph/text_layout.hpp"
#include "cru/platform/graph/util/painter.hpp"

#include <algorithm>
#include <cassert>

// TODO: Null Check!!!

namespace cru::ui::render {
TextRenderObject::TextRenderObject(
    std::shared_ptr<platform::graph::IBrush> brush,
    std::shared_ptr<platform::graph::IFont> font,
    std::shared_ptr<platform::graph::IBrush> selection_brush) {
  assert(brush);
  assert(font);
  assert(selection_brush);

  SetChildMode(ChildMode::None);

  brush.swap(brush_);
  font.swap(font_);
  selection_brush.swap(selection_brush_);

  const auto graph_factory = GetGraphFactory();
  text_layout_ = graph_factory->CreateTextLayout(font_, "");
}

std::string TextRenderObject::GetText() const {
  return text_layout_->GetText();
}

void TextRenderObject::SetText(std::string new_text) {
  text_layout_->SetText(std::move(new_text));
}

std::shared_ptr<platform::graph::IFont> TextRenderObject::GetFont() const {
  return text_layout_->GetFont();
}

void TextRenderObject::SetFont(std::shared_ptr<platform::graph::IFont> font) {
  text_layout_->SetFont(std::move(font));
}

void TextRenderObject::Draw(platform::graph::IPainter* painter) {
  platform::graph::util::WithTransform(
      painter,
      platform::Matrix::Translation(GetMargin().left + GetPadding().left,
                                    GetMargin().top + GetPadding().top),
      [this](platform::graph::IPainter* p) {
        if (this->selection_range_.has_value()) {
          const auto&& rects =
              text_layout_->TextRangeRect(this->selection_range_.value());
          for (const auto& rect : rects)
            p->FillRectangle(rect, this->GetSelectionBrush().get());
        }
        p->DrawText(Point{}, text_layout_.get(), brush_.get());
      });
}

RenderObject* TextRenderObject::HitTest(const Point& point) {
  const auto margin = GetMargin();
  const auto size = GetSize();
  return Rect{margin.left, margin.top,
              std::max(size.width - margin.GetHorizontalTotal(), 0.0f),
              std::max(size.height - margin.GetVerticalTotal(), 0.0f)}
                 .IsPointInside(point)
             ? this
             : nullptr;
}

Size TextRenderObject::OnMeasureContent(const Size& available_size) {
  text_layout_->SetMaxWidth(available_size.width);
  text_layout_->SetMaxHeight(available_size.height);
  return text_layout_->GetTextBounds().GetSize();
}

void TextRenderObject::OnLayoutContent(const Rect& content_rect) {
  CRU_UNUSED(content_rect)
}

void TextRenderObject::OnAfterLayout() {
  const auto&& size = GetContentRect().GetSize();
  text_layout_->SetMaxWidth(size.width);
  text_layout_->SetMaxHeight(size.height);
}

}  // namespace cru::ui::render
