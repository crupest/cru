#include "cru/ui/controls/Popup.h"

#include "cru/platform/gui/UiApplication.h"
#include "cru/ui/controls/RootControl.h"
#include "cru/ui/host/WindowHost.h"
#include "cru/ui/render/StackLayoutRenderObject.h"

#include <memory>

namespace cru::ui::controls {
Popup::Popup(Control* attached_control) : RootControl(attached_control) {
  GetWindowHost()->GetNativeWindow()->SetStyleFlag(
      cru::platform::gui::WindowStyleFlags::NoCaptionAndBorder);
  SetGainFocusOnCreateAndDestroyWhenLoseFocus(true);
}

Popup::~Popup() = default;
}  // namespace cru::ui::controls
