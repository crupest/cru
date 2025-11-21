#pragma once
#include "Control.h"

#include <cassert>

namespace cru::ui::controls {
template <typename TRenderObject>
class SingleChildControl : public Control {
 protected:
  SingleChildControl(std::string name) : Control(std::move(name)) {
    container_render_object_.SetAttachedControl(this);
  }

 public:
  Control* GetChild() {
    const auto& children = GetChildren();
    assert(children.empty() || children.size() == 1);
    return children.empty() ? nullptr : children.front();
  }

  void SetChild(Control* child) {
    auto old_child = GetChild();
    if (old_child == child) return;
    if (old_child) {
      RemoveChildAt(0);
    }
    if (child) {
      InsertChildAt(child, 0);
    }

    container_render_object_.SetChild(child ? child->GetRenderObject()
                                            : nullptr);
  }

  render::RenderObject* GetRenderObject() override {
    return &container_render_object_;
  }

  TRenderObject* GetContainerRenderObject() {
    return &container_render_object_;
  }

 private:
  TRenderObject container_render_object_;
};
}  // namespace cru::ui::controls
