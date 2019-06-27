#pragma once
#include "cru/win/graph/direct/painter.hpp"
#include "cru/win/native/native_window.hpp"

namespace cru::platform::native::win {
class WindowD2DPainter : public graph::win::direct::D2DPainter {
 public:
  explicit WindowD2DPainter(WinNativeWindow* window);
  WindowD2DPainter(const WindowD2DPainter& other) = delete;
  WindowD2DPainter& operator=(const WindowD2DPainter& other) = delete;
  WindowD2DPainter(WindowD2DPainter&& other) = delete;
  WindowD2DPainter& operator=(WindowD2DPainter&& other) = delete;
  ~WindowD2DPainter() override;

 protected:
  void DoEndDraw() override;

 private:
  WinNativeWindow* window_;
};
}  // namespace cru::win::native
