#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"

#include "DrawCircle.h"

int main() {
  std::unique_ptr<cru::platform::graphics::IGraphicsFactory> graphics_factory(
      new cru::platform::graphics::cairo::CairoGraphicsFactory());

  cru::demos::graphics::DrawCircle(graphics_factory.get());

  return 0;
}
