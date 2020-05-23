#include "cru/ui/UiManager.hpp"

#include "cru/platform/graph/Brush.hpp"
#include "cru/platform/graph/Factory.hpp"
#include "cru/platform/graph/Font.hpp"
#include "cru/platform/native/UiApplication.hpp"
#include "Helper.hpp"

namespace cru::ui {
using namespace cru::platform::graph;

namespace {
std::unique_ptr<ISolidColorBrush> CreateSolidColorBrush(IGraphFactory* factory,
                                                        const Color& color) {
  auto brush = factory->CreateSolidColorBrush();
  brush->SetColor(color);
  return brush;
}
}  // namespace

UiManager* UiManager::GetInstance() {
  static UiManager* instance = new UiManager();
  GetUiApplication()->AddOnQuitHandler([] {
    delete instance;
    instance = nullptr;
  });
  return instance;
}

UiManager::UiManager() {
  const auto factory = GetGraphFactory();

  theme_resource_.default_font = factory->CreateFont("等线", 24.0f);

  const auto black_brush = std::shared_ptr<platform::graph::ISolidColorBrush>(
      CreateSolidColorBrush(factory, colors::black));
  theme_resource_.text_brush = black_brush;
  theme_resource_.text_selection_brush =
      CreateSolidColorBrush(factory, colors::skyblue);
  theme_resource_.caret_brush = black_brush;

  theme_resource_.button_style.normal.border_brush =
      CreateSolidColorBrush(factory, Color::FromHex(0x00bfff));
  theme_resource_.button_style.hover.border_brush =
      CreateSolidColorBrush(factory, Color::FromHex(0x47d1ff));
  theme_resource_.button_style.press.border_brush =
      CreateSolidColorBrush(factory, Color::FromHex(0x91e4ff));
  theme_resource_.button_style.press_cancel.border_brush =
      CreateSolidColorBrush(factory, Color::FromHex(0x91e4ff));

  theme_resource_.button_style.normal.border_thickness =
      theme_resource_.button_style.hover.border_thickness =
          theme_resource_.button_style.press.border_thickness =
              theme_resource_.button_style.press_cancel.border_thickness =
                  Thickness(3);

  theme_resource_.button_style.normal.border_radius =
      theme_resource_.button_style.hover.border_radius =
          theme_resource_.button_style.press.border_radius =
              theme_resource_.button_style.press_cancel.border_radius =
                  CornerRadius({5, 5});

  theme_resource_.text_box_border_style.normal.border_brush =
      CreateSolidColorBrush(factory, Color::FromHex(0xced4da));
  theme_resource_.text_box_border_style.normal.border_radius = CornerRadius(5);
  theme_resource_.text_box_border_style.normal.border_thickness = Thickness(1);

  theme_resource_.text_box_border_style.hover =
      theme_resource_.text_box_border_style.normal;

  theme_resource_.text_box_border_style.normal.border_brush =
      CreateSolidColorBrush(factory, Color::FromHex(0x495057));
  theme_resource_.text_box_border_style.normal.border_radius = CornerRadius(5);
  theme_resource_.text_box_border_style.normal.border_thickness = Thickness(1);

  theme_resource_.text_box_border_style.focus_hover =
      theme_resource_.text_box_border_style.focus;
}

UiManager::~UiManager() = default;
}  // namespace cru::ui
