#pragma once
#include "LayoutControl.h"

#include "cru/ui/render/FlexLayoutRenderObject.h"

namespace cru::ui::controls {

using render::FlexChildLayoutData;
using render::FlexCrossAlignment;
using render::FlexDirection;
using render::FlexMainAlignment;

class CRU_UI_API FlexLayout
    : public LayoutControl<render::FlexLayoutRenderObject> {
 public:
  static constexpr auto kControlName = "FlexLayout";

 public:
  FlexLayout();

  FlexMainAlignment GetContentMainAlign();
  void SetContentMainAlign(FlexMainAlignment value);

  FlexDirection GetFlexDirection();
  void SetFlexDirection(FlexDirection direction);

  FlexCrossAlignment GetItemCrossAlign();
  void SetItemCrossAlign(FlexCrossAlignment alignment);
};
}  // namespace cru::ui::controls
