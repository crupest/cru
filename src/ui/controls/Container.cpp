#include "cru/ui/controls/Container.hpp"

#include "cru/platform/graphics/Factory.hpp"
#include "cru/ui/render/BorderRenderObject.hpp"
#include "cru/ui/render/RenderObject.hpp"

namespace cru::ui::controls {
Container::Container() {
  render_object_ = std::make_unique<render::BorderRenderObject>();
  render_object_->SetBorderEnabled(false);
  render_object_->SetAttachedControl(this);
  SetContainerRenderObject(render_object_.get());
}

Container::~Container() = default;

render::RenderObject* Container::GetRenderObject() const {
  return render_object_.get();
}
}  // namespace cru::ui::controls
