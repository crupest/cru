#pragma once
#include "cru/win/graph/d2d_painter.hpp"
#include "cru/win/native/win_native_window.hpp"

namespace cru::win::native {
class WindowPainter : public graph::D2DPainter {
 public:
  explicit WindowPainter(WinNativeWindow* window);
  WindowPainter(const WindowPainter& other) = delete;
  WindowPainter& operator=(const WindowPainter& other) = delete;
  WindowPainter(WindowPainter&& other) = delete;
  WindowPainter& operator=(WindowPainter&& other) = delete;
  ~WindowPainter() override;

 protected:
  void DoEndDraw() override;

 private:
  WinNativeWindow* window_;
};
}  // namespace cru::win::native
