#pragma once
#include "Control.h"

namespace cru::ui::controls {
template <typename TRenderObject>
class LayoutControl : public Control {
 protected:
  LayoutControl(std::string name) : Control(std::move(name)) {
    container_render_object_.SetAttachedControl(this);
  }

 public:
  using Control::AddChild;
  using Control::InsertChildAt;
  using Control::RemoveChildAt;

  render::RenderObject* GetRenderObject() override {
    return &container_render_object_;
  }

  TRenderObject* GetContainerRenderObject() {
    return &container_render_object_;
  }

  const typename TRenderObject::ChildLayoutData& GetChildLayoutData(
      Index position) {
    return container_render_object_.GetChildLayoutDataAt(position);
  }

  void SetChildLayoutData(Index position,
                          typename TRenderObject::ChildLayoutData data) {
    container_render_object_.SetChildLayoutDataAt(position, data);
  }

 protected:
  void OnChildInserted(Control* control, Index index) override {
    container_render_object_.AddChild(control->GetRenderObject(), index);
  }

  void OnChildRemoved([[maybe_unused]] Control* control, Index index) override {
    container_render_object_.RemoveChild(index);
  }

 private:
  TRenderObject container_render_object_;
};
}  // namespace cru::ui::controls
