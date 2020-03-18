#pragma once
#include "../layout_control.hpp"

namespace cru::ui::controls {
class StackLayout : public LayoutControl {
 public:
  static constexpr std::string_view control_type = "StackLayout";

  static StackLayout* Create() { return new StackLayout(); }

 protected:
  StackLayout();

 public:
  CRU_DELETE_COPY(StackLayout)
  CRU_DELETE_MOVE(StackLayout)

  ~StackLayout() override;

  std::string_view GetControlType() const final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

 protected:
  void OnAddChild(Control* child, int position) override;
  void OnRemoveChild(Control* child, int position) override;

 private:
  std::shared_ptr<render::StackLayoutRenderObject> render_object_;
};
}  // namespace cru::ui::controls
