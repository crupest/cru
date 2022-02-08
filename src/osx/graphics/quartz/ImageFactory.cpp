#include "cru/osx/graphics/quartz/ImageFactory.h"
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

  QuartzImage* image =
      new QuartzImage(GetGraphicsFactory(), this, cg_image, true);

  CFRelease(cg_image);
  CGDataProviderRelease(data_provider);

  return std::unique_ptr<IImage>(image);
}
}  // namespace cru::platform::graphics::osx::quartz
