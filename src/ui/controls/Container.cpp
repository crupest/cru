#include "cru/ui/controls/Container.h"

namespace cru::ui::controls {
Container::Container() : Control(kControlName) {
  border_render_object_.SetAttachedControl(this);
}

render::RenderObject* Container::GetRenderObject() {
  return &border_render_object_;
}

void Container::OnChildChanged(Control* old_child, Control* new_child) {
  if (old_child) {
    border_render_object_.SetChild(nullptr);
  }
  if (new_child) {
    border_render_object_.SetChild(new_child->GetRenderObject());
  }
}
}  // namespace cru::ui::controls
