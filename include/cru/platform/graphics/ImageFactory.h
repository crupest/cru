#pragma once
#include "Image.h"
#include "Resource.h"
#include "cru/common/io/Stream.h"

namespace cru::platform::graphics {
struct CRU_PLATFORM_GRAPHICS_API IImageFactory
    : public virtual IGraphicsResource {
  virtual std::unique_ptr<IImage> DecodeFromStream(io::Stream* stream) = 0;

  /**
   * \brief Create an empty bitmap with given width and height.
   * \remarks Implementation should ensure that the bitmap supports alpha
   * channel. It had better be in 32-bit rgba format.
   */
  virtual std::unique_ptr<IImage> CreateBitmap(int width, int height) = 0;
};
}  // namespace cru::platform::graphics
