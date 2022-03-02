#include "cru/ui/controls/IconButton.h"

#include "../Helper.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/Geometry.h"
#include "cru/ui/ThemeManager.h"

namespace cru::ui::controls {
IconButton::IconButton()
    : container_render_object_(new render::BorderRenderObject()),
      geometry_render_object_(new render::GeometryRenderObject()),
      click_detector_(this) {
  container_render_object_->SetChild(geometry_render_object_.get());
  container_render_object_->SetAttachedControl(this);
  geometry_render_object_->SetAttachedControl(this);

  container_render_object_->SetBorderEnabled(true);
  GetStyleRuleSet()->SetParent(
      ThemeManager::GetInstance()->GetResourceStyleRuleSet(
          u"icon-button.style"));
}

IconButton::IconButton(StringView icon_svg_path_data_string,
                       const Rect& view_port)
    : IconButton() {
  SetIconWithSvgPathDataString(icon_svg_path_data_string, view_port);
}

IconButton::~IconButton() {}

void IconButton::SetIconFillColor(const Color& color) {
  SetIconFillBrush(GetGraphicsFactory()->CreateSolidColorBrush(color));
}

void IconButton::SetIconWithSvgPathDataString(
    StringView icon_svg_path_data_string, const Rect& view_port) {
  SetIconGeometry(platform::graphics::CreateGeometryFromSvgPathData(
                      GetGraphicsFactory(), icon_svg_path_data_string),
                  view_port);
}

void IconButton::SetIconWithSvgPathDataStringResourceKey(
    StringView icon_svg_path_data_string_resource_key, const Rect& view_port) {
  SetIconWithSvgPathDataString(
      ThemeManager::GetInstance()->GetResourceString(
          icon_svg_path_data_string_resource_key.ToString()),
      view_port);
}

}  // namespace cru::ui::controls
