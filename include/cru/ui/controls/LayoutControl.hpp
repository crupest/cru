#pragma once
#include "Control.hpp"

namespace cru::ui::controls {
class LayoutControl : public Control {
 protected:
  LayoutControl() = default;
  explicit LayoutControl(render::RenderObject* container_render_object)
      : container_render_object_(container_render_object) {}

 public:
  LayoutControl(const LayoutControl& other) = delete;
  LayoutControl(LayoutControl&& other) = delete;
  LayoutControl& operator=(const LayoutControl& other) = delete;
  LayoutControl& operator=(LayoutControl&& other) = delete;
  ~LayoutControl() override = default;

  using Control::AddChild;
  using Control::RemoveChild;

  void ClearChildren();

 protected:
  // If container render object is not null. Render object of added or removed
  // child control will automatically sync to the container render object.
  render::RenderObject* GetContainerRenderObject() const;
  void SetContainerRenderObject(render::RenderObject* ro) {
    container_render_object_ = ro;
  }

  void OnAddChild(Control* child, Index position) override;
  void OnRemoveChild(Control* child, Index position) override;

 private:
  render::RenderObject* container_render_object_ = nullptr;
};
}  // namespace cru::ui::controls
