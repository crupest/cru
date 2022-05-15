#include "cru/platform/graphics/quartz/Image.h"
#include "cru/common/Exception.h"
#include "cru/platform/graphics/quartz/Convert.h"
#include "cru/platform/graphics/quartz/Painter.h"

namespace cru::platform::graphics::quartz {
QuartzImage::QuartzImage(IGraphicsFactory* graphics_factory,
                         IImageFactory* image_factory, CGImageRef image,
                         bool auto_release, unsigned char* buffer)
    : OsxQuartzResource(graphics_factory),
      image_factory_(image_factory),
      image_(image),
      auto_release_(auto_release),
      buffer_(buffer) {
  Expects(image);
}

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

std::unique_ptr<IPainter> QuartzImage::CreatePainter() {
  if (!buffer_)
    throw Exception(
        u"Failed to create painter for image because failed to get its "
        u"buffer.");

  auto width = CGImageGetWidth(image_);
  auto height = CGImageGetHeight(image_);
  auto bits_per_component = CGImageGetBitsPerComponent(image_);
  auto bytes_per_row = CGImageGetBytesPerRow(image_);
  auto color_space = CGImageGetColorSpace(image_);
  auto bitmap_info = CGImageGetBitmapInfo(image_);

  auto cg_context =
      CGBitmapContextCreate(buffer_, width, height, bits_per_component,
                            bytes_per_row, color_space, bitmap_info);

  return std::make_unique<QuartzCGContextPainter>(
      GetGraphicsFactory(), cg_context, true, Size(width, height),
      [](QuartzCGContextPainter* painter) {

      });
}
}  // namespace cru::platform::graphics::quartz
