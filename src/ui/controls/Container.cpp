#include "cru/ui/controls/Container.h"

#include "cru/platform/graphics/Factory.h"
#include "cru/ui/render/BorderRenderObject.h"
#include "cru/ui/render/RenderObject.h"

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
