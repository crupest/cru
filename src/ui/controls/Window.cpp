#include "cru/ui/controls/Window.hpp"

#include "cru/common/Base.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/ui/host/WindowHost.hpp"
#include "cru/ui/render/Base.hpp"
#include "cru/ui/render/StackLayoutRenderObject.hpp"

namespace cru::ui::controls {
Window* Window::CreateOverlapped() { return new Window(); }

Window::Window() : render_object_(new render::StackLayoutRenderObject()) {
  render_object_->SetAttachedControl(this);
  SetContainerRenderObject(render_object_.get());
  window_host_ = std::make_unique<host::WindowHost>(this);
}

Window::~Window() {}

std::u16string_view Window::GetControlType() const { return control_type; }

render::RenderObject* Window::GetRenderObject() const {
  return render_object_.get();
}

void Window::Show(bool create) {
  platform::gui::INativeWindow* native_window =
      create ? window_host_->CreateNativeWindow().get()
             : window_host_->GetNativeWindow();
  if (!native_window) return;
  native_window->SetVisible(true);
}
}  // namespace cru::ui::controls
