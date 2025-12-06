#include "cru/ui/controls/Window.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"
#include "cru/ui/Base.h"
#include "cru/ui/controls/Control.h"
#include "cru/ui/controls/ControlHost.h"

#include <cassert>

namespace cru::ui::controls {
Window::Window()
    : LayoutControl<render::StackLayoutRenderObject>(kControlName),
      control_host_(new ControlHost(this)),
      attached_control_(nullptr) {
  GetContainerRenderObject()->SetDefaultHorizontalAlignment(Alignment::Stretch);
  GetContainerRenderObject()->SetDefaultVerticalAlignment(Alignment::Stretch);
}

Window* Window::CreatePopup(Control* attached_control) {
  auto window = new Window();
  window->GetNativeWindow()->SetStyleFlag(
      platform::gui::WindowStyleFlags::NoCaptionAndBorder);
  window->SetAttachedControl(attached_control);
  window->SetGainFocusOnCreateAndDestroyWhenLoseFocus(true);
  return window;
}

Control* Window::GetAttachedControl() { return attached_control_; }

void Window::SetAttachedControl(Control* control) {
  attached_control_ = control;
  if (control) {
    if (auto control_host = control->GetControlHost()) {
      control_host_->GetNativeWindow()->SetParent(
          control_host->GetNativeWindow());
    }
    parent_window_guard_.Reset(control->ControlHostChangeEvent()->AddHandler(
        [this](const ControlHostChangeEventArgs& args) {
          control_host_->GetNativeWindow()->SetParent(
              args.new_host ? args.new_host->GetNativeWindow() : nullptr);
        }));
  } else {
    control_host_->GetNativeWindow()->SetParent(nullptr);
    parent_window_guard_.Reset();
  }
}

platform::gui::INativeWindow* Window::GetNativeWindow() {
  return control_host_->GetNativeWindow();
}

void Window::SetGainFocusOnCreateAndDestroyWhenLoseFocus(bool value) {
  gain_focus_on_create_and_destroy_when_lose_focus_event_guard_.Clear();
  if (value) {
    gain_focus_on_create_and_destroy_when_lose_focus_event_guard_ +=
        GetNativeWindow()->VisibilityChangeEvent()->AddHandler(
            [this](platform::gui::WindowVisibilityType type) {
              if (type == platform::gui::WindowVisibilityType::Show) {
                GetNativeWindow()->RequestFocus();
              }
            });

    gain_focus_on_create_and_destroy_when_lose_focus_event_guard_ +=
        GetNativeWindow()->FocusEvent()->AddHandler(
            [this](platform::gui::FocusChangeType type) {
              if (type == platform::gui::FocusChangeType::Lose) {
                GetNativeWindow()->Close();
              }
            });
  }
}
}  // namespace cru::ui::controls
