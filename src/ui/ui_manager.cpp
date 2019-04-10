#include "cru/ui/ui_manager.hpp"

#include "cru/platform/graph/brush.hpp"
#include "cru/platform/graph/font.hpp"
#include "cru/platform/graph/graph_factory.hpp"
#include "cru/platform/native/ui_applicaition.hpp"

namespace cru::ui {
PredefineResources::PredefineResources() {
  const auto graph_factory = platform::graph::GraphFactory::GetInstance();

  button_normal_border_brush.reset(static_cast<platform::graph::Brush*>(
      graph_factory->CreateSolidColorBrush(colors::black)));
  text_block_selection_brush.reset(static_cast<platform::graph::Brush*>(
      graph_factory->CreateSolidColorBrush(colors::skyblue)));
  text_block_text_brush.reset(static_cast<platform::graph::Brush*>(
      graph_factory->CreateSolidColorBrush(colors::black)));
  text_block_font.reset(graph_factory->CreateFontDescriptor(L"等线", 24.0f));
}

UiManager* UiManager::GetInstance() {
  static UiManager instance;
  return &instance;
}

UiManager::UiManager() : predefine_resources_(new PredefineResources()) {}
}  // namespace cru::ui
