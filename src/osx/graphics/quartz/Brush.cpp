#include "cru/osx/graphics/quartz/Brush.hpp"

namespace cru::platform::graphics::osx::quartz {
QuartzSolidColorBrush::QuartzSolidColorBrush(IGraphFactory* graphics_factory,
                                             const Color& color)
    : QuartzBrush(graphics_factory), color_(color) {
  cg_color_ =
      CGColorCreateGenericRGB(color.GetFloatRed(), color.GetFloatGreen(),
                              color.GetFloatBlue(), color.GetFloatAlpha());
}

QuartzSolidColorBrush::~QuartzSolidColorBrush() { CGColorRelease(cg_color_); }

void QuartzSolidColorBrush::SetColor(const Color& color) {
  color_ = color;
  CGColorRelease(cg_color_);
  cg_color_ =
      CGColorCreateGenericRGB(color.GetFloatRed(), color.GetFloatGreen(),
                              color.GetFloatBlue(), color.GetFloatAlpha());
}

void QuartzSolidColorBrush::Select(CGContextRef context) {
  CGContextSaveGState(context);

  CGContextSetStrokeColorWithColor(context, cg_color_);
  CGContextSetFillColorWithColor(context, cg_color_);

  CGContextRestoreGState(context);
}
}  // namespace cru::platform::graphics::osx::quartz
