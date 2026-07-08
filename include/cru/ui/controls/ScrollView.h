#pragma once
#include "../render/RenderObject.h"
#include "../render/ScrollRenderObject.h"
#include "../render/StackLayoutRenderObject.h"
#include "SingleChildControl.h"

namespace cru::ui::controls {
using render::kScrollBarAreaKindList;
using render::Scroll;
using render::ScrollBarAreaKind;
using render::ScrollKind;

class ScrollControlMixin {
 protected:
  ScrollControlMixin(Control* self);

 public:
  bool IsMouseWheelScrollEnabled() { return is_mouse_wheel_enabled_; }
  void SetMouseWheelScrollEnabled(bool enable);

 protected:
  render::RenderObject* GetScrollRootRenderObject();

  render::ScrollRenderObject* GetScrollRenderObject();

 private:
  render::RenderObject* GetScrollChildRenderObject() {
    return scroll_render_object_.GetChild();
  }

  struct ScrollBarEventState {
    std::optional<ScrollBarAreaKind> mouse_press_area_;
    std::optional<Point> drag_thumb_mouse_start_;
    float drag_thumb_start_;
    platform::gui::TimerAutoCanceler auto_collapse_timer_canceler_;
  };

  void SyncScrollBars(render::ScrollState scroll_state);

  float CalculateDragNewScrollPosition(
      Direction direction, render::ScrollBarRenderObject* scroll_bar,
      ScrollBarEventState* state, Point point);

  void InstallHandlers(Control* self, render::ScrollBarRenderObject* scroll_bar,
                       ScrollBarEventState* state);

  void SetCursor();
  void RestoreCursor();
  void BeginAutoCollapseTimer(Control* self,
                              render::ScrollBarRenderObject* scroll_bar,
                              ScrollBarEventState* state);
  void StopAutoCollapseTimer(Control* self,
                             render::ScrollBarRenderObject* scroll_bar,
                             ScrollBarEventState* state);
  void OnMouseLeave(Control* self, render::ScrollBarRenderObject* scroll_bar,
                    ScrollBarEventState* state);

 private:
  Control* self_;

  render::StackLayoutRenderObject stack_layout_render_object_;
  render::ScrollRenderObject scroll_render_object_;
  render::HorizontalScrollBarRenderObject horizontal_scroll_bar_render_object_;
  render::VerticalScrollBarRenderObject vertical_scroll_bar_render_object_;

  ScrollBarEventState horizontal_scroll_bar_event_state_;
  ScrollBarEventState vertical_scroll_bar_event_state_;

  bool is_mouse_wheel_enabled_ = true;
  bool cursor_overridden_ = false;

  EventHandlerRevokerListGuard scroll_event_guard_;
};

class CRU_UI_API ScrollView
    : public Control,
      public ScrollControlMixin,
      public SingleChildControlMixin<ScrollView, render::ScrollRenderObject> {
 public:
  static constexpr auto kControlName = "ScrollView";

  ScrollView();

  render::RenderObject* GetRenderObject() override;

 protected:
  render::ScrollRenderObject* GetContainerRenderObject() override;
};
}  // namespace cru::ui::controls
