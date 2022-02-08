#pragma once
#include "../Base.h"

namespace cru::ui::components {
/**
 * \remarks In destructor, component should first delete all child components
 * and then remove root control from its parent (by calling
 * Control::RemoveFromParent). Then delete all its root control.
 */
class CRU_UI_API Component : public Object {
 public:
  Component() = default;

  CRU_DELETE_COPY(Component)
  CRU_DELETE_MOVE(Component)

  ~Component() = default;

  virtual controls::Control* GetRootControl() = 0;
};
}  // namespace cru::ui::components
