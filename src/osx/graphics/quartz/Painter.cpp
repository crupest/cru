#include "cru/osx/graphics/quartz/Painter.hpp"

#include "cru/osx/graphics/quartz/Convert.hpp"

namespace cru::platform::graphics::osx::quartz {
  Matrix QuartzCGContextPainter::GetTransform() {
    return Convert(CGContextGetCTM(cg_context_));
  }
  
  void QuartzCGContextPainter::SetTransform(const Matrix& matrix)  {
    auto old = CGContextGetCTM(cg_context_);
    old = CGAffineTransformInvert(old);
    CGContextConcatCTM(cg_context_, old);
    CGContextConcatCTM(cg_context_, Convert(matrix));
  }
}
