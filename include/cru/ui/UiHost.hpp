#pragma once
#include "Base.hpp"

#include "cru/common/Event.hpp"
#include "cru/common/SelfResolvable.hpp"
#include "render/Base.hpp"

namespace cru::ui {
struct AfterLayoutEventArgs {};

// The host of all controls and render objects.
//
// 3 situations on destroy:
// 1. Native window destroyed, IsRetainAfterDestroy: false:
// OnNativeDestroy(set native_window_destroyed_ to true, call ~Window due to
// deleting_ is false and IsRetainAfterDestroy is false) -> ~Window ->
// ~UiHost(not destroy native window repeatedly due to native_window_destroyed_
// is true)
// 2. Native window destroyed, IsRetainAfterDestroy: true:
// OnNativeDestroy(set native_window_destroyed_ to true, not call ~Window
// because deleting_ is false and IsRetainAfterDestroy is true)
// then, ~Window -> ~UiHost(not destroy native window repeatedly due to
// native_window_destroyed_ is true)
// 3. Native window not destroyed, ~Window is called:
// ~Window -> ~UiHost(set deleting_ to true, destroy native window
// due to native_window_destroyed is false) -> OnNativeDestroy(not call ~Window
// due to deleting_ is true and IsRetainAfterDestroy is whatever)
// In conclusion:
// 1. Set native_window_destroyed_ to true at the beginning of OnNativeDestroy.
// 2. Set deleting_ to true at the beginning of ~UiHost.
// 3. Destroy native window when native_window_destroy_ is false in ~Window.
// 4. Delete Window when deleting_ is false and IsRetainAfterDestroy is false in
// OnNativeDestroy.
class UiHost : public Object, public SelfResolvable<UiHost> {
  CRU_DEFINE_CLASS_LOG_TAG(u"cru::ui::UiHost")

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

  // If true, preferred size of root render object is set to window size when
  // measure. Default is true.
  bool IsLayoutPreferToFillWindow() const;
  void SetLayoutPreferToFillWindow(bool value);

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

  std::shared_ptr<platform::native::INativeWindowResolver>
  GetNativeWindowResolver() {
    return native_window_resolver_;
  }

  bool IsRetainAfterDestroy() { return retain_after_destroy_; }

  void SetRetainAfterDestroy(bool destroy) { retain_after_destroy_ = destroy; }

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

  // See remarks of UiHost.
  bool retain_after_destroy_ = false;
  // See remarks of UiHost.
  bool deleting_ = false;

  // We need this because calling Resolve on resolver in handler of destroy
  // event is bad and will always get the dying window. But we need to label the
  // window as destroyed so the destructor will not destroy native window
  // repeatedly. See remarks of UiHost.
  bool native_window_destroyed_ = false;

  std::vector<EventRevokerGuard> event_revoker_guards_;

  Window* window_control_;
  std::unique_ptr<render::WindowRenderObject> root_render_object_;

  Control* mouse_hover_control_;

  Control* focus_control_;  // "focus_control_" can't be nullptr

  Control* mouse_captured_control_;

  bool layout_prefer_to_fill_window_ = true;
};
}  // namespace cru::ui
