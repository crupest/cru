#include "cru/ui/controls/IconButton.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Geometry.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/ThemeManager.h"

namespace cru::ui::controls {
IconButton::IconButton() : Control(kControlName), click_detector_(this) {
  container_render_object_.SetChild(&geometry_render_object_);
  container_render_object_.SetAttachedControl(this);
  geometry_render_object_.SetAttachedControl(this);

  container_render_object_.SetBorderEnabled(true);
  GetStyleRuleSet()->SetParent(
      ThemeManager::GetInstance()->GetResourceStyleRuleSet(
          "icon-button.style"));
}

IconButton::IconButton(std::string_view icon_svg_path_data_string,
                       const Rect& view_port)
    : IconButton() {
  SetIconWithSvgPathDataString(icon_svg_path_data_string, view_port);
}

IconButton::~IconButton() {}

void IconButton::SetIconFillColor(const Color& color) {
  SetIconFillBrush(platform::gui::IUiApplication::GetInstance()
                       ->GetGraphicsFactory()
                       ->CreateSolidColorBrush(color));
}

void IconButton::SetIconWithSvgPathDataString(
    std::string_view icon_svg_path_data_string, const Rect& view_port) {
  SetIconGeometry(
      platform::graphics::CreateGeometryFromSvgPathData(
          platform::gui::IUiApplication::GetInstance()->GetGraphicsFactory(),
          icon_svg_path_data_string),
      view_port);
}

void IconButton::SetIconWithSvgPathDataStringResourceKey(
    std::string_view icon_svg_path_data_string_resource_key,
    const Rect& view_port) {
  SetIconWithSvgPathDataString(ThemeManager::GetInstance()->GetResourceString(
                                   icon_svg_path_data_string_resource_key),
                               view_port);
}

}  // namespace cru::ui::controls
