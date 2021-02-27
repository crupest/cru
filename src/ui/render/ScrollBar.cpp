#include "cru/ui/render/ScrollBar.hpp"

#include "../Helper.hpp"
#include "cru/common/Base.hpp"
#include "cru/platform/GraphBase.hpp"
#include "cru/platform/graphics/Factory.hpp"
#include "cru/platform/graphics/Painter.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/ui/Base.hpp"
#include "cru/ui/events/UiEvent.hpp"
#include "cru/ui/render/ScrollRenderObject.hpp"
#include "gsl/gsl_assert"

#include <algorithm>
#include <cassert>
#include <gsl/pointers>
#include <optional>
#include <stdexcept>

namespace cru::ui::render {
constexpr float kScrollBarCollapseThumbWidth = 2;
constexpr float kScrollBarCollapsedTriggerExpandAreaWidth = 5;
constexpr float kScrollBarExpandWidth = 10;

constexpr std::array<ScrollBarAreaKind, 5> kScrollBarAreaKindList{
    ScrollBarAreaKind::UpArrow, ScrollBarAreaKind::DownArrow,
    ScrollBarAreaKind::UpSlot, ScrollBarAreaKind::DownSlot,
    ScrollBarAreaKind::Thumb};

ScrollBar::ScrollBar(gsl::not_null<ScrollRenderObject*> render_object,
                     Direction direction)
    : render_object_(render_object), direction_(direction) {
  // TODO: Use theme resource and delete this.

  auto graphics_factory = GetUiApplication()->GetInstance()->GetGraphFactory();

  collapsed_thumb_brush_ =
      graphics_factory->CreateSolidColorBrush(colors::gray.WithAlpha(128));
  expanded_thumb_brush_ = graphics_factory->CreateSolidColorBrush(colors::gray);
  expanded_slot_brush_ =
      graphics_factory->CreateSolidColorBrush(colors::seashell);
  expanded_arrow_brush_ =
      graphics_factory->CreateSolidColorBrush(colors::white);
  expanded_arrow_background_brush_ =
      graphics_factory->CreateSolidColorBrush(colors::black);
}

void ScrollBar::SetEnabled(bool value) {
  if (value == is_enabled_) return;
  if (!value) {
    SetExpanded(false);
    if (move_thumb_start_) {
      if (const auto control = this->render_object_->GetAttachedControl()) {
        control->ReleaseMouse();
      }
      move_thumb_start_ = std::nullopt;
    }
  }
}

void ScrollBar::SetExpanded(bool value) {
  if (is_expanded_ == value) return;
  is_expanded_ = value;
  render_object_->InvalidatePaint();
}

void ScrollBar::Draw(platform::graphics::IPainter* painter) {
  if (is_enabled_) {
    OnDraw(painter, is_expanded_);
  }
}

void ScrollBar::InstallHandlers(controls::Control* control) {
  event_guard_.Clear();
  if (control != nullptr) {
    event_guard_ += control->MouseDownEvent()->Bubble()->AddHandler(
        [control, this](event::MouseButtonEventArgs& event) {
          if (event.GetButton() == mouse_buttons::left && IsEnabled() &&
              IsExpanded()) {
            auto hit_test_result =
                ExpandedHitTest(event.GetPoint(render_object_));
            if (!hit_test_result) return;

            switch (*hit_test_result) {
              case ScrollBarAreaKind::UpArrow:
                this->scroll_attempt_event_.Raise(
                    {GetDirection(), ScrollKind::Line, -1});
                event.SetHandled();
                break;
              case ScrollBarAreaKind::DownArrow:
                this->scroll_attempt_event_.Raise(
                    {GetDirection(), ScrollKind::Line, 1});
                event.SetHandled();
                break;
              case ScrollBarAreaKind::UpSlot:
                this->scroll_attempt_event_.Raise(
                    {GetDirection(), ScrollKind::Page, -1});
                event.SetHandled();
                break;
              case ScrollBarAreaKind::DownSlot:
                this->scroll_attempt_event_.Raise(
                    {GetDirection(), ScrollKind::Page, 1});
                event.SetHandled();
                break;
              case ScrollBarAreaKind::Thumb: {
                auto thumb_rect = GetExpandedAreaRect(ScrollBarAreaKind::Thumb);
                assert(thumb_rect);

                if (!control->CaptureMouse()) break;
                move_thumb_thumb_original_rect_ = *thumb_rect;
                move_thumb_start_ = event.GetPoint();
                event.SetHandled();
                break;
              }
              default:
                break;
            }
          }
        });

    event_guard_ += control->MouseUpEvent()->Bubble()->AddHandler(
        [control, this](event::MouseButtonEventArgs& event) {
          if (event.GetButton() == mouse_buttons::left && move_thumb_start_) {
            move_thumb_start_ = std::nullopt;
            control->ReleaseMouse();
            event.SetHandled();
          }
        });

    event_guard_ += control->MouseMoveEvent()->Bubble()->AddHandler(
        [this](event::MouseEventArgs& event) {
          if (move_thumb_start_) {
            auto new_scroll_position = CalculateNewScrollPosition(
                move_thumb_thumb_original_rect_,
                event.GetPoint() - *move_thumb_start_);

            this->scroll_attempt_event_.Raise(
                {GetDirection(), ScrollKind::Absolute, new_scroll_position});
            event.SetHandled();
            return;
          }

          if (IsEnabled() && !IsExpanded()) {
            auto trigger_expand_area = GetCollapsedTriggerExpandAreaRect();
            if (trigger_expand_area &&
                trigger_expand_area->IsPointInside(
                    event.GetPoint(this->render_object_)))
              SetExpanded(true);
            event.SetHandled();
          }
        });
  }
}

gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
ScrollBar::GetCollapsedThumbBrush() const {
  // TODO: Read theme resource.
  return collapsed_thumb_brush_;
}

gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
ScrollBar::GetExpandedThumbBrush() const {
  // TODO: Read theme resource.
  return expanded_thumb_brush_;
}

gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
ScrollBar::GetExpandedSlotBrush() const {
  // TODO: Read theme resource.
  return expanded_slot_brush_;
}

gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
ScrollBar::GetExpandedArrowBrush() const {
  // TODO: Read theme resource.
  return expanded_arrow_brush_;
}

gsl::not_null<std::shared_ptr<platform::graphics::IBrush>>
ScrollBar::GetExpandedArrowBackgroundBrush() const {
  // TODO: Read theme resource.
  return expanded_arrow_brush_;
}

void ScrollBar::OnDraw(platform::graphics::IPainter* painter,
                       bool is_expanded) {
  if (is_expanded) {
    auto thumb_rect = GetExpandedAreaRect(ScrollBarAreaKind::Thumb);
    if (thumb_rect)
      painter->FillRectangle(*thumb_rect, GetExpandedThumbBrush().get().get());

    auto slot_brush = GetExpandedSlotBrush().get().get();

    auto up_slot_rect = GetExpandedAreaRect(ScrollBarAreaKind::UpSlot);
    if (up_slot_rect) painter->FillRectangle(*up_slot_rect, slot_brush);

    auto down_slot_rect = GetExpandedAreaRect(ScrollBarAreaKind::DownSlot);
    if (down_slot_rect) painter->FillRectangle(*down_slot_rect, slot_brush);

    auto up_arrow = GetExpandedAreaRect(ScrollBarAreaKind::UpArrow);
    if (up_arrow) this->DrawUpArrow(painter, *up_arrow);

    auto down_arrow = GetExpandedAreaRect(ScrollBarAreaKind::DownArrow);
    if (down_arrow) this->DrawUpArrow(painter, *down_arrow);
  } else {
    auto optional_rect = GetCollapsedThumbRect();
    if (optional_rect) {
      painter->FillRectangle(*optional_rect,
                             GetCollapsedThumbBrush().get().get());
    }
  }
}

std::optional<ScrollBarAreaKind> ScrollBar::ExpandedHitTest(
    const Point& point) {
  for (auto kind : kScrollBarAreaKindList) {
    auto rect = this->GetExpandedAreaRect(kind);
    if (rect) {
      if (rect->IsPointInside(point)) return kind;
    }
  }
  return std::nullopt;
}

HorizontalScrollBar::HorizontalScrollBar(
    gsl::not_null<ScrollRenderObject*> render_object)
    : ScrollBar(render_object, Direction::Horizontal) {}

void HorizontalScrollBar::DrawUpArrow(platform::graphics::IPainter* painter,
                                      const Rect& area) {
  // TODO: Do what you must!
  painter->FillRectangle(area, GetExpandedArrowBackgroundBrush().get().get());
}

void HorizontalScrollBar::DrawDownArrow(platform::graphics::IPainter* painter,
                                        const Rect& area) {
  // TODO: Do what you must!
  painter->FillRectangle(area, GetExpandedArrowBackgroundBrush().get().get());
}

bool HorizontalScrollBar::IsShowBar() {
  const auto child = render_object_->GetFirstChild();
  if (child == nullptr) return false;

  const auto view_rect = render_object_->GetViewRect();
  const auto child_size = child->GetSize();

  if (view_rect.width >= child_size.width) return false;

  return true;
}

std::optional<Rect> HorizontalScrollBar::GetExpandedAreaRect(
    ScrollBarAreaKind area_kind) {
  auto show = IsShowBar();
  if (!show) return std::nullopt;

  const auto padding_rect = render_object_->GetPaddingRect();

  const auto child = render_object_->GetFirstChild();

  const auto view_rect = render_object_->GetViewRect();
  const auto child_size = child->GetSize();

  const float start_percentage = view_rect.left / child_size.width;
  const float length_percentage = view_rect.width / child_size.width;
  const float end_percentage = start_percentage + length_percentage;

  const float top = padding_rect.GetBottom() - kScrollBarExpandWidth;
  const float height = kScrollBarExpandWidth;

  // Without arrow.
  const float bar_area_length = padding_rect.width - 3 * kScrollBarExpandWidth;
  const float bar_area_start = padding_rect.left + kScrollBarExpandWidth;

  switch (area_kind) {
    case ScrollBarAreaKind::UpArrow:
      return Rect{padding_rect.left, top, kScrollBarExpandWidth, height};
    case ScrollBarAreaKind::DownArrow:
      return Rect{padding_rect.GetRight() - 2 * kScrollBarExpandWidth, top,
                  kScrollBarExpandWidth, height};
    case ScrollBarAreaKind::UpSlot:
      return Rect{bar_area_start, top, bar_area_length * start_percentage,
                  height};
    case ScrollBarAreaKind::DownSlot:
      return Rect{bar_area_start + bar_area_length * end_percentage, top,
                  bar_area_length * (1 - end_percentage), height};
    case ScrollBarAreaKind::Thumb:
      return Rect{bar_area_start + bar_area_length * start_percentage, top,
                  bar_area_length * length_percentage, height};
    default:
      throw std::invalid_argument("Unsupported scroll area kind.");
  }
}

std::optional<Rect> HorizontalScrollBar::GetCollapsedTriggerExpandAreaRect() {
  auto show = IsShowBar();
  if (!show) return std::nullopt;

  const auto padding_rect = render_object_->GetPaddingRect();

  return Rect{
      padding_rect.left,
      padding_rect.GetBottom() - kScrollBarCollapsedTriggerExpandAreaWidth,
      padding_rect.width, kScrollBarCollapseThumbWidth};
}

std::optional<Rect> HorizontalScrollBar::GetCollapsedThumbRect() {
  auto show = IsShowBar();
  if (!show) return std::nullopt;

  const auto child = render_object_->GetFirstChild();

  const auto view_rect = render_object_->GetViewRect();
  const auto child_size = child->GetSize();

  const float start_percentage = view_rect.left / child_size.width;
  const float length_percentage = view_rect.width / child_size.width;
  // const float end_percentage = start_percentage + length_percentage;

  const auto padding_rect = render_object_->GetPaddingRect();

  return Rect{padding_rect.left + padding_rect.width * start_percentage,
              padding_rect.GetBottom() - kScrollBarCollapseThumbWidth,
              padding_rect.width * length_percentage,
              kScrollBarCollapseThumbWidth};
}

float HorizontalScrollBar::CalculateNewScrollPosition(
    const Rect& thumb_original_rect, const Point& mouse_offset) {
  auto new_thumb_start = thumb_original_rect.left + mouse_offset.x;

  const auto padding_rect = render_object_->GetPaddingRect();

  auto scroll_area_start = padding_rect.left + kScrollBarExpandWidth;
  auto scroll_area_end = padding_rect.GetRight() - 2 * kScrollBarExpandWidth;

  auto thumb_head_end = scroll_area_end - thumb_original_rect.width;

  const auto child = render_object_->GetFirstChild();
  const auto child_size = child->GetSize();

  new_thumb_start =
      std::clamp(new_thumb_start, scroll_area_start, thumb_head_end);

  auto offset = new_thumb_start / (scroll_area_end - scroll_area_start) *
                child_size.width;

  return offset;
}

VerticalScrollBar::VerticalScrollBar(
    gsl::not_null<ScrollRenderObject*> render_object)
    : ScrollBar(render_object, Direction::Vertical) {}

void VerticalScrollBar::DrawUpArrow(platform::graphics::IPainter* painter,
                                    const Rect& area) {
  // TODO: Do what you must!
  painter->FillRectangle(area, GetExpandedArrowBackgroundBrush().get().get());
}

void VerticalScrollBar::DrawDownArrow(platform::graphics::IPainter* painter,
                                      const Rect& area) {
  // TODO: Do what you must!
  painter->FillRectangle(area, GetExpandedArrowBackgroundBrush().get().get());
}

bool VerticalScrollBar::IsShowBar() {
  const auto child = render_object_->GetFirstChild();
  if (child == nullptr) return false;

  const auto view_rect = render_object_->GetViewRect();
  const auto child_size = child->GetSize();

  if (view_rect.height >= child_size.height) return false;

  return true;
}

std::optional<Rect> VerticalScrollBar::GetExpandedAreaRect(
    ScrollBarAreaKind area_kind) {
  auto show = IsShowBar();
  if (!show) return std::nullopt;

  const auto padding_rect = render_object_->GetPaddingRect();

  const auto child = render_object_->GetFirstChild();

  const auto view_rect = render_object_->GetViewRect();
  const auto child_size = child->GetSize();

  const float start_percentage = view_rect.top / child_size.height;
  const float length_percentage = view_rect.height / child_size.height;
  const float end_percentage = start_percentage + length_percentage;

  const float left = padding_rect.GetRight() - kScrollBarExpandWidth;
  const float width = kScrollBarExpandWidth;

  // Without arrow.
  const float bar_area_length = padding_rect.height - 3 * kScrollBarExpandWidth;
  const float bar_area_start = padding_rect.top + kScrollBarExpandWidth;

  switch (area_kind) {
    case ScrollBarAreaKind::UpArrow:
      return Rect{left, padding_rect.top, width, kScrollBarExpandWidth};
    case ScrollBarAreaKind::DownArrow:
      return Rect{left, padding_rect.GetBottom() - 2 * kScrollBarExpandWidth,
                  width, kScrollBarExpandWidth};
    case ScrollBarAreaKind::UpSlot:
      return Rect{left, bar_area_start, width,
                  bar_area_length * start_percentage};
    case ScrollBarAreaKind::DownSlot:
      return Rect{left, bar_area_start + bar_area_length * end_percentage,
                  width, bar_area_length * (1 - end_percentage)};
    case ScrollBarAreaKind::Thumb:
      return Rect{left, bar_area_start + bar_area_length * start_percentage,
                  width, bar_area_length * length_percentage};
    default:
      throw std::invalid_argument("Unsupported scroll area kind.");
  }
}

std::optional<Rect> VerticalScrollBar::GetCollapsedTriggerExpandAreaRect() {
  auto show = IsShowBar();
  if (!show) return std::nullopt;

  const auto padding_rect = render_object_->GetPaddingRect();

  return Rect{
      padding_rect.GetRight() - kScrollBarCollapsedTriggerExpandAreaWidth,
      padding_rect.top, kScrollBarCollapseThumbWidth, padding_rect.height};
}

std::optional<Rect> VerticalScrollBar::GetCollapsedThumbRect() {
  const auto child = render_object_->GetFirstChild();
  if (child == nullptr) return std::nullopt;

  const auto view_rect = render_object_->GetViewRect();
  const auto padding_rect = render_object_->GetPaddingRect();
  const auto child_size = child->GetSize();

  if (view_rect.height >= child_size.height) return std::nullopt;

  const float start_percentage = view_rect.top / child_size.height;
  const float length_percentage = view_rect.height / child_size.height;
  // const float end_percentage = start_percentage + length_percentage;

  return Rect{padding_rect.GetRight() - kScrollBarCollapseThumbWidth,
              padding_rect.top + padding_rect.height * start_percentage,
              kScrollBarCollapseThumbWidth,
              padding_rect.height * length_percentage};
}

float VerticalScrollBar::CalculateNewScrollPosition(
    const Rect& thumb_original_rect, const Point& mouse_offset) {
  auto new_thumb_start = thumb_original_rect.top + mouse_offset.y;

  const auto padding_rect = render_object_->GetPaddingRect();

  auto scroll_area_start = padding_rect.top + kScrollBarExpandWidth;
  auto scroll_area_end = padding_rect.GetBottom() - 2 * kScrollBarExpandWidth;

  auto thumb_head_end = scroll_area_end - thumb_original_rect.height;

  const auto child = render_object_->GetFirstChild();
  const auto child_size = child->GetSize();

  new_thumb_start =
      std::clamp(new_thumb_start, scroll_area_start, thumb_head_end);

  auto offset = new_thumb_start / (scroll_area_end - scroll_area_start) *
                child_size.height;

  return offset;
}

ScrollBarDelegate::ScrollBarDelegate(
    gsl::not_null<ScrollRenderObject*> render_object)
    : render_object_(render_object),
      horizontal_bar_(render_object),
      vertical_bar_(render_object) {
  horizontal_bar_.ScrollAttemptEvent()->AddHandler(
      [this](auto scroll) { this->scroll_attempt_event_.Raise(scroll); });
  vertical_bar_.ScrollAttemptEvent()->AddHandler(
      [this](auto scroll) { this->scroll_attempt_event_.Raise(scroll); });
}

void ScrollBarDelegate::DrawScrollBar(platform::graphics::IPainter* painter) {
  horizontal_bar_.Draw(painter);
  vertical_bar_.Draw(painter);
}

void ScrollBarDelegate::InstallHandlers(controls::Control* control) {
  horizontal_bar_.InstallHandlers(control);
  vertical_bar_.InstallHandlers(control);
}
}  // namespace cru::ui::render
