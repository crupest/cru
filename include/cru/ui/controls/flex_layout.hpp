#pragma once
#include "../layout_control.hpp"

#include "../render/flex_layout_render_object.hpp"

#include <memory>

namespace cru::ui::controls {
// import these basic entities
using render::FlexChildLayoutData;
using render::FlexCrossAlignment;
using render::FlexDirection;
using render::FlexMainAlignment;

class FlexLayout : public LayoutControl {
 public:
  static constexpr std::string_view control_type = "FlexLayout";

  static FlexLayout* Create() { return new FlexLayout(); }

 protected:
  FlexLayout();

 public:
  FlexLayout(const FlexLayout& other) = delete;
  FlexLayout(FlexLayout&& other) = delete;
  FlexLayout& operator=(const FlexLayout& other) = delete;
  FlexLayout& operator=(FlexLayout&& other) = delete;
  ~FlexLayout() override = default;

  std::string_view GetControlType() const final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

  FlexMainAlignment GetContentMainAlign() const {
    return render_object_->GetContentMainAlign();
  }

  void SetContentMainAlign(FlexMainAlignment value) {
    if (value == GetContentMainAlign()) return;
    render_object_->SetContentMainAlign(value);
  }

  FlexDirection GetFlexDirection() const {
    return render_object_->GetFlexDirection();
  }

  void SetFlexDirection(FlexDirection direction) {
    if (direction == GetFlexDirection()) return;
    render_object_->SetFlexDirection(direction);
  }

  FlexChildLayoutData GetChildLayoutData(Control* control);
  void SetChildLayoutData(Control* control, const FlexChildLayoutData& data);

 protected:
  void OnAddChild(Control* child, int position) override;
  void OnRemoveChild(Control* child, int position) override;

 private:
  std::shared_ptr<render::FlexLayoutRenderObject> render_object_;
};
}  // namespace cru::ui::controls
