#include "Common.h"
#include "cru/platform/Color.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/gui/UiApplication.h"

#include <random>

namespace cru::theme_builder::components {
std::unique_ptr<platform::graphics::ISolidColorBrush>
CreateRandomEditorBackgroundBrush() {
  static float current_hue = 0.0f;
  current_hue += 23.f;
  if (current_hue > 360.f) {
    current_hue -= 360.f;
  }

  return platform::gui::IUiApplication::GetInstance()
      ->GetGraphicsFactory()
      ->CreateSolidColorBrush(platform::HslColor(current_hue, 0.5f, 0.8f));
}

}  // namespace cru::theme_builder::components
