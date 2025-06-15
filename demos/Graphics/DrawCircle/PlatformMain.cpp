#include "cru/bootstrap/Bootstrap.h"

#include "DrawCircle.h"

int main() {
  std::unique_ptr<cru::graphics::IGraphicsFactory> graphics_factory(
      cru::platform::bootstrap::CreateGraphicsFactory());

  cru::demos::graphics::DrawCircle(graphics_factory.get());

  return 0;
}
