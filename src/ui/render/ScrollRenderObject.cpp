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

RenderObject* ScrollRenderObject::HitTest(const Point& point) {
  if (const auto child = GetSingleChild()) {
    const auto offset = child->GetOffset();
    const auto r = child->HitTest(point - offset);
    if (r != nullptr) return r;
  }

  const auto rect = GetPaddingRect();
  return rect.IsPointInside(point) ? this : nullptr;
}  // namespace cru::ui::render

void ScrollRenderObject::OnDrawCore(platform::graph::IPainter* painter) {
  DefaultDrawContent(painter);
  if (const auto child = GetSingleChild()) {
    painter->PushLayer(this->GetPaddingRect());
    const auto offset = child->GetOffset();
    platform::graph::util::WithTransform(
        painter, Matrix::Translation(offset.x, offset.y),
        [child](platform::graph::IPainter* p) { child->Draw(p); });
    painter->PopLayer();
  }
}

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

void ScrollRenderObject::SetScrollOffset(std::optional<float> x,
                                         std::optional<float> y) {
  bool dirty = false;

  if (x.has_value()) {
    dirty = true;
    scroll_offset_.x = *x;
  }

  if (y.has_value()) {
    dirty = true;
    scroll_offset_.y = *y;
  }

  if (dirty) InvalidateLayout();
}

void ScrollRenderObject::ScrollToContain(const Rect& rect,
                                         const Thickness& margin) {
  std::optional<float> new_scroll_x;
  std::optional<float> new_scroll_y;

  Rect real_rect = rect.Expand(margin);

  Rect view_rect = GetViewRect();

  // horizontal
  if (real_rect.left < view_rect.left) {
    new_scroll_x = real_rect.left;
  } else if (real_rect.GetRight() > view_rect.GetRight()) {
    new_scroll_x = real_rect.GetRight() - view_rect.width;
  }

  // vertical
  if (real_rect.top < view_rect.top) {
    new_scroll_y = real_rect.top;
  } else if (real_rect.GetBottom() > view_rect.GetBottom()) {
    new_scroll_y = real_rect.GetBottom() - view_rect.height;
  }

  SetScrollOffset(new_scroll_x, new_scroll_y);
}

Size ScrollRenderObject::OnMeasureContent(const MeasureRequirement& requirement,
                                          const MeasureSize& preferred_size) {
  if (const auto child = GetSingleChild()) {
    child->Measure(MeasureRequirement{MeasureSize::NotSpecified(),
                                      MeasureSize::NotSpecified()},
                   MeasureSize::NotSpecified());

    Size result = requirement.Coerce(child->GetSize());
    if (preferred_size.width.IsSpecified()) {
      result.width = preferred_size.width.GetLengthOrUndefined();
    }
    if (preferred_size.height.IsSpecified()) {
      result.height = preferred_size.height.GetLengthOrUndefined();
    }
    return result;
  } else {
    Size result{preferred_size.width.IsSpecified()
                    ? preferred_size.width.GetLengthOrUndefined()
                    : requirement.min.width.GetLengthOr0(),
                preferred_size.height.IsSpecified()
                    ? preferred_size.height.GetLengthOrUndefined()
                    : requirement.min.height.GetLengthOr0()};
    return result;
  }
}  // namespace cru::ui::render

void ScrollRenderObject::OnLayoutContent(const Rect& content_rect) {
  if (const auto child = GetSingleChild()) {
    const auto child_size = child->GetSize();
    child->Layout(content_rect.GetLeftTop() - GetScrollOffset());
  }
}
}  // namespace cru::ui::render
