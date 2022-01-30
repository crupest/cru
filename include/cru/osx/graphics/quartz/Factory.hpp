#pragma once
#include "Resource.hpp"
#include "cru/common/Base.hpp"
#include "cru/osx/graphics/quartz/ImageFactory.hpp"
#include "cru/platform/graphics/Factory.hpp"
#include "cru/platform/graphics/ImageFactory.hpp"

namespace cru::platform::graphics::osx::quartz {
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

  std::unique_ptr<IFont> CreateFont(String font_family,
                                    float font_size) override;

  std::unique_ptr<ITextLayout> CreateTextLayout(std::shared_ptr<IFont> font,
                                                String text) override;

  IImageFactory* GetImageFactory() override;

 private:
  std::unique_ptr<QuartzImageFactory> image_factory_;
};
}  // namespace cru::platform::graphics::osx::quartz
