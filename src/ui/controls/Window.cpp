#include "cru/ui/controls/Window.h"
#include "cru/platform/gui/UiApplication.h"
#include "cru/platform/gui/Window.h"
#include "cru/ui/Base.h"
#include "cru/ui/controls/ControlHost.h"

#include <cassert>

namespace cru::ui::controls {
Window::Window()
    : LayoutControl<render::StackLayoutRenderObject>(kControlName),
      control_host_(new ControlHost(this)),
      attached_control_(nullptr) {
  GetContainerRenderObject()->SetDefaultHorizontalAlignment(Alignment::Stretch);
  GetContainerRenderObject()->SetDefaultVertialAlignment(Alignment::Stretch);
}

Window* Window::CreatePopup() {
  auto window = new Window();
  window->GetNativeWindow()->SetStyleFlag(
      platform::gui::WindowStyleFlags::NoCaptionAndBorder);
  window->SetGainFocusOnCreateAndDestroyWhenLoseFocus(true);
  return window;
}

void Window::SetAttachedControl(Control* control) {
  attached_control_ = control;
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
