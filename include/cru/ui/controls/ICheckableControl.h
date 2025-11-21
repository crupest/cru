#pragma once
#include "../Base.h"
#include "cru/base/Event.h"

namespace cru::ui::controls {
struct CRU_UI_API ICheckableControl : virtual Interface {
  virtual bool IsChecked() = 0;
  virtual void SetChecked(bool checked) = 0;
  virtual IEvent<bool>* CheckedChangeEvent() = 0;
};
}  // namespace cru::ui::controls
