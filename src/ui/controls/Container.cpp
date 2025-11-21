#include "cru/ui/controls/Container.h"
#include "cru/ui/render/BorderRenderObject.h"

namespace cru::ui::controls {
Container::Container()
    : SingleChildControl<render::BorderRenderObject>(kControlName) {}
}  // namespace cru::ui::controls
