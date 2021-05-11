#include "cru/ui/render/ScrollRenderObject.hpp"

#include "cru/platform/graphics/Painter.hpp"
#include "cru/platform/graphics/util/Painter.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/controls/Control.hpp"
#include "cru/ui/render/ScrollBar.hpp"

#include <algorithm>
#include <memory>
#include <optional>

namespace cru::ui::render {
constexpr float kLineHeight = 16;

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

  coerce(result.x, max_scroll.x);
  coerce(result.y, max_scroll.y);

  return result;
}
}  // namespace

ScrollRenderObject::ScrollRenderObject() : RenderObject(ChildMode::Single) {
  scroll_bar_delegate_ = std::make_unique<ScrollBarDelegate>(this);
  scroll_bar_delegate_->ScrollAttemptEvent()->AddHandler(
      [this](const struct Scroll& scroll) { this->ApplyScroll(scroll); });
}

void ScrollRenderObject::ApplyScroll(const struct Scroll& scroll) {
  auto direction = scroll.direction;

  switch (scroll.kind) {
    case ScrollKind::Absolute:
      SetScrollOffset(direction, scroll.value);
      break;
    case ScrollKind::Relative:
      SetScrollOffset(direction,
                      GetScrollOffset(scroll.direction) + scroll.value);
      break;
    case ScrollKind::Page:
      SetScrollOffset(direction, GetScrollOffset(direction) +
                                     (direction == Direction::Horizontal
                                          ? GetViewRect().width
                                          : GetViewRect().height) *
                                         scroll.value);
      break;
    case ScrollKind::Line:
      SetScrollOffset(direction,
                      GetScrollOffset(direction) + kLineHeight * scroll.value);
      break;
    default:
      break;
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

void ScrollRenderObject::OnDrawCore(platform::graphics::IPainter* painter) {
  DefaultDrawContent(painter);
  if (const auto child = GetSingleChild()) {
    painter->PushLayer(this->GetContentRect());
    const auto offset = child->GetOffset();
    platform::graphics::util::WithTransform(
        painter, Matrix::Translation(offset.x, offset.y),
        [child](platform::graphics::IPainter* p) { child->Draw(p); });
    painter->PopLayer();
  }
  scroll_bar_delegate_->DrawScrollBar(painter);
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

void ScrollRenderObject::SetMouseWheelScrollEnabled(bool enable) {
  if (enable == is_mouse_wheel_enabled_) return;
  if (const auto control = GetAttachedControl()) {
    if (enable) {
      InstallMouseWheelHandler(control);
    } else {
      InstallMouseWheelHandler(nullptr);
    }
  }
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
    child->Layout(content_rect.GetLeftTop() - GetScrollOffset());
  }
}

void ScrollRenderObject::OnAttachedControlChanged(controls::Control* control) {
  if (control) {
    scroll_bar_delegate_->InstallHandlers(control);
    if (is_mouse_wheel_enabled_) {
      InstallMouseWheelHandler(control);
    }
  } else {
    InstallMouseWheelHandler(nullptr);
    scroll_bar_delegate_->UninstallHandlers();
  }
}

void ScrollRenderObject::InstallMouseWheelHandler(controls::Control* control) {
  guard_.Clear();

  if (control != nullptr) {
    guard_ += control->MouseWheelEvent()->Bubble()->PrependShortCircuitHandler(
        [this](event::MouseWheelEventArgs& args) {
          const auto delta = args.GetDelta();
          if (delta > 0) {
            if (VerticalCanScrollDown()) {
              ApplyScroll(
                  Scroll{Direction::Vertical, ScrollKind::Relative, delta});
              return true;
            } else if (HorizontalCanScrollDown()) {
              ApplyScroll(
                  Scroll{Direction::Horizontal, ScrollKind::Relative, delta});
              return true;
            }
          } else if (delta < 0) {
            if (VerticalCanScrollUp()) {
              ApplyScroll(
                  Scroll{Direction::Vertical, ScrollKind::Relative, delta});
              return true;
            } else if (HorizontalCanScrollUp()) {
              ApplyScroll(
                  Scroll{Direction::Horizontal, ScrollKind::Relative, delta});
              return true;
            }
          }
          return false;
        });
  }
}

bool ScrollRenderObject::HorizontalCanScrollUp() {
  return GetScrollOffset().x > 0.0;
}

bool ScrollRenderObject::HorizontalCanScrollDown() {
  return GetScrollOffset().x <
         GetFirstChild()->GetSize().width - GetViewRect().width;
}

bool ScrollRenderObject::VerticalCanScrollUp() {
  return GetScrollOffset().y > 0.0;
}

bool ScrollRenderObject::VerticalCanScrollDown() {
  return GetScrollOffset().y <
         GetFirstChild()->GetSize().height - GetViewRect().height;
}
}  // namespace cru::ui::render
