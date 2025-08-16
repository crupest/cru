#include "cru/platform/graphics/quartz/ImageFactory.h"
#include "cru/base/Exception.h"
#include "cru/base/Osx.h"
#include "cru/platform/Check.h"
#include "cru/platform/graphics/Image.h"
#include "cru/platform/graphics/quartz/Convert.h"
#include "cru/platform/graphics/quartz/Image.h"

#include <ImageIO/ImageIO.h>

namespace cru::platform::graphics::quartz {
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

static String GetImageFormatUniformTypeIdentifier(ImageFormat format) {
  switch (format) {
    case ImageFormat::Png:
      return u"public.png";
    case ImageFormat::Jpeg:
      return u"public.jpeg";
    case ImageFormat::Gif:
      return u"com.compuserve.gif";
    default:
      throw Exception(u"Unknown image format.");
  }
}

void QuartzImageFactory::EncodeToStream(IImage* image, io::Stream* stream,
                                        ImageFormat format, float quality) {
  if (quality <= 0 || quality > 1) {
    throw Exception(u"Invalid quality value.");
  }

  auto quartz_image = CheckPlatform<QuartzImage>(image, GetPlatformId());
  auto cg_image = quartz_image->GetCGImage();

  auto uti = ToCFStringRef(GetImageFormatUniformTypeIdentifier(format));
  CGDataConsumerRef data_consumer = ConvertStreamToCGDataConsumer(stream);
  CGImageDestinationRef destination = CGImageDestinationCreateWithDataConsumer(
      data_consumer, uti.ref, 1, nullptr);

  CFMutableDictionaryRef properties =
      CFDictionaryCreateMutable(nullptr, 0, nullptr, nullptr);
  CFNumberRef quality_wrap =
      CFNumberCreate(nullptr, kCFNumberFloatType, &quality);
  CFDictionaryAddValue(properties, kCGImageDestinationLossyCompressionQuality,
                       quality_wrap);

  CGImageDestinationAddImage(destination, cg_image, properties);

  if (!CGImageDestinationFinalize(destination)) {
    throw Exception(u"Failed to finalize image destination.");
  }

  CFRelease(quality_wrap);
  CFRelease(properties);
  CFRelease(destination);
  CFRelease(data_consumer);
}

std::unique_ptr<IImage> QuartzImageFactory::CreateBitmap(int width,
                                                         int height) {
  if (width <= 0) throw Exception(u"Image width should be greater than 0.");
  if (height <= 0) throw Exception(u"Image height should be greater than 0.");

  CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();

  const auto buffer_size = width * height * 4;
  auto buffer = new unsigned char[buffer_size]{0};

  auto cg_data_provider = CGDataProviderCreateWithData(
      nullptr, buffer, buffer_size,
      [](void* info, const void* data, size_t size) {
        delete[] static_cast<const unsigned char*>(data);
      });

  auto cg_image =
      CGImageCreate(width, height, 8, 32, 4 * width, color_space,
                    kCGImageAlphaPremultipliedLast, cg_data_provider, nullptr,
                    true, kCGRenderingIntentDefault);

  CGColorSpaceRelease(color_space);
  CGDataProviderRelease(cg_data_provider);

  return std::unique_ptr<IImage>(
      new QuartzImage(GetGraphicsFactory(), this, cg_image, true, buffer));
}
}  // namespace cru::platform::graphics::quartz
