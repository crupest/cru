
#pragma once
#include "../Window.h"
#include "Base.h"

#include <cairo.h>
#include <xcb/xcb.h>
#include <cstddef>
#include <optional>

namespace cru::platform::gui::xcb {
class XcbUiApplication;

class XcbWindow : public XcbResource, public virtual INativeWindow {
  friend XcbUiApplication;

 public:
  explicit XcbWindow(XcbUiApplication* application);
  ~XcbWindow() override;

  void Close() override;

  INativeWindow* GetParent() override;
  void SetParent(INativeWindow* parent) override;

  WindowStyleFlag GetStyleFlag() override;
  void SetStyleFlag(WindowStyleFlag flag) override;

  String GetTitle() override;
  void SetTitle(String title) override;

  virtual WindowVisibilityType GetVisibility() = 0;
  virtual void SetVisibility(WindowVisibilityType visibility) = 0;

  virtual Size GetClientSize() = 0;
  virtual void SetClientSize(const Size& size) = 0;

  virtual Rect GetClientRect() = 0;
  virtual void SetClientRect(const Rect& rect) = 0;

  // Get the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  virtual Rect GetWindowRect() = 0;

  // Set the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  virtual void SetWindowRect(const Rect& rect) = 0;

  virtual bool RequestFocus() = 0;

  // Relative to client lefttop.
  virtual Point GetMousePosition() = 0;

  virtual bool CaptureMouse() = 0;
  virtual bool ReleaseMouse() = 0;

  virtual void SetCursor(std::shared_ptr<ICursor> cursor) = 0;

  virtual void SetToForeground() = 0;

  virtual void RequestRepaint() = 0;

  std::unique_ptr<graphics::IPainter> BeginPaint() override;

  IEvent<std::nullptr_t>* CreateEvent() override;
  IEvent<std::nullptr_t>* DestroyEvent() override;
  IEvent<std::nullptr_t>* PaintEvent() override;

  IEvent<WindowVisibilityType>* VisibilityChangeEvent() override;
  IEvent<Size>* ResizeEvent() override;
  IEvent<FocusChangeType>* FocusEvent() override;

  IEvent<MouseEnterLeaveType>* MouseEnterLeaveEvent() override;
  IEvent<Point>* MouseMoveEvent() override;
  IEvent<NativeMouseButtonEventArgs>* MouseDownEvent() override;
  IEvent<NativeMouseButtonEventArgs>* MouseUpEvent() override;
  IEvent<NativeMouseWheelEventArgs>* MouseWheelEvent() override;
  IEvent<NativeKeyEventArgs>* KeyDownEvent() override;
  IEvent<NativeKeyEventArgs>* KeyUpEvent() override;

  virtual IInputMethodContext* GetInputMethodContext() = 0;

 public:
  std::optional<xcb_window_t> GetXcbWindow();

 private:
  xcb_window_t DoCreateWindow();
  void HandleEvent(xcb_generic_event_t* event);
  static std::optional<xcb_window_t> GetEventWindow(xcb_generic_event_t* event);

  void DoSetParent(xcb_window_t window);
  void DoSetStyleFlags(xcb_window_t window);
  void DoSetTitle(xcb_window_t window);

 private:
  XcbUiApplication* application_;
  std::optional<xcb_window_t> xcb_window_;
  cairo_surface_t* cairo_surface_;
  Size current_size_;
  WindowStyleFlag style_;
  std::string title_;

  XcbWindow* parent_;

  Event<std::nullptr_t> create_event_;
  Event<std::nullptr_t> destroy_event_;
  Event<std::nullptr_t> paint_event_;

  Event<WindowVisibilityType> visibility_change_event_;
  Event<Size> resize_event_;
  Event<FocusChangeType> focus_event_;
  Event<MouseEnterLeaveType> mouse_enter_leave_event_;
  Event<Point> mouse_move_event_;
  Event<NativeMouseButtonEventArgs> mouse_down_event_;
  Event<NativeMouseButtonEventArgs> mouse_up_event_;
  Event<NativeMouseWheelEventArgs> mouse_wheel_event_;
  Event<NativeKeyEventArgs> key_down_event_;
  Event<NativeKeyEventArgs> key_up_event_;
};
}  // namespace cru::platform::gui::xcb
