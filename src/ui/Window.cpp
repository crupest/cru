#include "cru/ui/Window.hpp"

#include "cru/ui/render/WindowRenderObject.hpp"
#include "cru/ui/WindowHost.hpp"

namespace cru::ui {
Window* Window::CreateOverlapped() {
  return new Window(tag_overlapped_constructor{});
}

Window::Window(tag_overlapped_constructor) {
  managed_ui_host_ = std::make_unique<WindowHost>(this);
}

Window::~Window() {
  // explicit destroy ui host first.
  managed_ui_host_.reset();
}

std::u16string_view Window::GetControlType() const { return control_type; }

render::RenderObject* Window::GetRenderObject() const { return render_object_; }

void Window::OnChildChanged(Control* old_child, Control* new_child) {
  if (old_child) render_object_->RemoveChild(0);
  if (new_child) render_object_->AddChild(new_child->GetRenderObject(), 0);
}
}  // namespace cru::ui
