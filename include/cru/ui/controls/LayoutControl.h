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
  LayoutControl(const LayoutControl& other) = delete;
  LayoutControl(LayoutControl&& other) = delete;
  LayoutControl& operator=(const LayoutControl& other) = delete;
  LayoutControl& operator=(LayoutControl&& other) = delete;
  ~LayoutControl() override { ClearChildren(); }

 public:
  const std::vector<Control*>& GetChildren() const { return children_; }
  Index GetChildCount() const { return children_.size(); }
  Control* GetChild(Index index) const { return children_[index]; }
  Index IndexOf(Control* control) const {
    auto it = std::find(children_.begin(), children_.end(), control);
    if (it == children_.end()) {
      return -1;
    }
    return it - children_.begin();
  }

  void ForEachChild(const std::function<void(Control*)>& callback) override {
    for (auto child : children_) {
      callback(child);
    }
  }

  void RemoveChild(Control* child) override {
    auto index = IndexOf(child);
    if (index != -1) {
      RemoveChildAt(index);
    }
  }

  render::RenderObject* GetRenderObject() const override {
    return container_render_object_.get();
  }

  TRenderObject* GetContainerRenderObject() const {
    return container_render_object_.get();
  }

  void AddChildAt(Control* child, Index position) {
    Expects(child);
    Expects(child->GetParent() == nullptr);
    if (position < 0) position = 0;
    if (position > children_.size()) position = children_.size();
    children_.insert(children_.begin() + position, child);
    child->SetParent(this);

    assert(child->GetRenderObject());
    container_render_object_->AddChild(child->GetRenderObject(), position);
  }

  void AddChild(Control* child) { AddChildAt(child, GetChildCount()); }

  void RemoveChildAt(Index position) {
    if (position < 0 || position >= children_.size()) return;
    auto child = children_[position];
    children_.erase(children_.begin() + position);
    container_render_object_->RemoveChild(position);
    child->SetParent(nullptr);
  }

  void ClearChildren() {
    container_render_object_->ClearChildren();
    for (auto child : children_) {
      child->SetParent(nullptr);
    }
    children_.clear();
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
