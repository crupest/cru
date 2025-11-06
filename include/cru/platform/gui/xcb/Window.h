
#pragma once
#include "Base.h"

#include <cru/platform/gui/TimerHelper.h>
#include <cru/platform/gui/Window.h>

#include <cairo.h>
#include <xcb/xcb.h>
#include <cstddef>
#include <optional>

namespace cru::platform::gui::xcb {
class XcbUiApplication;
class XcbCursor;
class XcbXimInputMethodContext;

class XcbWindow : public XcbResource, public virtual INativeWindow {
  CRU_DEFINE_CLASS_LOG_TAG("cru::platform::gui::xcb::XcbWindow")

  friend XcbUiApplication;

 public:
  explicit XcbWindow(XcbUiApplication* application);
  ~XcbWindow() override;

  bool IsCreated() override;
  void Close() override;

  INativeWindow* GetParent() override;
  void SetParent(INativeWindow* parent) override;

  WindowStyleFlag GetStyleFlag() override;
  void SetStyleFlag(WindowStyleFlag flag) override;

  std::string GetTitle() override;
  void SetTitle(std::string title) override;

  WindowVisibilityType GetVisibility() override;
  void SetVisibility(WindowVisibilityType visibility) override;

  Size GetClientSize() override;
  void SetClientSize(const Size& size) override;

  Rect GetClientRect() override;
  void SetClientRect(const Rect& rect) override;

  Rect GetWindowRect() override;
  void SetWindowRect(const Rect& rect) override;

  bool RequestFocus() override;

  Point GetMousePosition() override;

  bool CaptureMouse() override;
  bool ReleaseMouse() override;

  void SetCursor(std::shared_ptr<ICursor> cursor) override;

  void SetToForeground() override;

  void RequestRepaint() override;

  std::unique_ptr<graphics::IPainter> BeginPaint() override;

  IEvent<std::nullptr_t>* CreateEvent() override;
  IEvent<std::nullptr_t>* DestroyEvent() override;
  IEvent<std::nullptr_t>* PaintEvent() override;

  IEvent<WindowVisibilityType>* VisibilityChangeEvent() override;
  IEvent<const Size&>* ResizeEvent() override;
  IEvent<FocusChangeType>* FocusEvent() override;

  IEvent<MouseEnterLeaveType>* MouseEnterLeaveEvent() override;
  IEvent<const Point&>* MouseMoveEvent() override;
  IEvent<const NativeMouseButtonEventArgs&>* MouseDownEvent() override;
  IEvent<const NativeMouseButtonEventArgs&>* MouseUpEvent() override;
  IEvent<const NativeMouseWheelEventArgs&>* MouseWheelEvent() override;
  IEvent<const NativeKeyEventArgs&>* KeyDownEvent() override;
  IEvent<const NativeKeyEventArgs&>* KeyUpEvent() override;

  IInputMethodContext* GetInputMethodContext() override;

 public:
  std::optional<xcb_window_t> GetXcbWindow();
  XcbUiApplication* GetXcbUiApplication();
  bool HasFocus();

 private:
  xcb_window_t DoCreateWindow();
  void HandleEvent(xcb_generic_event_t* event);
  static std::optional<xcb_window_t> GetEventWindow(xcb_generic_event_t* event);

  void DoSetParent(xcb_window_t window);
  void DoSetStyleFlags(xcb_window_t window);
  void DoSetTitle(xcb_window_t window);
  void DoSetClientRect(xcb_window_t window, const Rect& rect);
  void DoSetCursor(xcb_window_t window, XcbCursor* cursor);

  void* XcbGetProperty(xcb_window_t window, xcb_atom_t property,
                       xcb_atom_t type, std::uint32_t offset,
                       std::uint32_t length,
                       std::uint32_t* out_length = nullptr);

  // Relative to screen lefttop.
  Point GetXcbWindowPosition(xcb_window_t window);

  std::optional<Thickness> Get_NET_FRAME_EXTENTS(xcb_window_t window);

 private:
  XcbUiApplication* application_;
  std::optional<xcb_window_t> xcb_window_;
  cairo_surface_t* cairo_surface_;
  Size current_size_;
  WindowStyleFlag style_;
  std::string title_;
  bool mapped_;
  std::shared_ptr<XcbCursor> cursor_;

  XcbWindow* parent_;
  XcbXimInputMethodContext* input_method_;

  TimerAutoCanceler repaint_canceler_;

  Event<std::nullptr_t> create_event_;
  Event<std::nullptr_t> destroy_event_;
  Event<std::nullptr_t> paint_event_;

  Event<WindowVisibilityType> visibility_change_event_;
  Event<const Size&> resize_event_;
  Event<FocusChangeType> focus_event_;
  Event<MouseEnterLeaveType> mouse_enter_leave_event_;
  Event<const Point&> mouse_move_event_;
  Event<const NativeMouseButtonEventArgs&> mouse_down_event_;
  Event<const NativeMouseButtonEventArgs&> mouse_up_event_;
  Event<const NativeMouseWheelEventArgs&> mouse_wheel_event_;
  Event<const NativeKeyEventArgs&> key_down_event_;
  Event<const NativeKeyEventArgs&> key_up_event_;
};
}  // namespace cru::platform::gui::xcb
