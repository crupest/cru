#include "cru/ui/components/Component.h"

namespace cru::ui::components {
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
