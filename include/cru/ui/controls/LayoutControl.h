#pragma once
#include "Control.h"

namespace cru::ui::controls {
template <typename TRenderObject>
class CRU_UI_API LayoutControl : public Control {
 protected:
  LayoutControl() : container_render_object_(new TRenderObject()) {
    container_render_object_->SetAttachedControl(this);
  }

 public:
  LayoutControl(const LayoutControl& other) = delete;
  LayoutControl(LayoutControl&& other) = delete;
  LayoutControl& operator=(const LayoutControl& other) = delete;
  LayoutControl& operator=(LayoutControl&& other) = delete;
  ~LayoutControl() override = default;

 public:
  void ForEachChild(const std::function<void(Control*)>& callback) override {
    for (auto child : children_) {
      callback(child);
    }
  }

  render::RenderObject* GetRenderObject() const override {
    return container_render_object_.get();
  }

  TRenderObject* GetContainerRenderObject() const {
    return container_render_object_.get();
  }

  void AddChild(Control* child, Index position) {
    Expects(child);
    Expects(child->GetParent() == nullptr);
    if (position < 0) position = 0;
    if (position > children_.size()) position = children_.size();
    children_.insert(children_.begin() + position, child);
    child->SetParent(this);

    assert(child->GetRenderObject());
    container_render_object_->AddChild(child->GetRenderObject(), position);
  }

  void RemoveChild(Index position) {
    if (position < 0 || position >= children_.size()) return;
    auto child = children_[position];
    children_.erase(children_.begin() + position);
    child->SetParent(nullptr);
    container_render_object_->RemoveChild(position);
  }

  const typename TRenderObject::ChildLayoutData& GetChildLayoutData(
      Index position) {
    return container_render_object_->GetChildLayoutDataAt(position);
  }

  void SetChildLayoutData(Index position,
                          typename TRenderObject::ChildLayoutData data) {
    container_render_object_->SetChildLayoutDataAt(position, data);
  }

 private:
  std::unique_ptr<TRenderObject> container_render_object_;

  std::vector<Control*> children_;
};
}  // namespace cru::ui::controls
