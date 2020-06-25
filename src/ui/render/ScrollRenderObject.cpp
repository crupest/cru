#include "cru/ui/render/ScrollRenderObject.hpp"

#include "cru/platform/graph/Painter.hpp"
#include "cru/platform/graph/util/Painter.hpp"

#include <algorithm>

namespace cru::ui::render {
namespace {
// This method assumes margin offset is already considered.
// It promises that it won't return negetive value.
Point CoerceScroll(const Point& scroll_offset, const Size& content_size,
                   const Size& child_size) {
  Point result(scroll_offset);

  Point max_scroll(child_size - content_size);
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

  return result;
}
}  // namespace

void ScrollRenderObject::Draw(platform::graph::IPainter* painter) {
  if (const auto child = GetSingleChild()) {
    painter->PushLayer(this->GetPaddingRect());
    const auto offset = child->GetOffset();
    platform::graph::util::WithTransform(
        painter, Matrix::Translation(offset.x, offset.y),
        [child](platform::graph::IPainter* p) { child->Draw(p); });
    painter->PopLayer();
  }
}

RenderObject* ScrollRenderObject::HitTest(const Point& point) {
  if (const auto child = GetSingleChild()) {
    const auto offset = child->GetOffset();
    const auto r = child->HitTest(point - offset);
    if (r != nullptr) return r;
  }

  const auto rect = GetPaddingRect();
  return rect.IsPointInside(point) ? this : nullptr;
}  // namespace cru::ui::render

Point ScrollRenderObject::GetScrollOffset() {
  if (const auto child = GetSingleChild())
    return CoerceScroll(scroll_offset_, GetContentRect().GetSize(),
                        child->GetSize());
  else
    return scroll_offset_;
}

void ScrollRenderObject::SetScrollOffset(const Point& offset) {
  scroll_offset_ = offset;
  InvalidateLayout();
}

Size ScrollRenderObject::OnMeasureContent(const MeasureRequirement& requirement,
                                          const MeasureSize& preferred_size) {
  // TODO: Rewrite this.
  CRU_UNUSED(requirement);
  CRU_UNUSED(preferred_size);
  throw std::runtime_error("Not implemented.");

  // if (const auto child = GetSingleChild()) {
  //   child->Measure(MeasureRequirement::Infinate());
  //   const auto preferred_size = child->GetMeasuredSize();
  //   return Min(preferred_size, requirement.GetMaxSize());
  // } else {
  //   return Size{};
  // }
}  // namespace cru::ui::render

void ScrollRenderObject::OnLayoutContent(const Rect& content_rect) {
  // TODO: Rewrite this.
  CRU_UNUSED(content_rect);
  throw std::runtime_error("Not implemented.");

  // if (const auto child = GetSingleChild()) {
  //   const auto child_size = child->GetMeasuredSize();
  //   const auto true_scroll =
  //       CoerceScroll(scroll_offset_, content_rect.GetSize(), child_size);
  //   child->Layout(Rect{content_rect.GetLeftTop() - true_scroll, child_size});
  // }
}
}  // namespace cru::ui::render
