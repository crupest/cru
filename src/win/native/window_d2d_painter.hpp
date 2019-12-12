#pragma once
#include "cru/win/graph/direct/painter.hpp"
#include "cru/win/native/window.hpp"

namespace cru::platform::native::win {
class WindowD2DPainter : public graph::win::direct::D2DPainter {
 public:
  explicit WindowD2DPainter(WinNativeWindow* window);
  CRU_DELETE_COPY(WindowD2DPainter)
  CRU_DELETE_MOVE(WindowD2DPainter)
  ~WindowD2DPainter() override;

 protected:
  void DoEndDraw() override;

 private:
  WinNativeWindow* window_;
};
}  // namespace cru::platform::native::win
