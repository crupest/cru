#include "cru/ui/controls/Popup.hpp"

#include "cru/platform/gui/UiApplication.hpp"
#include "cru/ui/controls/RootControl.hpp"
#include "cru/ui/host/WindowHost.hpp"
#include "cru/ui/render/StackLayoutRenderObject.hpp"

#include <memory>

namespace cru::ui::controls {
Popup::Popup(Control* attached_control) : RootControl(attached_control) {}

Popup::~Popup() = default;

gsl::not_null<platform::gui::INativeWindow*> Popup::CreateNativeWindow(
    gsl::not_null<host::WindowHost*> host,
    platform::gui::INativeWindow* parent) {
  return host->CreateNativeWindow(
      {parent, platform::gui::CreateWindowFlags::NoCaptionAndBorder});
}

}  // namespace cru::ui::controls
