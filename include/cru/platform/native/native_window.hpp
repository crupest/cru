#pragma once
#include "cru/common/base.hpp"

#include "basic_types.hpp"
#include "cru/common/event.hpp"
#include "cru/common/ui_base.hpp"
#include "native_event.hpp"

namespace cru::platform::graph {
struct IPainter;
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
struct INativeWindow : public virtual Interface {
  // Return if the window is still valid, that is, hasn't been closed or
  // destroyed.
  virtual bool IsValid() = 0;
  virtual void SetDeleteThisOnDestroy(bool value) = 0;

  virtual void Close() = 0;

  virtual INativeWindow* GetParent() = 0;

  virtual bool IsVisible() = 0;
  virtual void SetVisible(bool is_visible) = 0;

  virtual ui::Size GetClientSize() = 0;
  virtual void SetClientSize(const ui::Size& size) = 0;

  // Get the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  virtual ui::Rect GetWindowRect() = 0;

  // Set the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  virtual void SetWindowRect(const ui::Rect& rect) = 0;

  virtual graph::IPainter* BeginPaint() = 0;

  virtual IEvent<std::nullptr_t>* DestroyEvent() = 0;
  virtual IEvent<std::nullptr_t>* PaintEvent() = 0;
  virtual IEvent<ui::Size>* ResizeEvent() = 0;
  virtual IEvent<bool>* FocusEvent() = 0;
  virtual IEvent<bool>* MouseEnterLeaveEvent() = 0;
  virtual IEvent<ui::Point>* MouseMoveEvent() = 0;
  virtual IEvent<NativeMouseButtonEventArgs>* MouseDownEvent() = 0;
  virtual IEvent<NativeMouseButtonEventArgs>* MouseUpEvent() = 0;
  virtual IEvent<int>* KeyDownEvent() = 0;
  virtual IEvent<int>* KeyUpEvent() = 0;
};
}  // namespace cru::platform::native
