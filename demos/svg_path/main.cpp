#include "cru/platform/Color.h"
#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Font.h"
#include "cru/platform/graphics/Painter.h"
#include "cru/platform/gui/InputMethod.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"

int main() {
  using namespace cru;
  using namespace cru::platform;
  using namespace cru::platform::graphics;
  using namespace cru::platform::gui;

  IUiApplication* application = bootstrap::CreateUiApplication();

  auto graphics_factory = application->GetGraphicsFactory();

  auto window = application->CreateWindow();

  auto brush = graphics_factory->CreateSolidColorBrush(colors::black);

  auto create_geometry = [graphics_factory](StringView path_d) {
    auto geometry_builder = graphics_factory->CreateGeometryBuilder();
    geometry_builder->ParseAndApplySvgPathData(path_d);
    return geometry_builder->Build();
  };

  auto geometry = create_geometry(
      uR"(
M8.5 5.5a.5.5 0 0 0-1 0v3.362l-1.429 2.38a.5.5 0 1 0 .858.515l1.5-2.5A.5.5 0 0 0 8.5 9V5.5z
M6.5 0a.5.5 0 0 0 0 1H7v1.07a7.001 7.001 0 0 0-3.273 12.474l-.602.602a.5.5 0 0 0 .707.708l.746-.746A6.97 6.97 0 0 0 8 16a6.97 6.97 0 0 0 3.422-.892l.746.746a.5.5 0 0 0 .707-.708l-.601-.602A7.001 7.001 0 0 0 9 2.07V1h.5a.5.5 0 0 0 0-1h-3zm1.038 3.018a6.093 6.093 0 0 1 .924 0 6 6 0 1 1-.924 0zM0 3.5c0 .753.333 1.429.86 1.887A8.035 8.035 0 0 1 4.387 1.86 2.5 2.5 0 0 0 0 3.5zM13.5 1c-.753 0-1.429.333-1.887.86a8.035 8.035 0 0 1 3.527 3.527A2.5 2.5 0 0 0 13.5 1z
  )");

  window->PaintEvent()->AddHandler([&](auto) {
    auto painter = window->BeginPaint();
    painter->PushState();
    painter->ConcatTransform(Matrix::Scale(10, 10));
    painter->FillGeometry(geometry.get(), brush.get());
    painter->PopState();
    painter->EndDraw();
  });

  window->SetVisibility(WindowVisibilityType::Show);

  return application->Run();
}
