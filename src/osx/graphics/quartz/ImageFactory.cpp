#include "cru/osx/graphics/quartz/ImageFactory.h"
#include "cru/common/Exception.h"
#include "cru/osx/graphics/quartz/Convert.h"
#include "cru/osx/graphics/quartz/Image.h"
#include "cru/platform/graphics/Image.h"

#include <ImageIO/ImageIO.h>

namespace cru::platform::graphics::osx::quartz {
QuartzImageFactory::QuartzImageFactory(IGraphicsFactory* graphics_factory)
    : OsxQuartzResource(graphics_factory) {}

QuartzImageFactory::~QuartzImageFactory() {}

std::unique_ptr<IImage> QuartzImageFactory::DecodeFromStream(
    io::Stream* stream) {
  CGDataProviderRef data_provider = ConvertStreamToCGDataProvider(stream);
  CGImageSourceRef image_source =
      CGImageSourceCreateWithDataProvider(data_provider, nullptr);

  CGImageRef cg_image =
      CGImageSourceCreateImageAtIndex(image_source, 0, nullptr);

  CFRelease(image_source);
  CGDataProviderRelease(data_provider);

  return std::unique_ptr<IImage>(
      new QuartzImage(GetGraphicsFactory(), this, cg_image, true));
}

std::unique_ptr<IImage> QuartzImageFactory::CreateBitmap(int width,
                                                         int height) {
  if (width <= 0) throw Exception(u"Image width should be greater than 0.");
  if (height <= 0) throw Exception(u"Image height should be greater than 0.");

  CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();

  auto cg_image = CGImageCreate(width, height, 8, 32, 4 * width, color_space,
                                kCGImageAlphaLast, nullptr, nullptr, true,
                                kCGRenderingIntentDefault);

  CGColorSpaceRelease(color_space);

  return std::unique_ptr<IImage>(
      new QuartzImage(GetGraphicsFactory(), this, cg_image, true));
}
}  // namespace cru::platform::graphics::osx::quartz
