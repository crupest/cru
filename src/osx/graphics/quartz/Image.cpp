#include "cru/osx/graphics/quartz/Image.hpp"

namespace cru::platform::graphics::osx::quartz {
QuartzImage::QuartzImage(IGraphicsFactory* graphics_factory,
                         IImageFactory* image_factory, CGImageRef image,
                         bool auto_release)
    : OsxQuartzResource(graphics_factory),
      image_(image),
      auto_release_(auto_release) {}

QuartzImage::~QuartzImage() {
  if (auto_release_) {
    CGImageRelease(image_);
  }
}
}  // namespace cru::platform::graphics::osx::quartz
