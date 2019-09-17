#include "cru/ui/ui_manager.hpp"

#include "cru/platform/graph/brush.hpp"
#include "cru/platform/graph/font.hpp"
#include "cru/platform/graph/graph_factory.hpp"
#include "cru/platform/native/ui_application.hpp"

namespace cru::ui {
using namespace cru::platform::graph;
PredefineResources::PredefineResources() {
  const auto graph_factory = GraphFactory::GetInstance();

  button_normal_border_brush.reset(
      static_cast<Brush*>(graph_factory->CreateSolidColorBrush(colors::black)));
  text_block_selection_brush.reset(static_cast<Brush*>(
      graph_factory->CreateSolidColorBrush(colors::skyblue)));
  text_block_text_brush.reset(
      static_cast<Brush*>(graph_factory->CreateSolidColorBrush(colors::black)));
  text_block_font.reset(graph_factory->CreateFont(L"等线", 24.0f));
}

UiManager* UiManager::GetInstance() {
  static UiManager* instance = new UiManager();
  platform::native::UiApplication::GetInstance()->AddOnQuitHandler([] {
    delete instance;
    instance = nullptr;
  });
  return instance;
}

UiManager::UiManager() : predefine_resources_(new PredefineResources()) {
  const auto factory = GraphFactory::GetInstance();
  theme_resource_.button_style.normal.border_brush = std::shared_ptr<platform::graph::Brush>(
      factory->CreateSolidColorBrush(Color::FromHex(0x00bfff)));
  theme_resource_.button_style.hover.border_brush = std::shared_ptr<platform::graph::Brush>(
      factory->CreateSolidColorBrush(Color::FromHex(0x47d1ff)));
  theme_resource_.button_style.press.border_brush = std::shared_ptr<platform::graph::Brush>(
      factory->CreateSolidColorBrush(Color::FromHex(0x91e4ff)));
  theme_resource_.button_style.press_cancel.border_brush = std::shared_ptr<platform::graph::Brush>(
      factory->CreateSolidColorBrush(Color::FromHex(0x91e4ff)));

  theme_resource_.button_style.normal.border_thickness =
      theme_resource_.button_style.hover.border_thickness =
          theme_resource_.button_style.press.border_thickness =
              theme_resource_.button_style.press_cancel.border_thickness =
                  Thickness(3);

  theme_resource_.button_style.normal.border_radius =
      theme_resource_.button_style.hover.border_radius =
          theme_resource_.button_style.press.border_radius =
              theme_resource_.button_style.press_cancel.border_radius =
                  controls::CornerRadius({5, 5});
}
}  // namespace cru::ui
