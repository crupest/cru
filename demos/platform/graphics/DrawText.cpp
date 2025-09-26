#include "Base.h"
#include "cru/platform/Color.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Font.h"
#include "cru/platform/graphics/Painter.h"

#include <memory>

int main() {
  CruPlatformGraphicsDemo demo("draw-text-demo.png", 500, 200);

  auto brush =
      demo.GetFactory()->CreateSolidColorBrush(cru::platform::colors::skyblue);

  std::shared_ptr<cru::platform::graphics::IFont> font(
      demo.GetFactory()->CreateFont(u"", 24));
  auto text_layout = demo.GetFactory()->CreateTextLayout(font, u"Hello world!");
  demo.GetPainter()->DrawText({0, 0}, text_layout.get(), brush.get());

  return 0;
}
