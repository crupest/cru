#pragma once
#include "LayoutControl.hpp"

namespace cru::ui::controls {
class CRU_UI_API StackLayout : public LayoutControl {
 public:
  static constexpr StringView control_type = u"StackLayout";

  static StackLayout* Create() { return new StackLayout(); }

 protected:
  StackLayout();

 public:
  CRU_DELETE_COPY(StackLayout)
  CRU_DELETE_MOVE(StackLayout)

  ~StackLayout() override;

  String GetControlType() const final { return control_type.ToString(); }

  render::RenderObject* GetRenderObject() const override;

 private:
  std::shared_ptr<render::StackLayoutRenderObject> render_object_;
};
}  // namespace cru::ui::controls
