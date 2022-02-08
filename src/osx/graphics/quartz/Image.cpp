#include "cru/osx/graphics/quartz/Image.h"
#include "cru/osx/graphics/quartz/Convert.h"

namespace cru::platform::graphics::osx::quartz {
QuartzImage::QuartzImage(IGraphicsFactory* graphics_factory,
                         IImageFactory* image_factory, CGImageRef image,
                         bool auto_release)
    : OsxQuartzResource(graphics_factory),
      image_factory_(image_factory),
      image_(image),
      auto_release_(auto_release) {}

QuartzImage::~QuartzImage() {
  if (auto_release_) {
    CGImageRelease(image_);
  }
}

float QuartzImage::GetWidth() { return CGImageGetWidth(image_); }

float QuartzImage::GetHeight() { return CGImageGetHeight(image_); }

std::unique_ptr<IImage> QuartzImage::CreateWithRect(const Rect& rect) {
  auto new_cg_image = CGImageCreateWithImageInRect(image_, Convert(rect));

  return std::make_unique<QuartzImage>(GetGraphicsFactory(), image_factory_,
                                       new_cg_image, true);
}
}  // namespace cru::platform::graphics::osx::quartz
