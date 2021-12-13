#include "cru/ui/events/MouseEventArgs.hpp"

#include "cru/ui/render/RenderObject.hpp"

namespace cru::ui::events {
Point MouseEventArgs::GetPoint(render::RenderObject* render_object) const {
  return GetPoint() - render_object->GetTotalOffset();
}

Point MouseEventArgs::GetPointToContent(
    render::RenderObject* render_object) const {
  return render_object->FromRootToContent(GetPoint());
}
}  // namespace cru::ui::events
