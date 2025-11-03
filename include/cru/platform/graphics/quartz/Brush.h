#pragma once
#include "Base.h"

#include <cru/platform/graphics/Brush.h>

#include <CoreGraphics/CoreGraphics.h>

namespace cru::platform::graphics::quartz {
class QuartzBrush : public OsxQuartzResource, public virtual IBrush {
 public:
  QuartzBrush(IGraphicsFactory* graphics_factory)
      : OsxQuartzResource(graphics_factory) {}

 public:
  virtual void Select(CGContextRef context) = 0;
};

class QuartzSolidColorBrush : public QuartzBrush,
                              public virtual ISolidColorBrush {
 public:
  QuartzSolidColorBrush(IGraphicsFactory* graphics_factory, const Color& color);
  ~QuartzSolidColorBrush() override;

  Color GetColor() override { return color_; }
  void SetColor(const Color& color) override;

  CGColorRef GetCGColorRef() const { return cg_color_; }

  void Select(CGContextRef context) override;

  std::string GetDebugString() override;

 private:
  Color color_;
  CGColorRef cg_color_;
};
}  // namespace cru::platform::graphics::quartz
