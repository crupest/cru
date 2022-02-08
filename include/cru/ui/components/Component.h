#pragma once
#include "../Base.h"

namespace cru::ui::components {
// In destructor, component should check all owned controls whether it is
// attached to window, if not, destroy them, otherwise it is host's duty to
// destroy them.
class CRU_UI_API Component : public Object {
 public:
  Component() = default;

  CRU_DELETE_COPY(Component)
  CRU_DELETE_MOVE(Component)

  ~Component() = default;

  virtual controls::Control* GetRootControl() = 0;
};
}  // namespace cru::ui::components
