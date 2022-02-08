#include "cru/ui/controls/Window.h"

#include "cru/common/Base.h"
#include "cru/platform/gui/Base.h"
#include "cru/ui/controls/RootControl.h"
#include "cru/ui/host/WindowHost.h"
#include "cru/ui/render/Base.h"
#include "cru/ui/render/StackLayoutRenderObject.h"

namespace cru::ui::controls {
Window* Window::Create(Control* attached_control) {
  return new Window(attached_control);
}

Window::Window(Control* attached_control) : RootControl(attached_control) {}

Window::~Window() {}
}  // namespace cru::ui::controls
