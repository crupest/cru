#pragma once
#include "cru/common/base.hpp"

#include "basic_types.hpp"
#include "cru/common/event.hpp"
#include "cru/common/ui_base.hpp"

namespace cru::platform {
struct Painter;

struct NativeWindow : public virtual Interface {
  // Return if the window is still valid, that is, hasn't been closed or
  // destroyed.
  virtual bool IsValid() = 0;
  virtual void SetDeleteThisOnDestroy(bool value) = 0;

  virtual void Close() = 0;

  virtual NativeWindow* GetParent() = 0;

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

  virtual Painter* BeginPaint() = 0;

  virtual Event<>* DestroyEvent() = 0;
  virtual Event<ui::Size>* ResizeEvent() = 0;
  virtual Event<>* PaintEvent() = 0;
  virtual Event<bool>* FocusEvent() = 0;
  virtual Event<bool>* MouseEnterLeaveEvent() = 0;
  virtual Event<ui::Point>* MouseMoveEvent() = 0;
  virtual Event<MouseButton, ui::Point>* MouseDownEvent() = 0;
  virtual Event<MouseButton, ui::Point>* MouseUpEvent() = 0;
  virtual Event<int>* KeyDownEvent() = 0;
  virtual Event<int>* KeyUpEvent() = 0;
};
}  // namespace cru::platform
