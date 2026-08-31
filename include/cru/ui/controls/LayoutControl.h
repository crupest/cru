#pragma once
#include "Control.h"

namespace cru::ui::controls {
template <typename TRenderObject>
class LayoutControl : public Control {
 protected:
  LayoutControl(std::string name)
      : Control(std::move(name), &container_render_object_) {
    container_render_object_.SetAttachedControl(this);
  }

 public:
  using Control::AddChild;
  using Control::InsertChildAt;
  using Control::RemoveChildAt;

  TRenderObject* GetContainerRenderObject() {
    return &container_render_object_;
  }

  const typename TRenderObject::ChildLayoutData& GetChildLayoutData(
      Index position) {
    return container_render_object_.GetChildLayoutDataAt(position);
  }

  void SetChildLayoutDataAt(Index position,
                            typename TRenderObject::ChildLayoutData data) {
    container_render_object_.SetChildLayoutDataAt(position, data);
  }

  void SetChildLayoutData(Control* child,
                          typename TRenderObject::ChildLayoutData data) {
    auto position = IndexOfChild(child);
    if (position == -1) {
      return;
    }
    container_render_object_.SetChildLayoutDataAt(position, data);
  }

 protected:
  void OnChildInserted(Control* control, Index index) override {
    container_render_object_.AddChild(control->GetRenderObject(), index);
  }

  void OnChildRemoved(Control* control, Index index) override {
    container_render_object_.RemoveChild(control->GetRenderObject());
  }

 private:
  TRenderObject container_render_object_;
};
}  // namespace cru::ui::controls
