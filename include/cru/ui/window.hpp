#pragma once
#include "content_control.hpp"

#include "cru/common/self_resolvable.hpp"

namespace cru::ui {
// TODO: Make Window able to be invalid and handle operations in invalidity
// situation.
class Window final : public ContentControl, public SelfResolvable<Window> {
  friend class Control;

 public:
  static constexpr std::string_view control_type = "Window";

 public:
  static Window* CreateOverlapped();

 private:
  struct tag_overlapped_constructor {};

  explicit Window(tag_overlapped_constructor);

 public:
  Window(const Window& other) = delete;
  Window(Window&& other) = delete;
  Window& operator=(const Window& other) = delete;
  Window& operator=(Window&& other) = delete;
  ~Window() override;

 public:
  std::string_view GetControlType() const final;

  render::RenderObject* GetRenderObject() const override;

  platform::native::INativeWindow* ResolveNativeWindow();

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

 protected:
  void OnChildChanged(Control* old_child, Control* new_child) override;

 private:
  Control* HitTest(const Point& point);

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
                       int virtual_code);
  void OnNativeKeyUp(platform::native::INativeWindow* window, int virtual_code);
  void OnNativeChar(platform::native::INativeWindow* window, std::string c);

  //*************** region: event dispatcher helper ***************

  void DispatchMouseHoverControlChangeEvent(Control* old_control,
                                            Control* new_control,
                                            const Point& point, bool no_leave,
                                            bool no_enter);

  void UpdateCursor();

 private:
  std::shared_ptr<platform::native::INativeWindowResolver>
      native_window_resolver_;

  std::vector<EventRevokerGuard> event_revoker_guards_;

  std::unique_ptr<render::WindowRenderObject> render_object_;

  Control* mouse_hover_control_;

  Control* focus_control_;  // "focus_control_" can't be nullptr

  Control* mouse_captured_control_;

  bool need_layout_ = false;
};
}  // namespace cru::ui
