#include "Base.h"
#include "cru/platform/Color.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Painter.h"

#include <memory>

int main() {
  CruPlatformGraphicsDemo demo("draw-circle-demo.png", 500, 500);

  auto brush =
      demo.GetFactory()->CreateSolidColorBrush(cru::platform::colors::skyblue);
  demo.GetPainter()->FillEllipse(cru::platform::Rect{200, 200, 100, 100},
                                 brush.get());

  return 0;
}
