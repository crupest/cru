#include "cru/ui/controls/Window.hpp"

#include "cru/common/Base.hpp"
#include "cru/ui/host/WindowHost.hpp"
#include "cru/ui/render/Base.hpp"
#include "cru/ui/render/StackLayoutRenderObject.hpp"

namespace cru::ui::controls {
Window* Window::CreateOverlapped() { return new Window(); }

Window::Window() : render_object_(new render::StackLayoutRenderObject()) {
  render_object_->SetAttachedControl(this);
  window_host_ = std::make_unique<host::WindowHost>(this);
}

Window::~Window() {}

std::u16string_view Window::GetControlType() const { return control_type; }

render::RenderObject* Window::GetRenderObject() const {
  return render_object_.get();
}

void Window::OnAddChild(Control* child, Index position) {
  render_object_->AddChild(child->GetRenderObject(), position);
}

void Window::OnRemoveChild(Control* child, Index position) {
  CRU_UNUSED(child);
  render_object_->RemoveChild(position);
}
}  // namespace cru::ui::controls
