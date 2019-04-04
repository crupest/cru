#pragma once
#include "content_control.hpp"

#include "event/ui_event.hpp"

#include <memory>

namespace cru::platform {
struct NativeWindow;
}

namespace cru::ui {
namespace render {
class WindowRenderObject;
}

class Window final : public ContentControl {
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

  platform::NativeWindow* GetNativeWindow() const { return native_window_; }

  Control* GetMouseHoverControl() const { return mouse_hover_control_; }

  //*************** region: focus ***************

  // Request focus for specified control.
  bool RequestFocusFor(Control* control);

  // Get the control that has focus.
  Control* GetFocusControl();

 protected:
  void OnChildChanged(Control* old_child, Control* new_child) override;

 private:
  Control* HitTest(const Point& point);

  //*************** region: native messages ***************

  void OnNativeDestroy();
  void OnNativePaint();
  void OnNativeResize(const Size& size);

  void OnNativeFocus(bool focus);

  void OnNativeMouseEnterLeave(bool enter);
  void OnNativeMouseMove(const Point& point);
  void OnNativeMouseDown(platform::MouseButton button, const Point& point);
  void OnNativeMouseUp(platform::MouseButton button, const Point& point);

  void OnNativeKeyDown(int virtual_code);
  void OnNativeKeyUp(int virtual_code);


  //*************** region: event dispatcher helper ***************

  void DispatchMouseHoverControlChangeEvent(Control* old_control,
                                            Control* new_control,
                                            const Point& point);

 private:
  platform::NativeWindow* native_window_;
  std::vector<EventHandlerRevoker> event_revokers_;

  std::shared_ptr<render::WindowRenderObject> render_object_;

  Control* mouse_hover_control_ = nullptr;

  Control* focus_control_ = this;  // "focus_control_" can't be nullptr
};
}  // namespace cru::ui
