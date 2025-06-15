#pragma once
#include "Painter.h"
#include "WindowRenderTarget.h"

namespace cru::graphics::direct2d {
class CRU_WIN_GRAPHICS_DIRECT_API D2DWindowPainter
    : public graphics::direct2d::D2DDeviceContextPainter {
 public:
  explicit D2DWindowPainter(D2DWindowRenderTarget* window);


  ~D2DWindowPainter() override;

 protected:
  void DoEndDraw() override;

 private:
  D2DWindowRenderTarget* render_target_;
};
}  // namespace cru::graphics::direct2d
