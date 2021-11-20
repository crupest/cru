#include "cru/ui/controls/Window.hpp"

#include "cru/common/Base.hpp"
#include "cru/platform/gui/Base.hpp"
#include "cru/ui/controls/RootControl.hpp"
#include "cru/ui/host/WindowHost.hpp"
#include "cru/ui/render/Base.hpp"
#include "cru/ui/render/StackLayoutRenderObject.hpp"

namespace cru::ui::controls {
Window* Window::Create(Control* attached_control) {
  return new Window(attached_control);
}

Window::Window(Control* attached_control)
    : RootControl(attached_control, host::CreateWindowParams{}) {}

Window::~Window() {}
}  // namespace cru::ui::controls
