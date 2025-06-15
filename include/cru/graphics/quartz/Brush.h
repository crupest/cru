#pragma once
#include "Resource.h"
#include <cru/Base.h>
#include "cru/graphics/Base.h"
#include "cru/graphics/Brush.h"

#include <CoreGraphics/CoreGraphics.h>

namespace cru::graphics::quartz {
class QuartzBrush : public OsxQuartzResource, public virtual IBrush {
 public:
  QuartzBrush(IGraphicsFactory* graphics_factory)
      : OsxQuartzResource(graphics_factory) {}
  ~QuartzBrush() override = default;

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

  String GetDebugString() override;

 private:
  Color color_;
  CGColorRef cg_color_;
};
}  // namespace cru::graphics::quartz
