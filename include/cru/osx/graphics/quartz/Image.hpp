#pragma once
#include "Resource.hpp"
#include "cru/platform/graphics/Image.hpp"
#include "cru/platform/graphics/ImageFactory.hpp"

#include <CoreGraphics/CoreGraphics.h>

namespace cru::platform::graphics::osx::quartz {
class QuartzImage : public OsxQuartzResource, public virtual IImage {
 public:
  QuartzImage(IGraphicsFactory* graphics_factory, IImageFactory* image_factory,
              CGImageRef image, bool auto_release);

  CRU_DELETE_COPY(QuartzImage)
  CRU_DELETE_MOVE(QuartzImage)

  ~QuartzImage() override;

 public:
  float GetWidth() override;
  float GetHeight() override;

  std::unique_ptr<IImage> CreateWithRect(const Rect& rect) override;

  CGImageRef GetCGImage() const { return image_; }

 private:
  IImageFactory* image_factory_;
  CGImageRef image_;
  bool auto_release_ = false;
};
}  // namespace cru::platform::graphics::osx::quartz
