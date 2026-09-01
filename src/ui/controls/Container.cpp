#include "cru/ui/controls/Container.h"

namespace cru::ui::controls {
Container::Container() : Control(kControlName, &border_render_object_) {
  border_render_object_.SetAttachedControl(this);
}
}  // namespace cru::ui::controls
