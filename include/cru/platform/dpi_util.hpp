#pragma once
#include "ui_applicaition.hpp"

namespace cru::platform {
inline Dpi GetDpi() {
  return UiApplication::GetInstance()->GetDpi();
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
}  // namespace cru::platform
