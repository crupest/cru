#include "cru/platform/graphics/quartz/Factory.h"

#include "cru/platform/graphics/quartz/Brush.h"
#include "cru/platform/graphics/quartz/Font.h"
#include "cru/platform/graphics/quartz/Geometry.h"
#include "cru/platform/graphics/quartz/ImageFactory.h"
#include "cru/platform/graphics/quartz/TextLayout.h"

#include <memory>

namespace cru::platform::graphics::quartz {
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

std::unique_ptr<IFont> QuartzGraphicsFactory::CreateFont(
    std::string font_family, float font_size) {
  return std::make_unique<OsxCTFont>(this, font_family, font_size);
}

std::unique_ptr<ITextLayout> QuartzGraphicsFactory::CreateTextLayout(
    std::shared_ptr<IFont> font, std::string text) {
  auto f = CheckPlatform<OsxCTFont>(font, GetPlatformId());
  return std::make_unique<OsxCTTextLayout>(this, f, text);
}

IImageFactory* QuartzGraphicsFactory::GetImageFactory() {
  return image_factory_.get();
}
}  // namespace cru::platform::graphics::quartz
