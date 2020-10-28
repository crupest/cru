#pragma once
#include "Base.hpp"

#include "cru/common/Event.hpp"
#include "cru/platform/native/UiApplication.hpp"
#include "cru/platform/native/Window.hpp"
#include "render/Base.hpp"

#include <functional>

namespace cru::ui {
struct AfterLayoutEventArgs {};

// The bridge between control tree and native window.
class WindowHost : public Object {
  CRU_DEFINE_CLASS_LOG_TAG(u"cru::ui::WindowHost")

 public:
  WindowHost(Control* root_control);

  CRU_DELETE_COPY(WindowHost)
  CRU_DELETE_MOVE(WindowHost)

  ~WindowHost() override;

 public:
  platform::native::INativeWindow* GetNativeWindow() { return native_window_; }

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
  void Relayout(const Size& available_size);

  // Is layout is invalid, wait for relayout and then run the action. Otherwist
  // run it right now.
  void RunAfterLayoutStable(std::function<void()> action);

  // If true, preferred size of root render object is set to window size when
  // measure. Default is true.
  bool IsLayoutPreferToFillWindow() const;
  void SetLayoutPreferToFillWindow(bool value);

  // Get current control that mouse hovers on. This ignores the mouse-capture
  // control. Even when mouse is captured by another control, this function
  // return the control under cursor. You can use `GetMouseCaptureControl` to
  // get more info.
  Control* GetMouseHoverControl() const { return mouse_hover_control_; }

  //*************** region: focus ***************

  Control* GetFocusControl();

  void SetFocusControl(Control* control);

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
  Control* root_control_;
  render::RenderObject* root_render_object_;

  platform::native::INativeWindow* native_window_;

  bool need_layout_ = false;
  platform::native::TimerAutoCanceler relayout_timer_canceler_;
  Event<AfterLayoutEventArgs> after_layout_event_;
  std::vector<std::function<void()> > after_layout_stable_action_;

  std::vector<EventRevokerGuard> event_revoker_guards_;

  Control* mouse_hover_control_ = nullptr;

  Control* focus_control_;

  Control* mouse_captured_control_;

  bool layout_prefer_to_fill_window_ = true;
};
}  // namespace cru::ui
