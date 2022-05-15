#include "cru/platform/graphics/quartz/Brush.h"
#include "cru/common/String.h"
#include "cru/common/Format.h"

namespace cru::platform::graphics::quartz {
QuartzSolidColorBrush::QuartzSolidColorBrush(IGraphicsFactory* graphics_factory,
                                             const Color& color)
    : QuartzBrush(graphics_factory), color_(color) {
  cg_color_ =
      CGColorCreateGenericRGB(color.GetFloatRed(), color.GetFloatGreen(),
                              color.GetFloatBlue(), color.GetFloatAlpha());
  Ensures(cg_color_);
}

QuartzSolidColorBrush::~QuartzSolidColorBrush() { CGColorRelease(cg_color_); }

void QuartzSolidColorBrush::SetColor(const Color& color) {
  color_ = color;
  CGColorRelease(cg_color_);
  cg_color_ =
      CGColorCreateGenericRGB(color.GetFloatRed(), color.GetFloatGreen(),
                              color.GetFloatBlue(), color.GetFloatAlpha());
  Ensures(cg_color_);
}

void QuartzSolidColorBrush::Select(CGContextRef context) {
  Expects(context);
  Expects(cg_color_);
  CGContextSetStrokeColorWithColor(context, cg_color_);
  CGContextSetFillColorWithColor(context, cg_color_);
}

String QuartzSolidColorBrush::GetDebugString() {
  return Format(u"QuartzSolidColorBrush(Color: {})", color_);
}
}  // namespace cru::platform::graphics::quartz
