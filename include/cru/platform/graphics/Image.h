#pragma once
#include "Base.h"

namespace cru::platform::graphics {
struct CRU_PLATFORM_GRAPHICS_API IImage : public virtual IGraphicsResource {
  virtual float GetWidth() = 0;
  virtual float GetHeight() = 0;
  virtual std::unique_ptr<IImage> CreateWithRect(const Rect& rect) = 0;

  /**
   * \brief Create a painter for this image.
   * \remarks Not all image could create a painter. If not this method will
   * throw. Currently we only ensure images returned by
   * IImageFactory::CreateBitmap or CloneToBitmap can create a painter.
   */
  virtual std::unique_ptr<IPainter> CreatePainter() = 0;

  /**
   * \brief Create a bitmap image with the same pixels as this image's.
   * \remarks This method can be used to create a bitmap image, so you can draw
   * on the new bitmap, if the original image can't be directly painted.
   */
  virtual std::unique_ptr<IImage> CloneToBitmap();
};
}  // namespace cru::platform::graphics
