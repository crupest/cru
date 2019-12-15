#pragma once
#include "cru/win/graph/direct/painter.hpp"
#include "cru/win/native/window_render_target.hpp"

namespace cru::platform::native::win {
class WindowD2DPainter : public graph::win::direct::D2DPainter {
 public:
  explicit WindowD2DPainter(WindowRenderTarget* window);

  CRU_DELETE_COPY(WindowD2DPainter)
  CRU_DELETE_MOVE(WindowD2DPainter)

  ~WindowD2DPainter() override;

 protected:
  void DoEndDraw() override;

 private:
  WindowRenderTarget* render_target_;
};
}  // namespace cru::platform::native::win
