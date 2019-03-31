#pragma once
#include "pre.hpp"

#include <Windows.h>
#include <map>
#include <memory>

#include "content_control.hpp"
#include "events/ui_event.hpp"
#include "events/window_event.hpp"
#include "window_class.hpp"

namespace cru::graph {
class WindowRenderTarget;
}

namespace cru::ui::render {
class WindowRenderObject;
}

namespace cru::ui {


class Window final : public ContentControl {
  friend class WindowManager;

 public:
  static constexpr auto control_type = L"Window";

 public:
  static Window* CreateOverlapped();
  static Window* CreatePopup(Window* parent, bool caption = false);

 private:
  struct tag_overlapped_constructor {};
  struct tag_popup_constructor {};

  explicit Window(tag_overlapped_constructor);
  Window(tag_popup_constructor, Window* parent, bool caption);

  void BeforeCreateHwnd();
  void AfterCreateHwnd(WindowManager* window_manager);

 public:
  Window(const Window& other) = delete;
  Window(Window&& other) = delete;
  Window& operator=(const Window& other) = delete;
  Window& operator=(Window&& other) = delete;
  ~Window() override;

 public:
  StringView GetControlType() const override final;

  render::RenderObject* GetRenderObject() const override;




  //*************** region: window operations ***************

  // Get the client size.
  Size GetClientSize();

  // Set the client size and repaint.
  void SetClientSize(const Size& size);

  // Get the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  Rect GetWindowRect();

  // Set the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  void SetWindowRect(const Rect& rect);

  // Set the lefttop of the window relative to screen.
  void SetWindowPosition(const Point& position);

  Point PointToScreen(const Point& point);

  Point PointFromScreen(const Point& point);

  // Handle the raw window message.
  // Return true if the message is handled and get the result through "result"
  // argument. Return false if the message is not handled.
  bool HandleWindowMessage(HWND hwnd, int msg, WPARAM w_param, LPARAM l_param,
                           LRESULT& result);

  //*************** region: mouse ***************

  Point GetMousePosition();

  Control* GetMouseHoverControl() const { return mouse_hover_control_; }

  //*************** region: focus ***************

  // Request focus for specified control.
  bool RequestFocusFor(Control* control);

  // Get the control that has focus.
  Control* GetFocusControl();

  //*************** region: mouse capture ***************

  Control* CaptureMouseFor(Control* control);
  Control* ReleaseCurrentMouseCapture();

  //*************** region: events ***************
 public:
  Event<events::UiEventArgs>* ActivatedEvent() { return &activated_event_; }
  Event<events::UiEventArgs>* DeactivatedEvent() { return &deactivated_event_; }
  Event<events::WindowNativeMessageEventArgs>* NativeMessageEvent() {
    return &native_message_event_;
  }

 private:
  Event<events::UiEventArgs> activated_event_;
  Event<events::UiEventArgs> deactivated_event_;
  Event<events::WindowNativeMessageEventArgs> native_message_event_;

 protected:
  void OnChildChanged(Control* old_child, Control* new_child) override;

 private:
  Control* HitTest(const Point& point);

  //*************** region: native operations ***************

  // Get the client rect in pixel.
  RECT GetClientRectPixel();

  bool IsMessageInQueue(UINT message);

  void SetCursorInternal(HCURSOR cursor);

  //*************** region: native messages ***************

  void OnDestroyInternal();
  void OnPaintInternal();
  void OnResizeInternal(int new_width, int new_height);

  void OnSetFocusInternal();
  void OnKillFocusInternal();

  void OnMouseMoveInternal(POINT point);
  void OnMouseLeaveInternal();
  void OnMouseDownInternal(MouseButton button, POINT point);
  void OnMouseUpInternal(MouseButton button, POINT point);

  void OnMouseWheelInternal(short delta, POINT point);
  void OnKeyDownInternal(int virtual_code);
  void OnKeyUpInternal(int virtual_code);
  void OnCharInternal(wchar_t c);

  void OnActivatedInternal();
  void OnDeactivatedInternal();

  //*************** region: event dispatcher helper ***************

  void DispatchMouseHoverControlChangeEvent(Control* old_control,
                                            Control* new_control,
                                            const Point& point);

 private:
  std::shared_ptr<graph::WindowRenderTarget> render_target_{};
  std::shared_ptr<render::WindowRenderObject> render_object_{};

  Control* mouse_hover_control_ = nullptr;

  bool window_focus_ = false;
  Control* focus_control_ = this;  // "focus_control_" can't be nullptr
  Control* mouse_capture_control_ = nullptr;
};
}  // namespace cru::ui
