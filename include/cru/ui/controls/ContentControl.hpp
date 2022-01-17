#pragma once
#include "Control.hpp"

#include "cru/ui/render/RenderObject.hpp"

namespace cru::ui::controls {
class CRU_UI_API ContentControl : public Control {
 protected:
  ContentControl() = default;

 public:
  ContentControl(const ContentControl& other) = delete;
  ContentControl(ContentControl&& other) = delete;
  ContentControl& operator=(const ContentControl& other) = delete;
  ContentControl& operator=(ContentControl&& other) = delete;
  ~ContentControl() override = default;

  Control* GetChild() const;
  void SetChild(Control* child);

 protected:
  virtual void OnChildChanged(Control* old_child, Control* new_child);

  render::RenderObject* GetContainerRenderObject() const {
    return container_render_object_;
  }
  void SetContainerRenderObject(render::RenderObject* ro) {
    container_render_object_ = ro;
  }

 private:
  using Control::AddChild;
  using Control::RemoveChild;

 private:
  render::RenderObject* container_render_object_ = nullptr;
};
}  // namespace cru::ui::controls
