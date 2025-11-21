#pragma once
#include "../helper/ClickDetector.h"
#include "../render/BorderRenderObject.h"
#include "../render/GeometryRenderObject.h"
#include "Control.h"
#include "IBorderControl.h"
#include "IClickableControl.h"
#include "IContentBrushControl.h"

#include <cru/base/Event.h>
#include <cru/platform/graphics/Brush.h>

namespace cru::ui::controls {
class CRU_UI_API IconButton : public Control,
                              public virtual IClickableControl,
                              public virtual IBorderControl,
                              public virtual IContentBrushControl {
 public:
  static constexpr auto kControlName = "IconButton";

 public:
  IconButton();
  IconButton(std::string_view icon_svg_path_data_string, const Rect& view_port);
  ~IconButton() override;

  render::RenderObject* GetRenderObject() override {
    return &container_render_object_;
  }

 public:
  helper::ClickState GetClickState() override {
    return click_detector_.GetState();
  }

  IEvent<helper::ClickState>* ClickStateChangeEvent() override {
    return click_detector_.StateChangeEvent();
  }

  IEvent<const helper::ClickEventArgs&>* ClickEvent() override {
    return click_detector_.ClickEvent();
  }

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override {
    container_render_object_.ApplyBorderStyle(style);
  }

  std::shared_ptr<platform::graphics::IGeometry> GetIconGeometry() {
    return geometry_render_object_.GetGeometry();
  }
  void SetIconGeometry(std::shared_ptr<platform::graphics::IGeometry> geometry,
                       std::optional<Rect> view_port = std::nullopt) {
    geometry_render_object_.SetGeometry(std::move(geometry), view_port);
  }

  Rect GetIconViewPort() { return geometry_render_object_.GetViewPort(); }
  void SetIconViewPort(const Rect& view_port) {
    geometry_render_object_.SetViewPort(view_port);
  }

  std::shared_ptr<platform::graphics::IBrush> GetIconFillBrush() {
    return geometry_render_object_.GetFillBrush();
  }
  void SetIconFillBrush(std::shared_ptr<platform::graphics::IBrush> brush) {
    geometry_render_object_.SetFillBrush(std::move(brush));
  }

  std::shared_ptr<platform::graphics::IBrush> GetIconStrokeBrush() {
    return geometry_render_object_.GetStrokeBrush();
  }
  void SetIconStrokeBrush(std::shared_ptr<platform::graphics::IBrush> brush) {
    geometry_render_object_.SetStrokeBrush(std::move(brush));
  }

  float GetIconStrokeWidth() {
    return geometry_render_object_.GetStrokeWidth();
  }
  void SetIconStrokeWidth(float width) {
    geometry_render_object_.SetStrokeWidth(width);
  }

  void SetIconFillColor(const Color& color);
  void SetIconWithSvgPathDataString(std::string_view icon_svg_path_data_string,
                                    const Rect& view_port);
  void SetIconWithSvgPathDataStringResourceKey(
      std::string_view icon_svg_path_data_string_resource_key,
      const Rect& view_port);

  std::shared_ptr<platform::graphics::IBrush> GetContentBrush() override {
    return GetIconFillBrush();
  }

  void SetContentBrush(
      std::shared_ptr<platform::graphics::IBrush> brush) override {
    SetIconFillBrush(std::move(brush));
  }

 private:
  render::BorderRenderObject container_render_object_;
  render::GeometryRenderObject geometry_render_object_;
  helper::ClickDetector click_detector_;
};
}  // namespace cru::ui::controls
