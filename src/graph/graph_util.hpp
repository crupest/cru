#pragma once
#include "pre.hpp"

#include <d2d1_1.h>
#include <functional>

#include "exception.hpp"
#include "graph_manager.hpp"

namespace cru::graph {
struct Dpi {
  float x;
  float y;
};

inline Dpi GetDpi() {
  Dpi dpi;
  GraphManager::GetInstance()->GetD2D1Factory()->GetDesktopDpi(&dpi.x, &dpi.y);
  return dpi;
}

inline int DipToPixelInternal(const float dip, const float dpi) {
  return static_cast<int>(dip * dpi / 96.0f);
}

inline int DipToPixelX(const float dip_x) {
  return DipToPixelInternal(dip_x, GetDpi().x);
}

inline int DipToPixelY(const float dip_y) {
  return DipToPixelInternal(dip_y, GetDpi().y);
}

inline float DipToPixelInternal(const int pixel, const float dpi) {
  return static_cast<float>(pixel) * 96.0f / dpi;
}

inline float PixelToDipX(const int pixel_x) {
  return DipToPixelInternal(pixel_x, GetDpi().x);
}

inline float PixelToDipY(const int pixel_y) {
  return DipToPixelInternal(pixel_y, GetDpi().y);
}

inline void WithTransform(
    ID2D1RenderTarget* render_target, const D2D1_MATRIX_3X2_F matrix,
    const std::function<void(ID2D1RenderTarget*)>& action) {
  D2D1_MATRIX_3X2_F old_transform;
  render_target->GetTransform(&old_transform);
  render_target->SetTransform(old_transform * matrix);
  action(render_target);
  render_target->SetTransform(old_transform);
}

inline ID2D1SolidColorBrush* CreateSolidColorBrush(const D2D1_COLOR_F& color) {
  ID2D1SolidColorBrush* brush;
  ThrowIfFailed(GraphManager::GetInstance()
                    ->GetD2D1DeviceContext()
                    ->CreateSolidColorBrush(color, &brush));
  return brush;
}
}  // namespace cru::graph
