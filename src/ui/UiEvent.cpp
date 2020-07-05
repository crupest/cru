#include "cru/ui/UiEvent.hpp"

#include "cru/ui/render/RenderObject.hpp"

namespace cru::ui::event {
Point MouseEventArgs::GetPointToContent(
    render::RenderObject* render_object) const {
  return render_object->FromRootToContent(GetPoint());
}
}  // namespace cru::ui::event
