#include "cru/ui/components/Component.h"

#include "cru/ui/controls/Control.h"

namespace cru::ui::components {
void Component::OnPrepareDelete() { GetRootControl()->RemoveFromParent(); }

void Component::DeleteIfDeleteByParent(bool delete_later) {
  if (delete_by_parent_) {
    if (delete_later) {
      DeleteLater();
    } else {
      delete this;
    }
  }
}
}  // namespace cru::ui::components
