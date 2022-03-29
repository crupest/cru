#include "cru/ui/controls/RootControl.h"

#include "cru/common/Base.h"
#include "cru/platform/gui/Base.h"
#include "cru/platform/gui/Window.h"
#include "cru/ui/Base.h"
#include "cru/ui/host/WindowHost.h"

#include <memory>

namespace cru::ui::controls {
RootControl::RootControl(Control* attached_control)
    : attached_control_(attached_control) {
  GetContainerRenderObject()->SetDefaultHorizontalAlignment(Alignment::Stretch);
  GetContainerRenderObject()->SetDefaultVertialAlignment(Alignment::Stretch);
  window_host_ = std::make_unique<host::WindowHost>(this);

  Control::window_host_ = this->window_host_.get();

  window_host_->SetLayoutPreferToFillWindow(true);
}

RootControl::~RootControl() {}

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
