#include "cru/platform/gui/Window.h"
#include "cru/base/Base.h"

namespace cru::platform::gui {
bool INativeWindow::IsCreated() { NotImplemented(); }

IEvent<const NativePaintEventArgs&>* INativeWindow::Paint1Event() {
  NotImplemented();
}

}  // namespace cru::platform::gui
