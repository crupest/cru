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
  Control* GetChild() {
    return GetChildren().empty() ? nullptr : GetChildren().front();
  }

  void SetChild(Control* child) {
    if (GetChild() == child) return;
    if (!GetChildren().empty()) {
      RemoveChildAt(0);
    }
    if (child) {
      InsertChildAt(child, 0);
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

 private:
  std::unique_ptr<TRenderObject> container_render_object_;
};
}  // namespace cru::ui::controls
