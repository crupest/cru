#pragma once
#include "Control.h"

#include <cassert>

namespace cru::ui::controls {
template <typename TSelf, typename TRenderObject>
class SingleChildControlMixin {
 public:
  Control* GetChild() {
    const auto& children = SingleChildControlMixinGetSelf()->GetChildren();
    assert(children.empty() || children.size() == 1);
    return children.empty() ? nullptr : children.front();
  }

  void SetChild(Control* child) {
    auto old_child = GetChild();
    if (old_child == child) return;
    auto self = SingleChildControlMixinGetSelf();
    if (old_child) {
      self->RemoveChildAt(0);
    }
    if (child) {
      self->InsertChildAt(child, 0);
    }

    GetContainerRenderObject()->SetChild(child ? child->GetRenderObject()
                                               : nullptr);
  }

  virtual TRenderObject* GetContainerRenderObject() = 0;

 private:
  TSelf* SingleChildControlMixinGetSelf() { return static_cast<TSelf*>(this); }
};

template <typename TRenderObject>
class SingleChildControl
    : public Control,
      public SingleChildControlMixin<SingleChildControl<TRenderObject>,
                                     TRenderObject> {
 protected:
  SingleChildControl(std::string name)
      : Control(std::move(name), &container_render_object_) {
    container_render_object_.SetAttachedControl(this);
  }

 protected:
  TRenderObject* GetContainerRenderObject() override {
    return &container_render_object_;
  }

  void OnChildRemoved(Control* control, Index index) override {
    container_render_object_.SetChild(nullptr);
  }

 private:
  TRenderObject container_render_object_;
};
}  // namespace cru::ui::controls
