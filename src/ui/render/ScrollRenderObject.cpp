#include "cru/ui/render/ScrollRenderObject.hpp"

#include "cru/platform/graph/Painter.hpp"
#include "cru/platform/graph/util/Painter.hpp"

#include <algorithm>

namespace cru::ui::render {
namespace {
// This method assumes margin offset is already considered.
// It promises that it won't return negetive value.
Point CalculateChildOffsetOfScroll(const Point& scroll_offset,
                                   const Size& content_area_size,
                                   const Thickness& padding,
                                   const Size& child_size) {
  Point result(scroll_offset);

  Point max_scroll(child_size - content_area_size);
  max_scroll.x = std::max(max_scroll.x, 0.f);
  max_scroll.y = std::max(max_scroll.y, 0.f);

  auto coerce = [](float& n, float max) {
    if (n < 0)
      n = 0;
    else if (n > max)
      n = max;
  };

  coerce(result.x, scroll_offset.x);
  coerce(result.y, scroll_offset.y);

  result.x += padding.left;
  result.y += padding.top;

  return result;
}
}  // namespace

void ScrollRenderObject::Draw(platform::graph::IPainter* painter) {
  for (auto child : this->GetChildren()) {
    painter->PushLayer(this->GetPaddingRect());
    const auto true_offset =
        CalculateChildOffsetOfScroll(scroll_offset_, GetContentRect().GetSize(),
                                     GetPadding(), child->GetSize());
    platform::graph::util::WithTransform(
        painter, Matrix::Translation(true_offset.x, true_offset.y),
        [child](platform::graph::IPainter* p) { child->Draw(p); });
    painter->PopLayer();
  }
}

RenderObject* ScrollRenderObject::HitTest(const Point& point) {
  const auto rect = GetPaddingRect();
  return rect.IsPointInside(point) ? this : nullptr;
}

void ScrollRenderObject::SetScrollOffset(const Point& offset) {
  scroll_offset_ = offset;
  InvalidatePaint();
}

Size ScrollRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement) {
  CRU_UNUSED(requirement);
  // TODO!
  // const auto& children = GetChildren();
  // if (children.empty()) return Size{};
  // const auto child = children.front();
  // child->Measure(MeasureRequirement::Infinate());
  // const auto preferred_size = child->GetMeasuredSize();
  return Size{};
}  // namespace cru::ui::render

void ScrollRenderObject::OnLayoutContent(const Rect& content_rect) {
  CRU_UNUSED(content_rect);
  // TODO!
}
}  // namespace cru::ui::render
