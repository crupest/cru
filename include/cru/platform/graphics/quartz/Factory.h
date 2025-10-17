#pragma once
#include "Resource.h"
#include "cru/base/Base.h"
#include "cru/platform/graphics/quartz/ImageFactory.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/ImageFactory.h"

namespace cru::platform::graphics::quartz {
class QuartzGraphicsFactory : public OsxQuartzResource,
                              public virtual IGraphicsFactory {
 public:
  QuartzGraphicsFactory();

  CRU_DELETE_COPY(QuartzGraphicsFactory)
  CRU_DELETE_MOVE(QuartzGraphicsFactory)

  ~QuartzGraphicsFactory() override;

 public:
  std::unique_ptr<ISolidColorBrush> CreateSolidColorBrush() override;

  std::unique_ptr<IGeometryBuilder> CreateGeometryBuilder() override;

  std::unique_ptr<IFont> CreateFont(std::string font_family,
                                    float font_size) override;

  std::unique_ptr<ITextLayout> CreateTextLayout(std::shared_ptr<IFont> font,
                                                std::string text) override;

  IImageFactory* GetImageFactory() override;

 private:
  std::unique_ptr<QuartzImageFactory> image_factory_;
};
}  // namespace cru::platform::graphics::quartz
