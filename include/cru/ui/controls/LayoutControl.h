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
    container_render_object_.InsertChildAt(control->GetRenderObject(), index);
    Control::OnChildInserted(control, index);
  }

  void OnChildRemoved(Control* control, Index index) override {
    if (!control->IsUnderTreeObjectDestroying()) {
      container_render_object_.RemoveChildAt(index);
    }
    Control::OnChildRemoved(control, index);
  }

 private:
  TRenderObject container_render_object_;
};
}  // namespace cru::ui::controls
