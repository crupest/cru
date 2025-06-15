#include "cru/graphics/cairo/CairoGraphicsFactory.h"

#include "DrawCircle.h"

int main() {
  std::unique_ptr<cru::graphics::IGraphicsFactory> graphics_factory(
      new cru::graphics::cairo::CairoGraphicsFactory());

  cru::demos::graphics::DrawCircle(graphics_factory.get());

  return 0;
}
