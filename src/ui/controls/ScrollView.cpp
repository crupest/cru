#include "cru/ui/controls/ScrollView.h"
#include "cru/ui/controls/ControlHost.h"

#include <chrono>

namespace cru::ui::controls {
using namespace std::chrono_literals;

constexpr auto kScrollBarAutoCollapseDelay = 1500ms;

ScrollControlMixin::ScrollControlMixin(Control* self) : self_(self) {
  stack_layout_render_object_.AddChild(&scroll_render_object_);
  stack_layout_render_object_.AddChild(&horizontal_scroll_bar_render_object_);
  stack_layout_render_object_.AddChild(&vertical_scroll_bar_render_object_);
  stack_layout_render_object_.SetChildLayoutData(
      &scroll_render_object_,
      render::StackChildLayoutData{.horizontal = Alignment::Stretch,
                                   .vertical = Alignment::Stretch});
  stack_layout_render_object_.SetChildLayoutData(
      &horizontal_scroll_bar_render_object_,
      render::StackChildLayoutData{.horizontal = Alignment::Stretch,
                                   .vertical = Alignment::End});
  stack_layout_render_object_.SetChildLayoutData(
      &vertical_scroll_bar_render_object_,
      render::StackChildLayoutData{.horizontal = Alignment::End,
                                   .vertical = Alignment::Stretch});

  stack_layout_render_object_.SetAttachedControl(self);
  horizontal_scroll_bar_render_object_.SetAttachedControl(self);
  vertical_scroll_bar_render_object_.SetAttachedControl(self);
  scroll_render_object_.SetAttachedControl(self);

  scroll_event_guard_ += scroll_render_object_.ScrollEvent()->AddHandler(
      [this](render::ScrollState scroll_state) {
        this->SyncScrollBars(scroll_state);
      });

  self->MouseWheelEvent()->Bubble()->AddHandler(
      [this](events::MouseWheelEventArgs& args) {
        if (!is_mouse_wheel_enabled_) return;

        auto delta = args.GetDelta();

        delta *= 24;

        auto scroll_state = scroll_render_object_.GetScrollState();
        if (delta > 0) {
          if (scroll_state.CanScrollDown()) {
            scroll_render_object_.ApplyScroll(
                Scroll{Direction::Vertical, ScrollKind::Relative, delta});
          } else if (scroll_state.CanScrollRight()) {
            scroll_render_object_.ApplyScroll(
                Scroll{Direction::Horizontal, ScrollKind::Relative, delta});
          }
        } else if (delta < 0) {
          if (scroll_state.CanScrollUp()) {
            scroll_render_object_.ApplyScroll(
                Scroll{Direction::Vertical, ScrollKind::Relative, delta});
          } else if (scroll_state.CanScrollLeft()) {
            scroll_render_object_.ApplyScroll(
                Scroll{Direction::Horizontal, ScrollKind::Relative, delta});
          }
        }
      });

  InstallHandlers(self, &horizontal_scroll_bar_render_object_,
                  &horizontal_scroll_bar_event_state_);
  InstallHandlers(self, &vertical_scroll_bar_render_object_,
                  &vertical_scroll_bar_event_state_);
}

void ScrollControlMixin::SetMouseWheelScrollEnabled(bool enable) {
  is_mouse_wheel_enabled_ = enable;
}

render::RenderObject* ScrollControlMixin::GetScrollRootRenderObject() {
  return &stack_layout_render_object_;
}

render::ScrollRenderObject* ScrollControlMixin::GetScrollRenderObject() {
  return &scroll_render_object_;
}

void ScrollControlMixin::SyncScrollBars(render::ScrollState scroll_state) {
  horizontal_scroll_bar_render_object_.SetEnabled(
      scroll_state.CanScrollHorizontal());
  vertical_scroll_bar_render_object_.SetEnabled(
      scroll_state.CanScrollVertical());
  horizontal_scroll_bar_render_object_.SetScrollState(
      scroll_state.GetHorizontalPercentage());
  vertical_scroll_bar_render_object_.SetScrollState(
      scroll_state.GetVerticalPercentage());
}

float ScrollControlMixin::CalculateDragNewScrollPosition(
    Direction direction, render::ScrollBarRenderObject* scroll_bar,
    ScrollBarEventState* state, Point point) {
  assert(state->drag_thumb_mouse_start_);
  auto scroll_area_rect =
      scroll_bar->GetExpandedAreaRect(ScrollBarAreaKind::Scroll);
  assert(scroll_area_rect);
  auto child = scroll_render_object_.GetChild();
  auto child_size = child ? child->GetMeasureResultSize() : Size{0, 0};
  float offset_percentage;
  float total_length;
  if (direction == Direction::Horizontal) {
    offset_percentage =
        (point.x - state->drag_thumb_mouse_start_->x) / scroll_area_rect->width;
    total_length = child_size.width;
  } else {
    offset_percentage = (point.y - state->drag_thumb_mouse_start_->y) /
                        scroll_area_rect->height;
    total_length = child_size.height;
  }
  return std::clamp(state->drag_thumb_start_ + offset_percentage, 0.0f, 1.0f) *
         total_length;
}

void ScrollControlMixin::InstallHandlers(
    Control* self, render::ScrollBarRenderObject* scroll_bar,
    ScrollBarEventState* state) {
  self->MouseDownEvent()->Tunnel()->AddHandler([this, self, scroll_bar, state](
                                                   events::MouseButtonEventArgs&
                                                       event) {
    if (event.GetButton() == MouseButtons::Left && scroll_bar->IsEnabled() &&
        scroll_bar->IsExpanded()) {
      auto point = event.GetPoint(scroll_bar);
      auto hit_test_result = scroll_bar->ExpandedHitTest(point);
      if (!hit_test_result) return;

      state->mouse_press_area_ = hit_test_result;
      for (auto area : kScrollBarAreaKindList) {
        if (area == *hit_test_result) {
          scroll_bar->SetBrushState(area,
                                    render::ScrollBarBrushStateKind::Press);
        } else {
          scroll_bar->SetBrushState(area,
                                    render::ScrollBarBrushStateKind::Normal);
        }
      }

      auto direction = scroll_bar->GetDirection();
      switch (*hit_test_result) {
        case ScrollBarAreaKind::UpArrow:
          scroll_render_object_.ApplyScroll({direction, ScrollKind::Line, -1});
          event.SetHandled();
          break;
        case ScrollBarAreaKind::DownArrow:
          scroll_render_object_.ApplyScroll({direction, ScrollKind::Line, 1});
          event.SetHandled();
          break;
        case ScrollBarAreaKind::UpSlot:
          scroll_render_object_.ApplyScroll({direction, ScrollKind::Page, -1});
          event.SetHandled();
          break;
        case ScrollBarAreaKind::DownSlot:
          scroll_render_object_.ApplyScroll({direction, ScrollKind::Page, 1});
          event.SetHandled();
          break;
        case ScrollBarAreaKind::Thumb: {
          auto thumb_rect =
              scroll_bar->GetExpandedAreaRect(ScrollBarAreaKind::Thumb);
          assert(thumb_rect);
          if (!self->CaptureMouse()) break;
          state->drag_thumb_mouse_start_ = point;
          state->drag_thumb_start_ = scroll_bar->GetScrollState().start;
          StopAutoCollapseTimer(self, scroll_bar, state);
          event.SetHandled();
          break;
        }
        default:
          break;
      }
    }
  });

  self->MouseUpEvent()->Tunnel()->AddHandler(
      [this, self, scroll_bar, state](events::MouseButtonEventArgs& event) {
        if (state->mouse_press_area_ != std::nullopt) {
          state->mouse_press_area_ = std::nullopt;
          for (auto area : kScrollBarAreaKindList) {
            scroll_bar->SetBrushState(area,
                                      render::ScrollBarBrushStateKind::Normal);
          }
        }

        if (event.GetButton() == MouseButtons::Left &&
            state->drag_thumb_mouse_start_) {
          state->drag_thumb_mouse_start_ = std::nullopt;

          auto hit_test_result =
              scroll_bar->ExpandedHitTest(event.GetPoint(scroll_bar));
          if (!hit_test_result) {
            OnMouseLeave(self, scroll_bar, state);
          }

          self->ReleaseMouse();
          event.SetHandled();
        }
      });

  self->MouseMoveEvent()->Tunnel()->AddHandler(
      [this, self, scroll_bar, state](events::MouseEventArgs& event) {
        auto point = event.GetPoint(scroll_bar);
        if (state->drag_thumb_mouse_start_) {
          auto direction = scroll_bar->GetDirection();
          auto new_scroll_position = CalculateDragNewScrollPosition(
              direction, scroll_bar, state, point);
          scroll_render_object_.ApplyScroll(
              {direction, ScrollKind::Absolute, new_scroll_position});
          event.SetHandled();
          return;
        }

        if (scroll_bar->IsEnabled()) {
          if (scroll_bar->IsExpanded()) {
            auto hit_test_result = scroll_bar->ExpandedHitTest(point);

            for (auto area : kScrollBarAreaKindList) {
              if (hit_test_result && area == hit_test_result) {
                scroll_bar->SetBrushState(
                    area, render::ScrollBarBrushStateKind::Hover);
              } else {
                scroll_bar->SetBrushState(
                    area, render::ScrollBarBrushStateKind::Normal);
              }
            }

            if (hit_test_result) {
              SetCursor();
              StopAutoCollapseTimer(self, scroll_bar, state);
            } else {
              OnMouseLeave(self, scroll_bar, state);
            }
          } else {
            auto trigger_expand_area =
                scroll_bar->GetCollapsedTriggerExpandAreaRect();
            if (trigger_expand_area &&
                trigger_expand_area->IsPointInside(point)) {
              scroll_bar->SetExpanded(true);
              SetCursor();
              event.SetHandled();
            }
          }
        }
      });

  self->MouseLeaveEvent()->Tunnel()->AddHandler(
      [this, self, scroll_bar, state](events::MouseEventArgs&) {
        if (scroll_bar->IsExpanded() && !state->drag_thumb_mouse_start_) {
          for (auto area : kScrollBarAreaKindList) {
            scroll_bar->SetBrushState(area,
                                      render::ScrollBarBrushStateKind::Normal);
          }
          OnMouseLeave(self, scroll_bar, state);
        }
      });
}

void ScrollControlMixin::SetCursor() {
  if (const auto host = self_->GetControlHost()) {
    host->SetOverrideCursor(
        platform::gui::IUiApplication::GetInstance()
            ->GetCursorManager()
            ->GetSystemCursor(platform::gui::SystemCursorType::Arrow));
    cursor_overridden_ = true;
  }
}
void ScrollControlMixin::RestoreCursor() {
  if (cursor_overridden_) {
    if (const auto host = self_->GetControlHost()) {
      host->SetOverrideCursor(nullptr);
    }
    cursor_overridden_ = false;
  }
}
void ScrollControlMixin::BeginAutoCollapseTimer(
    Control* self, render::ScrollBarRenderObject* scroll_bar,
    ScrollBarEventState* state) {
  CRU_UNUSED(self);
  if (!state->auto_collapse_timer_canceler_ && scroll_bar->IsExpanded()) {
    state->auto_collapse_timer_canceler_.Reset(
        platform::gui::IUiApplication::GetInstance()->SetTimeout(
            kScrollBarAutoCollapseDelay,
            [scroll_bar] { scroll_bar->SetExpanded(false); }));
  }
}
void ScrollControlMixin::StopAutoCollapseTimer(
    Control* self, render::ScrollBarRenderObject* scroll_bar,
    ScrollBarEventState* state) {
  CRU_UNUSED(self);
  CRU_UNUSED(scroll_bar);
  state->auto_collapse_timer_canceler_.Reset();
}

void ScrollControlMixin::OnMouseLeave(Control* self,
                                      render::ScrollBarRenderObject* scroll_bar,
                                      ScrollBarEventState* state) {
  RestoreCursor();
  BeginAutoCollapseTimer(self, scroll_bar, state);
}

ScrollView::ScrollView()
    : Control(kControlName, nullptr), ScrollControlMixin(this) {
  root_render_object_ = GetScrollRenderObject();
}

render::ScrollRenderObject* ScrollView::GetContainerRenderObject() {
  return GetScrollRenderObject();
}
}  // namespace cru::ui::controls
