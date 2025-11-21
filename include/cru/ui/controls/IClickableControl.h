#pragma once

#include "../helper/ClickDetector.h"
#include "cru/base/Event.h"

namespace cru::ui::controls {
struct CRU_UI_API IClickableControl : virtual Interface {
  virtual helper::ClickState GetClickState() = 0;
  virtual IEvent<helper::ClickState>* ClickStateChangeEvent() = 0;
  virtual IEvent<const helper::ClickEventArgs&>* ClickEvent() = 0;
};
}  // namespace cru::ui::controls
