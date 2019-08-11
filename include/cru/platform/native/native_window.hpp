#pragma once
#include "../native_resource.hpp"

#include "../graphic_base.hpp"
#include "basic_types.hpp"
#include "cru/common/event.hpp"
#include "native_event.hpp"

namespace cru::platform::graph {
class Painter;
}

namespace cru::platform::native {
// Represents a native window, which exposes some low-level events and
// operations.
//
// Although you can always retain an instance of this class, the real window
// associated with it might be have already been destroyed by explicitly calling
// Close or closed by the user, which leads to an invalid instance. You can
// check the validity by IsValid. When you call perform native operations on the
// invalid instance, there is no effect.
class NativeWindow : public NativeResource {
 protected:
  NativeWindow() = default;

 public:
  NativeWindow(const NativeWindow& other) = delete;
  NativeWindow& operator=(const NativeWindow& other) = delete;

  NativeWindow(NativeWindow&& other) = delete;
  NativeWindow& operator=(NativeWindow&& other) = delete;

  ~NativeWindow() override = default;

 public:
  // Return if the window is still valid, that is, hasn't been closed or
  // destroyed.
  virtual bool IsValid() = 0;
  virtual void SetDeleteThisOnDestroy(bool value) = 0;

  virtual void Close() = 0;

  virtual NativeWindow* GetParent() = 0;

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

  virtual void Repaint() = 0;
  virtual graph::Painter* BeginPaint() = 0;

  virtual IEvent<std::nullptr_t>* DestroyEvent() = 0;
  virtual IEvent<std::nullptr_t>* PaintEvent() = 0;
  virtual IEvent<Size>* ResizeEvent() = 0;
  virtual IEvent<bool>* FocusEvent() = 0;
  virtual IEvent<bool>* MouseEnterLeaveEvent() = 0;
  virtual IEvent<Point>* MouseMoveEvent() = 0;
  virtual IEvent<NativeMouseButtonEventArgs>* MouseDownEvent() = 0;
  virtual IEvent<NativeMouseButtonEventArgs>* MouseUpEvent() = 0;
  virtual IEvent<int>* KeyDownEvent() = 0;
  virtual IEvent<int>* KeyUpEvent() = 0;
};
}  // namespace cru::platform::native
