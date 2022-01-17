#pragma once
#include "Base.hpp"

#include "cru/common/Event.hpp"
#include "cru/ui/helper/ClickDetector.hpp"

namespace cru::ui::controls {
struct CRU_UI_API IClickableControl : virtual Interface {
  virtual helper::ClickState GetClickState() = 0;
  virtual IEvent<helper::ClickState>* ClickStateChangeEvent() = 0;
};
}  // namespace cru::ui::controls
