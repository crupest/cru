#pragma once
#include "cru/common/base.hpp"
#include "cru/common/ui_base.hpp"

namespace cru::platform {
struct Painter;

struct NativeWindow : public virtual Interface {
  virtual bool IsValid() = 0;

  virtual void Close() = 0;

  virtual NativeWindow* GetParent() = 0;

  virtual bool IsVisible() const = 0;
  virtual void SetVisible(bool is_visible) = 0;

  virtual ui::Size GetClientSize() = 0;
  virtual void SetClientSize(const ui::Size& size) = 0;

  // Get the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  virtual ui::Rect GetWindowRect() = 0;

  // Set the rect of the window containing frame.
  // The lefttop of the rect is relative to screen lefttop.
  virtual void SetWindowRect(const ui::Rect& rect) = 0;

  virtual Painter* GetPainter() = 0;
};
}  // namespace cru::platform
