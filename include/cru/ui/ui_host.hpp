#pragma once
#include "base.hpp"

#include "cru/common/event.hpp"
#include "cru/common/self_resolvable.hpp"
#include "render/base.hpp"

namespace cru::ui {
struct AfterLayoutEventArgs {};

class UiHost : public Object, public SelfResolvable<UiHost> {
 public:
  // This will create root window render object and attach it to window.
  // It will also create and manage a native window.
  UiHost(Window* window);

  CRU_DELETE_COPY(UiHost)
  CRU_DELETE_MOVE(UiHost)

  ~UiHost() override;

 public:
  // Mark the layout as invalid, and arrange a re-layout later.
  // This method could be called more than one times in a message cycle. But
  // layout only takes place once.
  void InvalidateLayout();

  // Mark the paint as invalid, and arrange a re-paint later.
  // This method could be called more than one times in a message cycle. But
  // paint only takes place once.
  void InvalidatePaint();

  IEvent<AfterLayoutEventArgs>* AfterLayoutEvent() {
    return &after_layout_event_;
  }

  void Relayout();

  // Get current control that mouse hovers on. This ignores the mouse-capture
  // control. Even when mouse is captured by another control, this function
  // return the control under cursor. You can use `GetMouseCaptureControl` to
  // get more info.
  Control* GetMouseHoverControl() const { return mouse_hover_control_; }

  //*************** region: focus ***************

  // Request focus for specified control.
  bool RequestFocusFor(Control* control);

  // Get the control that has focus.
  Control* GetFocusControl();

  //*************** region: focus ***************

  // Pass nullptr to release capture. If mouse is already capture by a control,
  // this capture will fail and return false. If control is identical to the
  // capturing control, capture is not changed and this function will return
  // true.
  //
  // When capturing control changes,
  // appropriate event will be sent. If mouse is not on the capturing control
  // and capture is released, mouse enter event will be sent to the mouse-hover
  // control. If mouse is not on the capturing control and capture is set, mouse
  // leave event will be sent to the mouse-hover control.
  bool CaptureMouseFor(Control* control);

  // Return null if not captured.
  Control* GetMouseCaptureControl();

  Control* HitTest(const Point& point);

  void UpdateCursor();

 private:
  //*************** region: native messages ***************
  void OnNativeDestroy(platform::native::INativeWindow* window, std::nullptr_t);
  void OnNativePaint(platform::native::INativeWindow* window, std::nullptr_t);
  void OnNativeResize(platform::native::INativeWindow* window,
                      const Size& size);

  void OnNativeFocus(platform::native::INativeWindow* window,
                     cru::platform::native::FocusChangeType focus);

  void OnNativeMouseEnterLeave(
      platform::native::INativeWindow* window,
      cru::platform::native::MouseEnterLeaveType enter);
  void OnNativeMouseMove(platform::native::INativeWindow* window,
                         const Point& point);
  void OnNativeMouseDown(
      platform::native::INativeWindow* window,
      const platform::native::NativeMouseButtonEventArgs& args);
  void OnNativeMouseUp(
      platform::native::INativeWindow* window,
      const platform::native::NativeMouseButtonEventArgs& args);

  void OnNativeKeyDown(platform::native::INativeWindow* window,
                       const platform::native::NativeKeyEventArgs& args);
  void OnNativeKeyUp(platform::native::INativeWindow* window,
                     const platform::native::NativeKeyEventArgs& args);

  //*************** region: event dispatcher helper ***************

  void DispatchMouseHoverControlChangeEvent(Control* old_control,
                                            Control* new_control,
                                            const Point& point, bool no_leave,
                                            bool no_enter);

 private:
  bool need_layout_ = false;

  Event<AfterLayoutEventArgs> after_layout_event_;

  std::shared_ptr<platform::native::INativeWindowResolver>
      native_window_resolver_;

  std::vector<EventRevokerGuard> event_revoker_guards_;

  Window* window_control_;
  std::unique_ptr<render::WindowRenderObject> root_render_object_;

  Control* mouse_hover_control_;

  Control* focus_control_;  // "focus_control_" can't be nullptr

  Control* mouse_captured_control_;
};
}  // namespace cru::ui
