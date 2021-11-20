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
RootControl::RootControl(Control* attached_control,
                         host::CreateWindowParams params)
    : attached_control_(attached_control) {
  render_object_ = std::make_unique<render::StackLayoutRenderObject>();
  render_object_->SetAttachedControl(this);
  SetContainerRenderObject(render_object_.get());
  window_host_ = std::make_unique<host::WindowHost>(this, params);

  if (gain_focus_on_create_and_destroy_when_lose_focus_) {
    auto native_window = window_host_->GetNativeWindow();
    native_window->CreateEvent()->AddHandler(
        [](platform::gui::INativeWindow* window) {
          window->CreateEvent()->AddHandler(
              [window](std::nullptr_t) { window->RequestFocus(); });
        });

    native_window->FocusEvent()->AddHandler(
        [native_window](platform::gui::FocusChangeType type) {
          if (type == platform::gui::FocusChangeType::Lost) {
            native_window->Close();
          }
        });
  }
}

RootControl::~RootControl() {}

render::RenderObject* RootControl::GetRenderObject() const {
  return render_object_.get();
}

void RootControl::SetGainFocusOnCreateAndDestroyWhenLoseFocus(bool value) {
  gain_focus_on_create_and_destroy_when_lose_focus_ = value;
}
}  // namespace cru::ui::controls
