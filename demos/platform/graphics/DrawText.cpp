#include "Base.h"
#include "cru/platform/Color.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Font.h"
#include "cru/platform/graphics/Painter.h"

#include <iostream>
#include <memory>

int main() {
  CruPlatformGraphicsDemo demo("draw-text-demo.png", 500, 200);

  auto brush =
      demo.GetFactory()->CreateSolidColorBrush(cru::platform::colors::skyblue);

  std::shared_ptr<cru::platform::graphics::IFont> font(
      demo.GetFactory()->CreateFont("", 24));
  auto text_layout = demo.GetFactory()->CreateTextLayout(font, "Hello world!");
  demo.GetPainter()->DrawText({0, 0}, text_layout.get(), brush.get());

  auto bounds = text_layout->GetTextBounds();
  std::cout << "Bounds of text:\n\tx: " << bounds.left
            << "\n\ty: " << bounds.top << "\n\twidth: " << bounds.width
            << "\n\theight: " << bounds.height << std::endl;

  return 0;
}
