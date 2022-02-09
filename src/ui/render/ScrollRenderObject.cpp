#include "cru/ui/render/ScrollRenderObject.h"

#include "cru/platform/graphics/Painter.h"
#include "cru/platform/graphics/util/Painter.h"
#include "cru/ui/Base.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/render/ScrollBar.h"

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

ScrollRenderObject::ScrollRenderObject() : RenderObject() {
  scroll_bar_delegate_ = std::make_unique<ScrollBarDelegate>(this);
  scroll_bar_delegate_->ScrollAttemptEvent()->AddHandler(
      [this](const struct Scroll& scroll) { this->ApplyScroll(scroll); });
}

void ScrollRenderObject::SetChild(RenderObject* new_child) {
  if (child_ == new_child) return;
  if (child_) child_->SetParent(nullptr);
  child_ = new_child;
  if (child_) child_->SetParent(this);
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
  if (child_) {
    const auto offset = child_->GetOffset();
    const auto r = child_->HitTest(point - offset);
    if (r != nullptr) return r;
  }

  const auto rect = GetPaddingRect();
  return rect.IsPointInside(point) ? this : nullptr;
}  // namespace cru::ui::render

void ScrollRenderObject::Draw(platform::graphics::IPainter* painter) {
  if (child_) {
    painter->PushLayer(this->GetContentRect());
    const auto offset = child_->GetOffset();
    painter->PushState();
    painter->ConcatTransform(Matrix::Translation(offset));
    child_->Draw(painter);
    painter->PopState();
    painter->PopLayer();
  }
  scroll_bar_delegate_->DrawScrollBar(painter);
}

Point ScrollRenderObject::GetScrollOffset() {
  if (child_)
    return CoerceScroll(scroll_offset_, GetContentRect().GetSize(),
                        child_->GetDesiredSize());
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
  if (child_) {
    child_->Measure(MeasureRequirement{MeasureSize::NotSpecified(),
                                       MeasureSize::NotSpecified()},
                    MeasureSize::NotSpecified());

    Size result = requirement.Coerce(child_->GetDesiredSize());
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
  if (child_) {
    child_->Layout(content_rect.GetLeftTop() - GetScrollOffset());
  }
}

void ScrollRenderObject::OnAttachedControlChanged(
    controls::Control* old_control, controls::Control* new_control) {
  if (new_control) {
    scroll_bar_delegate_->InstallHandlers(new_control);
    if (is_mouse_wheel_enabled_) {
      InstallMouseWheelHandler(new_control);
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
        [this](events::MouseWheelEventArgs& args) {
          auto delta = args.GetDelta();

          delta *= 24;

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
         child_->GetDesiredSize().width - GetViewRect().width;
}

bool ScrollRenderObject::VerticalCanScrollUp() {
  return GetScrollOffset().y > 0.0;
}

bool ScrollRenderObject::VerticalCanScrollDown() {
  return GetScrollOffset().y <
         child_->GetDesiredSize().height - GetViewRect().height;
}
}  // namespace cru::ui::render
