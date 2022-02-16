#pragma once
#include "Control.h"

namespace cru::ui::controls {
template <typename TRenderObject>
class SingleChildControl : public Control {
 protected:
  SingleChildControl() : container_render_object_(new TRenderObject()) {
    container_render_object_->SetAttachedControl(this);
  }

 public:
  CRU_DELETE_COPY(SingleChildControl)
  CRU_DELETE_MOVE(SingleChildControl)

  ~SingleChildControl() override {}

  Control* GetChild() const { return child_; }
  void SetChild(Control* child) {
    if (child == child_) return;

    assert(child == nullptr || child->GetParent() == nullptr);

    if (child_) {
      child_->SetParent(nullptr);
    }

    child_ = child;

    if (child) {
      child->SetParent(this);
    }

    container_render_object_->SetChild(
        child == nullptr ? nullptr : child->GetRenderObject());
  }

  render::RenderObject* GetRenderObject() const override {
    return container_render_object_.get();
  }

  TRenderObject* GetContainerRenderObject() const {
    return container_render_object_.get();
  }

  void ForEachChild(const std::function<void(Control*)>& predicate) override {
    if (child_) {
      predicate(child_);
    }
  }

  void RemoveChild(Control* child) override {
    if (child_ == child) {
      SetChild(nullptr);
    }
  }

 private:
  Control* child_ = nullptr;
  std::unique_ptr<TRenderObject> container_render_object_;
};
}  // namespace cru::ui::controls
