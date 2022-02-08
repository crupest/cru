#include "cru/ui/controls/ScrollView.h"

#include "cru/ui/render/RenderObject.h"
#include "cru/ui/render/ScrollRenderObject.h"

#include <memory>

namespace cru::ui::controls {
ScrollView::ScrollView() {
  scroll_render_object_ = std::make_unique<render::ScrollRenderObject>();
  scroll_render_object_->SetAttachedControl(this);

  SetContainerRenderObject(scroll_render_object_.get());
}

render::RenderObject* ScrollView::GetRenderObject() const {
  return scroll_render_object_.get();
}
}  // namespace cru::ui::controls
