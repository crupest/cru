#include "cru/platform/Color.hpp"
#include "cru/platform/bootstrap/Bootstrap.hpp"
#include "cru/platform/graphics/Factory.hpp"
#include "cru/platform/graphics/Painter.hpp"
#include "cru/platform/gui/UiApplication.hpp"
#include "cru/platform/gui/Window.hpp"

#include <functional>

void BresenhamDrawLine(int x1, int y1, int x2, int y2,
                       const std::function<void(int, int)>& draw_pixel) {
  // calculating range for line between start and end point
  int dx = x2 - x1;
  int dy = y2 - y1;

  int x = x1;
  int y = y1;

  // this is the case when slope(m) < 1
  if (abs(dx) > abs(dy)) {
    draw_pixel(x, y);  // this putpixel is for very first pixel of the line
    int pk = (2 * abs(dy)) - abs(dx);

    for (int i = 0; i < abs(dx); i++) {
      x = x + 1;
      if (pk < 0)
        pk = pk + (2 * abs(dy));
      else {
        y = y + 1;
        pk = pk + (2 * abs(dy)) - (2 * abs(dx));
      }
      draw_pixel(x, y);
    }
  } else {
    // this is the case when slope is greater than or equal to 1  i.e: m>=1
    draw_pixel(x, y);  // this putpixel is for very first pixel of the line
    int pk = (2 * abs(dx)) - abs(dy);

    for (int i = 0; i < abs(dy); i++) {
      y = y + 1;
      if (pk < 0)
        pk = pk + (2 * abs(dx));
      else {
        x = x + 1;
        pk = pk + (2 * abs(dx)) - (2 * abs(dy));
      }

      draw_pixel(x, y);  // display pixel at coordinate (x, y)
    }
  }
}

int main() {
  auto application = cru::platform::bootstrap::CreateUiApplication();
  auto window = application->CreateWindow();

  auto brush = application->GetGraphicsFactory()->CreateSolidColorBrush(
      cru::platform::colors::black);

  window->SetClientSize(cru::platform::Size(400, 200));

  window->PaintEvent()->AddHandler([window, &brush](nullptr_t) {
    auto painter = window->BeginPaint();
    auto draw_pixel = [&painter, &brush](int x, int y) {
      painter->FillRectangle({static_cast<float>(x) - 0.5f,
                              static_cast<float>(y) - 0.5f, 1.0f, 1.0f},
                             brush.get());
    };
    BresenhamDrawLine(50, 50, 100, 200, draw_pixel);
    BresenhamDrawLine(50, 50, 200, 100, draw_pixel);
  });

  window->SetVisibility(cru::platform::gui::WindowVisibilityType::Show);

  return application->Run();
}
