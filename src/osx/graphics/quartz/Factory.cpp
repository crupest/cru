#include "cru/osx/graphics/quartz/Factory.hpp"

#include "cru/osx/graphics/quartz/Brush.hpp"

#include <memory>

namespace cru::platform::graphics::osx::quartz {
std::unique_ptr<ISolidColorBrush>
QuartzGraphicsFactory::CreateSolidColorBrush() {
  return std::make_unique<QuartzSolidColorBrush>(this, colors::black);
}
}  // namespace cru::platform::graphics::osx::quartz
