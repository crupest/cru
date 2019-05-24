#pragma once
#include "cru/win/graph/win_painter.hpp"
#include "cru/win/native/win_native_window.hpp"

namespace cru::win::native {
class WindowPainter : public graph::WinPainter {
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
