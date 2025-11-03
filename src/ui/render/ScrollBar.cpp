#include "cru/ui/render/ScrollBar.h"

#include "../Helper.h"
#include "cru/platform/GraphicsBase.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Geometry.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/ui/Base.h"
#include "cru/ui/ThemeManager.h"
#include "cru/ui/host/WindowHost.h"
#include "cru/ui/render/ScrollRenderObject.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

namespace cru::ui::render {
using namespace std::chrono_literals;
constexpr float kScrollBarCollapseThumbWidth = 3;
constexpr float kScrollBarCollapsedTriggerExpandAreaWidth = 7;
constexpr float kScrollBarExpandWidth = 14;
constexpr float kScrollBarArrowHeight = 4;
constexpr auto kScrollBarAutoCollapseDelay = 1500ms;

constexpr std::array<ScrollBarAreaKind, 5> kScrollBarAreaKindList{
    ScrollBarAreaKind::UpArrow, ScrollBarAreaKind::DownArrow,
    ScrollBarAreaKind::UpSlot, ScrollBarAreaKind::DownSlot,
    ScrollBarAreaKind::Thumb};

std::string GenerateScrollBarThemeColorKey(ScrollBarBrushUsageKind usage,
                                           ScrollBarBrushStateKind state) {
  std::string result = "scrollbar.";
  switch (usage) {
    case ScrollBarBrushUsageKind::Arrow:
      result.append("arrow");
      break;
    case ScrollBarBrushUsageKind::ArrowBackground:
      result.append("arrow-background");
      break;
    case ScrollBarBrushUsageKind::Slot:
      result.append("slot");
      break;
    case ScrollBarBrushUsageKind::Thumb:
      result.append("thumb");
      break;
  }
  result.push_back('.');
  switch (state) {
    case ScrollBarBrushStateKind::Normal:
      result.append("normal");
      break;
    case ScrollBarBrushStateKind::Hover:
      result.append("hover");
      break;
    case ScrollBarBrushStateKind::Press:
      result.append("press");
      break;
    case ScrollBarBrushStateKind::Disable:
      result.append("disable");
      break;
  }
  result.append(".color");
  return result;
}

namespace {
std::unique_ptr<platform::graphics::IGeometry> CreateScrollBarArrowGeometry() {
  auto geometry_builder = GetGraphicsFactory()->CreateGeometryBuilder();
  geometry_builder->MoveTo({-kScrollBarArrowHeight / 2, 0});
  geometry_builder->LineTo({kScrollBarArrowHeight / 2, kScrollBarArrowHeight});
  geometry_builder->LineTo({kScrollBarArrowHeight / 2, -kScrollBarArrowHeight});
  geometry_builder->CloseFigure(true);
  return geometry_builder->Build();
}
}  // namespace

ScrollBar::ScrollBar(ScrollRenderObject* render_object, Direction direction)
    : render_object_(render_object), direction_(direction) {
  arrow_geometry_ = CreateScrollBarArrowGeometry();
}

ScrollBar::~ScrollBar() { RestoreCursor(); }

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
    event_guard_ +=
        control->MouseDownEvent()->Tunnel()->PrependShortCircuitHandler(
            [control, this](events::MouseButtonEventArgs& event) {
              if (event.GetButton() == MouseButtons::Left && IsEnabled() &&
                  IsExpanded()) {
                auto hit_test_result =
                    ExpandedHitTest(event.GetPoint(render_object_));
                if (!hit_test_result) return false;

                if (mouse_press_ != hit_test_result) {
                  mouse_press_ = hit_test_result;
                  render_object_->InvalidatePaint();
                }

                switch (*hit_test_result) {
                  case ScrollBarAreaKind::UpArrow:
                    this->scroll_attempt_event_.Raise(
                        {GetDirection(), ScrollKind::Line, -1});
                    event.SetHandled();
                    return true;
                  case ScrollBarAreaKind::DownArrow:
                    this->scroll_attempt_event_.Raise(
                        {GetDirection(), ScrollKind::Line, 1});
                    event.SetHandled();
                    return true;
                  case ScrollBarAreaKind::UpSlot:
                    this->scroll_attempt_event_.Raise(
                        {GetDirection(), ScrollKind::Page, -1});
                    event.SetHandled();
                    return true;
                  case ScrollBarAreaKind::DownSlot:
                    this->scroll_attempt_event_.Raise(
                        {GetDirection(), ScrollKind::Page, 1});
                    event.SetHandled();
                    return true;
                  case ScrollBarAreaKind::Thumb: {
                    auto thumb_rect =
                        GetExpandedAreaRect(ScrollBarAreaKind::Thumb);
                    assert(thumb_rect);

                    if (!control->CaptureMouse()) break;
                    move_thumb_thumb_original_rect_ = *thumb_rect;
                    move_thumb_start_ = event.GetPoint();
                    event.SetHandled();
                    return true;
                  }
                  default:
                    break;
                }
              }

              return false;
            });

    event_guard_ +=
        control->MouseUpEvent()->Tunnel()->PrependShortCircuitHandler(
            [control, this](events::MouseButtonEventArgs& event) {
              if (mouse_press_ != std::nullopt) {
                mouse_press_ = std::nullopt;
                render_object_->InvalidatePaint();
              }

              if (event.GetButton() == MouseButtons::Left &&
                  move_thumb_start_) {
                move_thumb_start_ = std::nullopt;

                auto hit_test_result =
                    ExpandedHitTest(event.GetPoint(this->render_object_));
                if (!hit_test_result) {
                  OnMouseLeave();
                }

                control->ReleaseMouse();
                event.SetHandled();
                return true;
              }
              return false;
            });

    event_guard_ +=
        control->MouseMoveEvent()->Tunnel()->PrependShortCircuitHandler(
            [this](events::MouseEventArgs& event) {
              if (move_thumb_start_) {
                auto new_scroll_position = CalculateNewScrollPosition(
                    move_thumb_thumb_original_rect_,
                    event.GetPoint() - *move_thumb_start_);

                this->scroll_attempt_event_.Raise({GetDirection(),
                                                   ScrollKind::Absolute,
                                                   new_scroll_position});
                event.SetHandled();
                return true;
              }

              if (IsEnabled()) {
                if (IsExpanded()) {
                  auto hit_test_result =
                      ExpandedHitTest(event.GetPoint(this->render_object_));
                  if (mouse_hover_ != hit_test_result) {
                    mouse_hover_ = hit_test_result;
                    render_object_->InvalidatePaint();
                  }
                  if (hit_test_result) {
                    SetCursor();
                    StopAutoCollapseTimer();
                  } else {
                    OnMouseLeave();
                  }
                } else {
                  auto trigger_expand_area =
                      GetCollapsedTriggerExpandAreaRect();
                  if (trigger_expand_area &&
                      trigger_expand_area->IsPointInside(
                          event.GetPoint(this->render_object_))) {
                    SetExpanded(true);
                    SetCursor();
                    event.SetHandled();
                    return true;
                  }
                }
              }

              return false;
            });

    event_guard_ +=
        control->MouseLeaveEvent()->Tunnel()->PrependShortCircuitHandler(
            [this](events::MouseEventArgs&) {
              if (IsExpanded() && !move_thumb_start_) {
                if (mouse_hover_ != std::nullopt) {
                  mouse_hover_ = std::nullopt;
                  render_object_->InvalidatePaint();
                }
                OnMouseLeave();
              }
              return false;
            });
  }
}

std::shared_ptr<platform::graphics::IBrush>
ScrollBar::GetCollapsedThumbBrush() {
  return collapsed_thumb_brush_ ? collapsed_thumb_brush_
                                : ThemeManager::GetInstance()->GetResourceBrush(
                                      "scrollbar.collapse-thumb.color");
}

void ScrollBar::SetCollapsedThumbBrush(
    std::shared_ptr<platform::graphics::IBrush> brush) {
  if (brush == collapsed_thumb_brush_) return;
  collapsed_thumb_brush_ = std::move(brush);
  render_object_->InvalidatePaint();
}

std::shared_ptr<platform::graphics::IBrush> ScrollBar::GetBrush(
    ScrollBarBrushUsageKind usage, ScrollBarBrushStateKind state) {
  auto b = brushes_[usage][state];
  return b ? b
           : ThemeManager::GetInstance()->GetResourceBrush(
                 GenerateScrollBarThemeColorKey(usage, state));
}

// Brush could be nullptr to use the theme brush.
void ScrollBar::SetBrush(ScrollBarBrushUsageKind usage,
                         ScrollBarBrushStateKind state,
                         std::shared_ptr<platform::graphics::IBrush> brush) {
  if (brushes_[usage][state] == brush) return;
  brushes_[usage][state] = std::move(brush);
  render_object_->InvalidatePaint();
}

void ScrollBar::OnDraw(platform::graphics::IPainter* painter,
                       bool is_expanded) {
  if (is_expanded) {
    auto thumb_rect = GetExpandedAreaRect(ScrollBarAreaKind::Thumb);
    if (thumb_rect) {
      auto thumb_state = GetState(ScrollBarAreaKind::Thumb);
      painter->FillRectangle(
          *thumb_rect,
          GetBrush(ScrollBarBrushUsageKind::Thumb, thumb_state).get());
    }

    auto up_slot_rect = GetExpandedAreaRect(ScrollBarAreaKind::UpSlot);
    auto up_slot_state = GetState(ScrollBarAreaKind::UpSlot);
    if (up_slot_rect)
      painter->FillRectangle(
          *up_slot_rect,
          GetBrush(ScrollBarBrushUsageKind::Slot, up_slot_state).get());

    auto down_slot_rect = GetExpandedAreaRect(ScrollBarAreaKind::DownSlot);
    auto down_slot_state = GetState(ScrollBarAreaKind::DownSlot);
    if (down_slot_rect)
      painter->FillRectangle(
          *down_slot_rect,
          GetBrush(ScrollBarBrushUsageKind::Slot, down_slot_state).get());

    auto up_arrow_rect = GetExpandedAreaRect(ScrollBarAreaKind::UpArrow);
    auto up_arrow_state = GetState(ScrollBarAreaKind::UpArrow);
    if (up_arrow_rect)
      this->DrawUpArrow(
          painter, *up_arrow_rect,
          GetBrush(ScrollBarBrushUsageKind::Arrow, up_arrow_state).get(),
          GetBrush(ScrollBarBrushUsageKind::ArrowBackground, up_arrow_state)

              .get());

    auto down_arrow_rect = GetExpandedAreaRect(ScrollBarAreaKind::DownArrow);
    auto down_arrow_state = GetState(ScrollBarAreaKind::DownArrow);
    if (down_arrow_rect)
      this->DrawDownArrow(
          painter, *down_arrow_rect,
          GetBrush(ScrollBarBrushUsageKind::Arrow, down_arrow_state).get(),
          GetBrush(ScrollBarBrushUsageKind::ArrowBackground, down_arrow_state)
              .get());
  } else {
    auto optional_rect = GetCollapsedThumbRect();
    if (optional_rect) {
      painter->FillRectangle(*optional_rect, GetCollapsedThumbBrush().get());
    }
  }
}

void ScrollBar::SetCursor() {
  if (const auto control = render_object_->GetAttachedControl()) {
    if (const auto window_host = control->GetWindowHost()) {
      window_host->SetOverrideCursor(
          GetUiApplication()->GetCursorManager()->GetSystemCursor(
              platform::gui::SystemCursorType::Arrow));
      cursor_overrided_ = true;
    }
  }
}

void ScrollBar::RestoreCursor() {
  if (cursor_overrided_) {
    if (const auto control = render_object_->GetAttachedControl()) {
      if (const auto window_host = control->GetWindowHost()) {
        window_host->SetOverrideCursor(nullptr);
      }
    }
    cursor_overrided_ = false;
  }
}

void ScrollBar::BeginAutoCollapseTimer() {
  if (!auto_collapse_timer_canceler_ && IsExpanded()) {
    auto_collapse_timer_canceler_ = GetUiApplication()->SetTimeout(
        kScrollBarAutoCollapseDelay, [this] { this->SetExpanded(false); });
  }
}

void ScrollBar::StopAutoCollapseTimer() {
  auto_collapse_timer_canceler_.Reset();
}

void ScrollBar::OnMouseLeave() {
  RestoreCursor();
  BeginAutoCollapseTimer();
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

ScrollBarBrushStateKind ScrollBar::GetState(ScrollBarAreaKind area) {
  if (area == ScrollBarAreaKind::UpArrow) {
    if (!CanScrollUp()) return ScrollBarBrushStateKind::Disable;
  }

  if (area == ScrollBarAreaKind::DownArrow) {
    if (!CanScrollDown()) return ScrollBarBrushStateKind::Disable;
  }

  if (mouse_hover_ == area) {
    if (mouse_press_ == area) {
      return ScrollBarBrushStateKind::Press;
    } else {
      return ScrollBarBrushStateKind::Hover;
    }
  } else {
    return ScrollBarBrushStateKind::Normal;
  }
}

HorizontalScrollBar::HorizontalScrollBar(ScrollRenderObject* render_object)
    : ScrollBar(render_object, Direction::Horizontal) {}

void HorizontalScrollBar::DrawUpArrow(
    platform::graphics::IPainter* painter, const Rect& area,
    platform::graphics::IBrush* arrow_brush,
    platform::graphics::IBrush* background_brush) {
  painter->FillRectangle(area, background_brush);

  painter->WithTransform(
      Matrix::Translation(area.GetCenter()),
      [this, arrow_brush](platform::graphics::IPainter* painter) {
        painter->FillGeometry(arrow_geometry_.get(), arrow_brush);
      });
}

void HorizontalScrollBar::DrawDownArrow(
    platform::graphics::IPainter* painter, const Rect& area,
    platform::graphics::IBrush* arrow_brush,
    platform::graphics::IBrush* background_brush) {
  painter->FillRectangle(area, background_brush);

  painter->WithTransform(
      Matrix::Rotation(180) * Matrix::Translation(area.GetCenter()),
      [this, arrow_brush](platform::graphics::IPainter* painter) {
        painter->FillGeometry(arrow_geometry_.get(), arrow_brush);
      });
}

bool HorizontalScrollBar::IsShowBar() {
  const auto child = render_object_->GetChild();
  if (child == nullptr) return false;

  const auto view_rect = render_object_->GetViewRect();
  const auto child_size = child->GetDesiredSize();

  if (view_rect.width >= child_size.width) return false;

  return true;
}

std::optional<Rect> HorizontalScrollBar::GetExpandedAreaRect(
    ScrollBarAreaKind area_kind) {
  auto show = IsShowBar();
  if (!show) return std::nullopt;

  const auto padding_rect = render_object_->GetPaddingRect();

  const auto child = render_object_->GetChild();

  const auto view_rect = render_object_->GetViewRect();
  const auto child_size = child->GetDesiredSize();

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

  const auto child = render_object_->GetChild();

  const auto view_rect = render_object_->GetViewRect();
  const auto child_size = child->GetDesiredSize();

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

  const auto child = render_object_->GetChild();
  const auto child_size = child->GetDesiredSize();

  new_thumb_start =
      std::clamp(new_thumb_start, scroll_area_start, thumb_head_end);

  auto offset = (new_thumb_start - scroll_area_start) /
                (scroll_area_end - scroll_area_start) * child_size.width;

  return offset;
}

bool HorizontalScrollBar::CanScrollUp() {
  return render_object_->HorizontalCanScrollUp();
}

bool HorizontalScrollBar::CanScrollDown() {
  return render_object_->HorizontalCanScrollDown();
}

VerticalScrollBar::VerticalScrollBar(ScrollRenderObject* render_object)
    : ScrollBar(render_object, Direction::Vertical) {}

void VerticalScrollBar::DrawUpArrow(
    platform::graphics::IPainter* painter, const Rect& area,
    platform::graphics::IBrush* arrow_brush,
    platform::graphics::IBrush* background_brush) {
  painter->FillRectangle(area, background_brush);

  painter->WithTransform(
      Matrix::Rotation(90) * Matrix::Translation(area.GetCenter()),
      [this, arrow_brush](platform::graphics::IPainter* painter) {
        painter->FillGeometry(arrow_geometry_.get(), arrow_brush);
      });
}

void VerticalScrollBar::DrawDownArrow(
    platform::graphics::IPainter* painter, const Rect& area,
    platform::graphics::IBrush* arrow_brush,
    platform::graphics::IBrush* background_brush) {
  painter->FillRectangle(area, background_brush);

  painter->WithTransform(
      Matrix::Rotation(270) * Matrix::Translation(area.GetCenter()),
      [this, arrow_brush](platform::graphics::IPainter* painter) {
        painter->FillGeometry(arrow_geometry_.get(), arrow_brush);
      });
}

bool VerticalScrollBar::IsShowBar() {
  const auto child = render_object_->GetChild();
  if (child == nullptr) return false;

  const auto view_rect = render_object_->GetViewRect();
  const auto child_size = child->GetDesiredSize();

  if (view_rect.height >= child_size.height) return false;

  return true;
}

std::optional<Rect> VerticalScrollBar::GetExpandedAreaRect(
    ScrollBarAreaKind area_kind) {
  auto show = IsShowBar();
  if (!show) return std::nullopt;

  const auto padding_rect = render_object_->GetPaddingRect();

  const auto child = render_object_->GetChild();

  const auto view_rect = render_object_->GetViewRect();
  const auto child_size = child->GetDesiredSize();

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
  const auto child = render_object_->GetChild();
  if (child == nullptr) return std::nullopt;

  const auto view_rect = render_object_->GetViewRect();
  const auto padding_rect = render_object_->GetPaddingRect();
  const auto child_size = child->GetDesiredSize();

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

  const auto child = render_object_->GetChild();
  const auto child_size = child->GetDesiredSize();

  new_thumb_start =
      std::clamp(new_thumb_start, scroll_area_start, thumb_head_end);

  auto offset = (new_thumb_start - scroll_area_start) /
                (scroll_area_end - scroll_area_start) * child_size.height;

  return offset;
}

bool VerticalScrollBar::CanScrollUp() {
  return render_object_->VerticalCanScrollUp();
}

bool VerticalScrollBar::CanScrollDown() {
  return render_object_->VerticalCanScrollDown();
}

ScrollBarDelegate::ScrollBarDelegate(ScrollRenderObject* render_object)
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
