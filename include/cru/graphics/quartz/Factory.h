#pragma once
#include "Resource.h"
#include <cru/Base.h>
#include "cru/graphics/quartz/ImageFactory.h"
#include "cru/graphics/Factory.h"
#include "cru/graphics/ImageFactory.h"

namespace cru::graphics::quartz {
class QuartzGraphicsFactory : public OsxQuartzResource,
                              public virtual IGraphicsFactory {
 public:
  QuartzGraphicsFactory();


  ~QuartzGraphicsFactory() override;

 public:
  std::unique_ptr<ISolidColorBrush> CreateSolidColorBrush() override;

  std::unique_ptr<IGeometryBuilder> CreateGeometryBuilder() override;

  std::unique_ptr<IFont> CreateFont(String font_family,
                                    float font_size) override;

  std::unique_ptr<ITextLayout> CreateTextLayout(std::shared_ptr<IFont> font,
                                                String text) override;

  IImageFactory* GetImageFactory() override;

 private:
  std::unique_ptr<QuartzImageFactory> image_factory_;
};
}  // namespace cru::graphics::quartz
