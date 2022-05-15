#pragma once
#include "Resource.h"
#include "cru/platform/graphics/Image.h"
#include "cru/platform/graphics/ImageFactory.h"

#include <CoreGraphics/CoreGraphics.h>

namespace cru::platform::graphics::quartz {
class QuartzImage : public OsxQuartzResource, public virtual IImage {
 public:
  QuartzImage(IGraphicsFactory* graphics_factory, IImageFactory* image_factory,
              CGImageRef image, bool auto_release,
              unsigned char* buffer = nullptr);

  CRU_DELETE_COPY(QuartzImage)
  CRU_DELETE_MOVE(QuartzImage)

  ~QuartzImage() override;

 public:
  float GetWidth() override;
  float GetHeight() override;

  std::unique_ptr<IImage> CreateWithRect(const Rect& rect) override;

  std::unique_ptr<IPainter> CreatePainter() override;

  CGImageRef GetCGImage() const { return image_; }

 private:
  IImageFactory* image_factory_;
  CGImageRef image_;
  bool auto_release_ = false;

  unsigned char* buffer_;
};
}  // namespace cru::platform::graphics::quartz
