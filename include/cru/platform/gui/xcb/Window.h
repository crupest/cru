
#pragma once
#include "../Window.h"
#include "Base.h"

#include <xcb/xcb.h>

namespace cru::platform::gui::xcb {
class XcbUiApplication;

class XcbWindow : public XcbResource, public virtual INativeWindow {
 public:
  explicit XcbWindow(XcbUiApplication* application);
  ~XcbWindow() override;

  virtual void Close() = 0;

  virtual INativeWindow* GetParent() = 0;
  virtual void SetParent(INativeWindow* parent) = 0;

  virtual WindowStyleFlag GetStyleFlag() = 0;
  virtual void SetStyleFlag(WindowStyleFlag flag) = 0;

  virtual String GetTitle() = 0;
  virtual void SetTitle(String title) = 0;

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

  // Remember to call EndDraw on return value and destroy it.
  virtual std::unique_ptr<graphics::IPainter> BeginPaint() = 0;

  virtual IEvent<std::nullptr_t>* CreateEvent() = 0;
  virtual IEvent<std::nullptr_t>* DestroyEvent() = 0;
  virtual IEvent<std::nullptr_t>* PaintEvent() = 0;

  virtual IEvent<WindowVisibilityType>* VisibilityChangeEvent() = 0;
  virtual IEvent<Size>* ResizeEvent() = 0;
  virtual IEvent<FocusChangeType>* FocusEvent() = 0;

  virtual IEvent<MouseEnterLeaveType>* MouseEnterLeaveEvent() = 0;
  virtual IEvent<Point>* MouseMoveEvent() = 0;
  virtual IEvent<NativeMouseButtonEventArgs>* MouseDownEvent() = 0;
  virtual IEvent<NativeMouseButtonEventArgs>* MouseUpEvent() = 0;
  virtual IEvent<NativeMouseWheelEventArgs>* MouseWheelEvent() = 0;
  virtual IEvent<NativeKeyEventArgs>* KeyDownEvent() = 0;
  virtual IEvent<NativeKeyEventArgs>* KeyUpEvent() = 0;

  virtual IInputMethodContext* GetInputMethodContext() = 0;

 private:
  XcbUiApplication* application_;
  xcb_window_t xcb_window_;
};
}  // namespace cru::platform::gui::xcb
