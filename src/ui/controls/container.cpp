#include "cru/ui/controls/container.hpp"

#include "cru/platform/graph/factory.hpp"
#include "cru/ui/render/border_render_object.hpp"

namespace cru::ui::controls {
Container::Container() {
  render_object_ = std::make_unique<render::BorderRenderObject>();
  render_object_->SetBorderEnabled(false);
}

Container::~Container() = default;

void Container::OnChildChanged(Control*, Control* new_child) {
  render_object_->RemoveChild(0);
  render_object_->AddChild(new_child->GetRenderObject(), 0);
}
}  // namespace cru::ui::controls
