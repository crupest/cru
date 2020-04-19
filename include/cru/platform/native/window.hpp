#pragma once
#include "../resource.hpp"
#include "base.hpp"
#include "cru/common/event.hpp"

#include <string_view>

namespace cru::platform::native {
// Represents a native window, which exposes some low-level events and
// operations.
//
// Usually you save an INativeWindowResolver after creating a window. Because
// window may be destroyed when user do certain actions like click the close
// button. Then the INativeWindow instance is destroyed and
// INativeWindowResolver::Resolve return nullptr to indicate the fact.
struct INativeWindow : virtual INativeResource {
  virtual std::shared_ptr<INativeWindowResolver> GetResolver() = 0;

  virtual void Close() = 0;

  virtual INativeWindow* GetParent() = 0;

  virtual bool IsVisible() = 0;
  virtual void SetVisible(bool is_visible) = 0;

  virtual Size GetClientSize() = 0;
  virtual void SetClientSize(const Size& size) = 0;

  // Get the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  virtual Rect GetWindowRect() = 0;

  // Set the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  virtual void SetWindowRect(const Rect& rect) = 0;

  // Relative to client lefttop.
  virtual Point GetMousePosition() = 0;

  virtual bool CaptureMouse() = 0;
  virtual bool ReleaseMouse() = 0;

  virtual void SetCursor(std::shared_ptr<ICursor> cursor) = 0;

  virtual void RequestRepaint() = 0;

  // Remember to call EndDraw on return value and destroy it.
  virtual std::unique_ptr<graph::IPainter> BeginPaint() = 0;

  virtual IEvent<std::nullptr_t>* DestroyEvent() = 0;
  virtual IEvent<std::nullptr_t>* PaintEvent() = 0;
  virtual IEvent<Size>* ResizeEvent() = 0;
  virtual IEvent<FocusChangeType>* FocusEvent() = 0;
  virtual IEvent<MouseEnterLeaveType>* MouseEnterLeaveEvent() = 0;
  virtual IEvent<Point>* MouseMoveEvent() = 0;
  virtual IEvent<NativeMouseButtonEventArgs>* MouseDownEvent() = 0;
  virtual IEvent<NativeMouseButtonEventArgs>* MouseUpEvent() = 0;
  virtual IEvent<NativeKeyEventArgs>* KeyDownEvent() = 0;
  virtual IEvent<NativeKeyEventArgs>* KeyUpEvent() = 0;
  virtual IEvent<std::string_view>* CharEvent() = 0;
};

// See INativeWindow for more info.
struct INativeWindowResolver : virtual INativeResource {
  // Think twice before you save the return value.
  virtual INativeWindow* Resolve() = 0;
};
}  // namespace cru::platform::native
