#include "cru/ui/controls/Popup.hpp"

#include "cru/platform/gui/UiApplication.hpp"
#include "cru/ui/host/WindowHost.hpp"
#include "cru/ui/render/StackLayoutRenderObject.hpp"

#include <memory>

namespace cru::ui::controls {
Popup::Popup(Control* attached_control) : attached_control_(attached_control) {
  render_object_ = std::make_unique<render::StackLayoutRenderObject>();
  render_object_->SetAttachedControl(this);
  SetContainerRenderObject(render_object_.get());

  window_host_ = std::make_unique<host::WindowHost>(this);
}

Popup::~Popup() = default;
}  // namespace cru::ui::controls
