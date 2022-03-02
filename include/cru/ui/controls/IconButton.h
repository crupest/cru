#pragma once
#include "NoChildControl.h"

#include "../helper/ClickDetector.h"
#include "../render/BorderRenderObject.h"
#include "../render/GeometryRenderObject.h"
#include "IBorderControl.h"
#include "IClickableControl.h"
#include "cru/common/Event.h"

namespace cru::ui::controls {
class CRU_UI_API IconButton : public NoChildControl,
                              public virtual IClickableControl,
                              public virtual IBorderControl {
 public:
  static constexpr StringView kControlType = u"IconButton";

 public:
  IconButton();
  IconButton(StringView icon_svg_path_data_string, const Rect& view_port);
  ~IconButton() override;

  String GetControlType() const final { return kControlType.ToString(); }

  render::RenderObject* GetRenderObject() const override {
    return container_render_object_.get();
  }

 public:
  helper::ClickState GetClickState() override {
    return click_detector_.GetState();
  }

  IEvent<helper::ClickState>* ClickStateChangeEvent() override {
    return click_detector_.StateChangeEvent();
  }

  IEvent<helper::ClickEventArgs>* ClickEvent() {
    return click_detector_.ClickEvent();
  }

  void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) override {
    container_render_object_->ApplyBorderStyle(style);
  }

  std::shared_ptr<platform::graphics::IGeometry> GetIconGeometry() const {
    return geometry_render_object_->GetGeometry();
  }
  void SetIconGeometry(std::shared_ptr<platform::graphics::IGeometry> geometry,
                       std::optional<Rect> view_port = std::nullopt) {
    geometry_render_object_->SetGeometry(std::move(geometry), view_port);
  }

  Rect GetIconViewPort() const {
    return geometry_render_object_->GetViewPort();
  }
  void SetIconViewPort(const Rect& view_port) {
    geometry_render_object_->SetViewPort(view_port);
  }

  std::shared_ptr<platform::graphics::IBrush> GetIconFillBrush() const {
    return geometry_render_object_->GetFillBrush();
  }
  void SetIconFillBrush(std::shared_ptr<platform::graphics::IBrush> brush) {
    geometry_render_object_->SetFillBrush(std::move(brush));
  }

  std::shared_ptr<platform::graphics::IBrush> GetIconStrokeBrush() const {
    return geometry_render_object_->GetStrokeBrush();
  }
  void SetIconStrokeBrush(std::shared_ptr<platform::graphics::IBrush> brush) {
    geometry_render_object_->SetStrokeBrush(std::move(brush));
  }

  float GetIconStrokeWidth() const {
    return geometry_render_object_->GetStrokeWidth();
  }
  void SetIconStrokeWidth(float width) {
    geometry_render_object_->SetStrokeWidth(width);
  }

  void SetIconFillColor(const Color& color);
  void SetIconWithSvgPathDataString(StringView icon_svg_path_data_string,
                                    const Rect& view_port);
  void SetIconWithSvgPathDataStringResourceKey(
      StringView icon_svg_path_data_string_resource_key, const Rect& view_port);

 private:
  std::unique_ptr<render::BorderRenderObject> container_render_object_;
  std::unique_ptr<render::GeometryRenderObject> geometry_render_object_;
  helper::ClickDetector click_detector_;
};
}  // namespace cru::ui::controls
