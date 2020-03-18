#pragma once
#include "cru/platform/native/base.hpp"

// The dpi awareness needs to be implemented in the future. Currently we use 96
// as default.

namespace cru::platform::native::win {
inline platform::native::Dpi GetDpi() {
  return platform::native::Dpi{96.0f, 96.0f};
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
}  // namespace cru::platform::native::win
