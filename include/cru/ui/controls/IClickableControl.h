#pragma once
#include "Base.h"

#include "cru/common/Event.h"
#include "cru/ui/helper/ClickDetector.h"

namespace cru::ui::controls {
struct CRU_UI_API IClickableControl : virtual Interface {
  virtual helper::ClickState GetClickState() = 0;
  virtual IEvent<helper::ClickState>* ClickStateChangeEvent() = 0;
};
}  // namespace cru::ui::controls
