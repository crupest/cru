#include "cru/win/graphics/direct/Image.hpp"

namespace cru::platform::graphics::win::direct {
Direct2DImage::Direct2DImage(DirectGraphicsFactory* graphics_factory,
                             ID2D1Image* d2d_image, bool auto_release)
    : DirectGraphicsResource(graphics_factory),
      d2d_image_(d2d_image),
      auto_release_(auto_release) {}

Direct2DImage::~Direct2DImage() {
  if (auto_release_) {
    d2d_image_->Release();
  }
}
}  // namespace cru::platform::graphics::win::direct
