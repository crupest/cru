#include "cru/ui/events/MouseEventArgs.hpp"

#include "cru/ui/controls/Control.hpp"
#include "cru/ui/host/WindowHost.hpp"
#include "cru/ui/render/RenderObject.hpp"

namespace cru::ui::events {
Point MouseEventArgs::GetPoint(render::RenderObject* render_object) const {
  return GetPoint() - render_object->GetTotalOffset();
}

Point MouseEventArgs::GetPointToContent(
    render::RenderObject* render_object) const {
  return render_object->FromRootToContent(GetPoint());
}

Point MouseEventArgs::GetPointOfScreen() const {
  auto sender = GetSender();
  if (auto control = dynamic_cast<controls::Control*>(sender)) {
    if (auto host = control->GetWindowHost())
      return GetPoint() + control->GetWindowHost()
                              ->GetNativeWindow()
                              ->GetClientRect()
                              .GetLeftTop();
  }
  return GetPoint();
}
}  // namespace cru::ui::events
