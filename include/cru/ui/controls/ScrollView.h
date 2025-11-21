#pragma once
#include "SingleChildControl.h"
#include "cru/ui/render/ScrollRenderObject.h"

namespace cru::ui::controls {
class CRU_UI_API ScrollView
    : public SingleChildControl<render::ScrollRenderObject> {
 public:
  static constexpr auto kControlName = "ScrollView";

  ScrollView();
};
}  // namespace cru::ui::controls
