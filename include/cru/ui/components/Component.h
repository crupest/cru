#pragma once
#include "../Base.h"
#include "../DeleteLater.h"

namespace cru::ui::components {
/**
 * \brief A component is a composition of controls.
 * \remarks Component should respect children's Component::IsDeleteByParent
 * value and decide whether to delete it.
 */
class CRU_UI_API Component : public Object, public DeleteLaterImpl {
 public:
  virtual controls::Control* GetRootControl() = 0;

  bool IsDeleteByParent() const { return delete_by_parent_; }
  void SetDeleteByParent(bool delete_by_parent) {
    delete_by_parent_ = delete_by_parent;
  }
  void DeleteIfDeleteByParent(bool delete_later = true);

 private:
  bool delete_by_parent_ = false;
};
}  // namespace cru::ui::components
