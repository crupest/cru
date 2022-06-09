#include "cru/platform/graphics/cairo/CairoImageFactory.h"
#include <memory>
#include "cru/common/Exception.h"
#include "cru/platform/graphics/cairo/CairoImage.h"
#include "cru/platform/graphics/cairo/CairoResource.h"

namespace cru::platform::graphics::cairo {
CairoImageFactory::CairoImageFactory(CairoGraphicsFactory* factory)
    : CairoResource(factory) {}

CairoImageFactory::~CairoImageFactory() {}

std::unique_ptr<IImage> CairoImageFactory::DecodeFromStream(
    io::Stream* stream) {
  throw Exception(u"Not implemented.");
}

void CairoImageFactory::EncodeToStream(IImage* image, io::Stream* stream,
                                       ImageFormat format, float quality) {
  throw Exception(u"Not implemented.");
}

std::unique_ptr<IImage> CairoImageFactory::CreateBitmap(int width, int height) {
  return std::make_unique<CairoImage>(GetCairoGraphicsFactory(), width, height);
}

}  // namespace cru::platform::graphics::cairo
