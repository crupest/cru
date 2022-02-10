#include "cru/ui/controls/Window.h"

namespace cru::ui::controls {
Window::Window(Control* attached_control) : RootControl(attached_control) {}

Window::~Window() {}
}  // namespace cru::ui::controls
