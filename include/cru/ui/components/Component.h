#pragma once
#include "../Base.h"
#include "../DeleteLater.h"
#include "cru/common/SelfResolvable.h"

namespace cru::ui::components {
/**
 * \brief A component is a composition of controls.
 * \remarks Component should respect children's Component::IsDeleteByParent
 * value and decide whether to delete it.
 */
class CRU_UI_API Component : public Object,
                             public SelfResolvable<Component>,
                             public DeleteLaterImpl {
 public:
  Component() = default;

  CRU_DELETE_COPY(Component)
  CRU_DELETE_MOVE(Component)

  ~Component() = default;

  virtual controls::Control* GetRootControl() = 0;

  bool IsDeleteByParent() const { return delete_by_parent_; }
  void SetDeleteByParent(bool delete_by_parent) {
    delete_by_parent_ = delete_by_parent;
  }
  void DeleteIfDeleteByParent() const {
    if (delete_by_parent_) delete this;
  }

 private:
  bool delete_by_parent_ = false;
};
}  // namespace cru::ui::components
