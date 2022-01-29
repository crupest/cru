#include "cru/ui/controls/RootControl.hpp"

#include "cru/common/Base.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/platform/gui/Window.hpp"
#include "cru/ui/Base.hpp"
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
  render_object_->SetDefaultHorizontalAlignment(Alignment::Stretch);
  render_object_->SetDefaultVertialAlignment(Alignment::Stretch);
  SetContainerRenderObject(render_object_.get());
  window_host_ = std::make_unique<host::WindowHost>(this);
  window_host_->SetLayoutPreferToFillWindow(true);
}

RootControl::~RootControl() {}

render::RenderObject* RootControl::GetRenderObject() const {
  return render_object_.get();
}

platform::gui::INativeWindow* RootControl::GetNativeWindow() {
  return window_host_->GetNativeWindow();
}

void RootControl::SetGainFocusOnCreateAndDestroyWhenLoseFocus(bool value) {
  gain_focus_on_create_and_destroy_when_lose_focus_event_guard_.Clear();
  if (value) {
    auto native_window = window_host_->GetNativeWindow();

    gain_focus_on_create_and_destroy_when_lose_focus_event_guard_ +=
        native_window->VisibilityChangeEvent()->AddHandler(
            [native_window](platform::gui::WindowVisibilityType type) {
              if (type == platform::gui::WindowVisibilityType::Show) {
                native_window->RequestFocus();
              }
            });

    gain_focus_on_create_and_destroy_when_lose_focus_event_guard_ +=
        native_window->FocusEvent()->AddHandler(
            [native_window](platform::gui::FocusChangeType type) {
              if (type == platform::gui::FocusChangeType::Lose) {
                native_window->Close();
              }
            });
  }
}
}  // namespace cru::ui::controls
