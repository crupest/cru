#pragma once
#include "Image.h"
#include "Resource.h"
#include "cru/common/io/Stream.h"

namespace cru::platform::graphics {
struct CRU_PLATFORM_GRAPHICS_API IImageFactory
    : public virtual IGraphicsResource {
  virtual std::unique_ptr<IImage> DecodeFromStream(io::Stream* stream) = 0;
};
}  // namespace cru::platform::graphics
