#pragma once
#include "../style/ApplyBorderStyleInfo.h"
#include "cru/base/Base.h"

namespace cru::ui::controls {
struct CRU_UI_API IBorderControl : virtual Interface {
  virtual void ApplyBorderStyle(const style::ApplyBorderStyleInfo& style) = 0;
};
}  // namespace cru::ui::controls
