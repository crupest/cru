#pragma once
#include "Resource.h"
#include "cru/base/Base.h"
#include "cru/platform/graphics/Base.h"
#include "cru/platform/graphics/Brush.h"

#include <CoreGraphics/CoreGraphics.h>

namespace cru::platform::graphics::quartz {
class QuartzBrush : public OsxQuartzResource, public virtual IBrush {
 public:
  QuartzBrush(IGraphicsFactory* graphics_factory)
      : OsxQuartzResource(graphics_factory) {}
  CRU_DELETE_COPY(QuartzBrush)
  CRU_DELETE_MOVE(QuartzBrush)
  ~QuartzBrush() override = default;

 public:
  virtual void Select(CGContextRef context) = 0;
};

class QuartzSolidColorBrush : public QuartzBrush,
                              public virtual ISolidColorBrush {
 public:
  QuartzSolidColorBrush(IGraphicsFactory* graphics_factory, const Color& color);

  CRU_DELETE_COPY(QuartzSolidColorBrush)
  CRU_DELETE_MOVE(QuartzSolidColorBrush)

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
}  // namespace cru::platform::graphics::quartz
