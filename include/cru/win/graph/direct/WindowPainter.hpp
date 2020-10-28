#pragma once
#include "Painter.hpp"
#include "WindowRenderTarget.hpp"

namespace cru::platform::graph::win::direct {
class D2DWindowPainter : public graph::win::direct::D2DPainter {
 public:
  explicit D2DWindowPainter(D2DWindowRenderTarget* window);

  CRU_DELETE_COPY(D2DWindowPainter)
  CRU_DELETE_MOVE(D2DWindowPainter)

  ~D2DWindowPainter() override;

 protected:
  void DoEndDraw() override;

 private:
  D2DWindowRenderTarget* render_target_;
};
}  // namespace cru::platform::graph::win::direct
