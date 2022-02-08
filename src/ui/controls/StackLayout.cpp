#include "cru/ui/controls/StackLayout.h"
#include <memory>

#include "cru/ui/render/StackLayoutRenderObject.h"

namespace cru::ui::controls {
using render::StackLayoutRenderObject;

StackLayout::StackLayout() {
  render_object_ = std::make_unique<StackLayoutRenderObject>();
  render_object_->SetAttachedControl(this);
  SetContainerRenderObject(render_object_.get());
}

StackLayout::~StackLayout() = default;

render::RenderObject* StackLayout::GetRenderObject() const {
  return render_object_.get();
}

const StackChildLayoutData& StackLayout::GetChildLayoutData(Index position) {
  return render_object_->GetChildLayoutData(position);
}

void StackLayout::SetChildLayoutData(Index position,
                                     StackChildLayoutData data) {
  render_object_->SetChildLayoutData(position, std::move(data));
}
}  // namespace cru::ui::controls
