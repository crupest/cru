#pragma once
#include "Control.h"

namespace cru::ui::controls {
template <typename TRenderObject>
class LayoutControl : public Control {
 protected:
  LayoutControl() : container_render_object_(new TRenderObject()) {
    container_render_object_->SetAttachedControl(this);
  }

 public:
  using Control::AddChild;
  using Control::InsertChildAt;
  using Control::RemoveChildAt;

  Index GetChildCount() const { return children_.size(); }
  Control* GetChildAt(Index index) const { return children_[index]; }

  Index IndexOfChild(Control* control) const {
    auto it = std::find(children_.begin(), children_.end(), control);
    if (it == children_.end()) {
      return -1;
    }
    return it - children_.begin();
  }

  render::RenderObject* GetRenderObject() const override {
    return container_render_object_.get();
  }

  TRenderObject* GetContainerRenderObject() const {
    return container_render_object_.get();
  }

  const typename TRenderObject::ChildLayoutData& GetChildLayoutData(
      Index position) {
    return container_render_object_->GetChildLayoutDataAt(position);
  }

  void SetChildLayoutData(Index position,
                          typename TRenderObject::ChildLayoutData data) {
    container_render_object_->SetChildLayoutDataAt(position, data);
  }

 protected:
  void OnChildInserted(Control* control, Index index) override {
    container_render_object_->AddChild(control->GetRenderObject(), index);
  }

  void OnChildRemoved([[maybe_unused]] Control* control, Index index) override {
    container_render_object_->RemoveChild(index);
  }

 private:
  std::unique_ptr<TRenderObject> container_render_object_;

  std::vector<Control*> children_;
};
}  // namespace cru::ui::controls
