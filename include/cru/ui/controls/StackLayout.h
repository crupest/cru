#pragma once
#include "LayoutControl.h"
#include "../render/StackLayoutRenderObject.h"

namespace cru::ui::controls {
using render::StackChildLayoutData;

class CRU_UI_API StackLayout
    : public LayoutControl<render::StackLayoutRenderObject> {
 public:
  static constexpr auto kControlName = "StackLayout";

  StackLayout();
};
}  // namespace cru::ui::controls
