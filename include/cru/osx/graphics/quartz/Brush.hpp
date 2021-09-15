#pragma once
#include "Resource.hpp"
#include "cru/common/Base.hpp"
#include "cru/platform/graphics/Base.hpp"
#include "cru/platform/graphics/Brush.hpp"

#include <CoreGraphics/CoreGraphics.h>

#include <functional>

namespace cru::platform::graphics::osx::quartz {
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

 private:
  Color color_;
  CGColorRef cg_color_;
};
}  // namespace cru::platform::graphics::osx::quartz
