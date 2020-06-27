#pragma once
#include "../LayoutControl.hpp"

namespace cru::ui::controls {
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
  ~FlexLayout() override;

  std::string_view GetControlType() const final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

  FlexMainAlignment GetContentMainAlign() const;
  void SetContentMainAlign(FlexMainAlignment value);

  FlexDirection GetFlexDirection() const;
  void SetFlexDirection(FlexDirection direction);

  FlexChildLayoutData GetChildLayoutData(Control* control);
  void SetChildLayoutData(Control* control, FlexChildLayoutData data);

 protected:
  void OnAddChild(Control* child, Index position) override;
  void OnRemoveChild(Control* child, Index position) override;

 private:
  std::shared_ptr<render::FlexLayoutRenderObject> render_object_;
};
}  // namespace cru::ui::controls
