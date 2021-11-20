#include "cru/ui/controls/RootControl.hpp"

#include "cru/common/Base.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/platform/gui/Window.hpp"
#include "cru/ui/host/WindowHost.hpp"
#include "cru/ui/render/Base.hpp"
#include "cru/ui/render/StackLayoutRenderObject.hpp"
#include "gsl/pointers"

#include <memory>

namespace cru::ui::controls {
RootControl::RootControl(Control* attached_control)
    : attached_control_(attached_control) {
  render_object_ = std::make_unique<render::StackLayoutRenderObject>();
  render_object_->SetAttachedControl(this);
  SetContainerRenderObject(render_object_.get());
  window_host_ = std::make_unique<host::WindowHost>(this);
}

RootControl::~RootControl() {}

render::RenderObject* RootControl::GetRenderObject() const {
  return render_object_.get();
}

void RootControl::EnsureWindowCreated() { this->GetNativeWindow(true); }

Rect RootControl::GetRect() { return window_host_->GetWindowRect(); }

void RootControl::SetRect(const Rect& rect) {
  window_host_->SetWindowRect(rect);
}

void RootControl::Show(bool create) {
  platform::gui::INativeWindow* native_window = GetNativeWindow(create);
  if (!native_window) return;
  native_window->SetVisible(true);
  if (gain_focus_on_create_and_destroy_when_lose_focus_) {
    native_window->RequestFocus();
  }
}

platform::gui::INativeWindow* RootControl::GetNativeWindow(bool create) {
  const auto host = GetWindowHost();
  platform::gui::INativeWindow* native_window = host->GetNativeWindow();
  if (!create) return native_window;
  if (!native_window) {
    native_window = this->CreateNativeWindow(
        host, attached_control_
                  ? attached_control_->GetWindowHost()->GetNativeWindow()
                  : nullptr);

    if (gain_focus_on_create_and_destroy_when_lose_focus_) {
      native_window->FocusEvent()->AddHandler(
          [native_window](platform::gui::FocusChangeType type) {
            if (type == platform::gui::FocusChangeType::Lost) {
              native_window->Close();
            }
          });
    }
  }
  return native_window;
}

void RootControl::SetGainFocusOnCreateAndDestroyWhenLoseFocus(bool value) {
  gain_focus_on_create_and_destroy_when_lose_focus_ = value;
}
}  // namespace cru::ui::controls
