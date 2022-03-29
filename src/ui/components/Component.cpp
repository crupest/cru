#include "cru/ui/components/Component.h"

#include "cru/ui/controls/Control.h"

namespace cru::ui::components {
void Component::OnPrepareDelete() { GetRootControl()->RemoveFromParent(); }
}  // namespace cru::ui::components
