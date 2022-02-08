#pragma once
#include "Painter.h"
#include "WindowRenderTarget.h"

namespace cru::platform::graphics::win::direct {
class CRU_WIN_GRAPHICS_DIRECT_API D2DWindowPainter
    : public graphics::win::direct::D2DDeviceContextPainter {
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
}  // namespace cru::platform::graphics::win::direct
