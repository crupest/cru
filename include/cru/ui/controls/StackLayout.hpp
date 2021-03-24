#pragma once
#include "LayoutControl.hpp"

namespace cru::ui::controls {
class StackLayout : public LayoutControl {
 public:
  static constexpr std::u16string_view control_type = u"StackLayout";

  static StackLayout* Create() { return new StackLayout(); }

 protected:
  StackLayout();

 public:
  CRU_DELETE_COPY(StackLayout)
  CRU_DELETE_MOVE(StackLayout)

  ~StackLayout() override;

  std::u16string_view GetControlType() const final { return control_type; }

  render::RenderObject* GetRenderObject() const override;

 private:
  std::shared_ptr<render::StackLayoutRenderObject> render_object_;
};
}  // namespace cru::ui::controls
