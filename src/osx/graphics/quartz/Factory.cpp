#include "cru/osx/graphics/quartz/Factory.hpp"

#include "cru/osx/graphics/quartz/Brush.hpp"
#include "cru/osx/graphics/quartz/Font.hpp"
#include "cru/osx/graphics/quartz/Geometry.hpp"
#include "cru/osx/graphics/quartz/ImageFactory.hpp"
#include "cru/osx/graphics/quartz/TextLayout.hpp"
#include "cru/platform/Check.hpp"
#include "cru/platform/graphics/ImageFactory.hpp"

#include <memory>

namespace cru::platform::graphics::osx::quartz {
QuartzGraphicsFactory::QuartzGraphicsFactory()
    : OsxQuartzResource(this), image_factory_(new QuartzImageFactory(this)) {}

QuartzGraphicsFactory::~QuartzGraphicsFactory() {}

std::unique_ptr<ISolidColorBrush>
QuartzGraphicsFactory::CreateSolidColorBrush() {
  return std::make_unique<QuartzSolidColorBrush>(this, colors::black);
}

std::unique_ptr<IGeometryBuilder>
QuartzGraphicsFactory::CreateGeometryBuilder() {
  return std::make_unique<QuartzGeometryBuilder>(this);
}

std::unique_ptr<IFont> QuartzGraphicsFactory::CreateFont(String font_family,
                                                         float font_size) {
  return std::make_unique<OsxCTFont>(this, font_family, font_size);
}

std::unique_ptr<ITextLayout> QuartzGraphicsFactory::CreateTextLayout(
    std::shared_ptr<IFont> font, String text) {
  auto f = CheckPlatform<OsxCTFont>(font, GetPlatformId());
  return std::make_unique<OsxCTTextLayout>(this, f, text);
}

IImageFactory* QuartzGraphicsFactory::GetImageFactory() {
  return image_factory_.get();
}
}  // namespace cru::platform::graphics::osx::quartz
