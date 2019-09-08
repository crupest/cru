#pragma once
#include "content_control.hpp"

#include "cru/common/self_resolvable.hpp"
#include "cru/platform/native/native_event.hpp"
#include "event/ui_event.hpp"

#include <memory>
#include <vector>

namespace cru::platform::native {
class NativeWindow;
}

namespace cru::ui {
namespace render {
class WindowRenderObject;
}

class Window final : public ContentControl, public SelfResolvable<Window> {
  friend class Control;

 public:
  static constexpr auto control_type = L"Window";

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
  std::wstring_view GetControlType() const override final;

  render::RenderObject* GetRenderObject() const override;

  platform::native::NativeWindow* GetNativeWindow() const {
    return native_window_;
  }

  // Get current control that mouse hovers on. This ignores the mouse-capture
  // control. Even when mouse is captured by another control, this function
  // return the control under cursor. You can use `GetMouseCaptureControl` to
  // get more info.
  Control* GetMouseHoverControl() const { return mouse_hover_control_; }

  //*************** region: layout ***************
  void Relayout();

  void InvalidateLayout();

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

  void OnNativeDestroy(std::nullptr_t);
  void OnNativePaint(std::nullptr_t);
  void OnNativeResize(const Size& size);

  void OnNativeFocus(bool focus);

  void OnNativeMouseEnterLeave(bool enter);
  void OnNativeMouseMove(const Point& point);
  void OnNativeMouseDown(
      const platform::native::NativeMouseButtonEventArgs& args);
  void OnNativeMouseUp(
      const platform::native::NativeMouseButtonEventArgs& args);

  void OnNativeKeyDown(int virtual_code);
  void OnNativeKeyUp(int virtual_code);

  //*************** region: event dispatcher helper ***************

  void DispatchMouseHoverControlChangeEvent(Control* old_control,
                                            Control* new_control,
                                            const Point& point, bool no_leave,
                                            bool no_enter);

  void UpdateCursor();

 private:
  platform::native::NativeWindow* native_window_;
  std::vector<EventRevokerGuard> event_revoker_guards_;

  std::shared_ptr<render::WindowRenderObject> render_object_;

  Control* mouse_hover_control_;

  Control* focus_control_;  // "focus_control_" can't be nullptr

  Control* mouse_captured_control_;

  bool need_layout_ = false;
};
}  // namespace cru::ui
