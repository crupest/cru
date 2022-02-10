#pragma once
#include "LayoutControl.h"

#include "../render/StackLayoutRenderObject.h"

namespace cru::ui::controls {
class CRU_UI_API StackLayout
    : public LayoutControl<render::StackLayoutRenderObject> {
 public:
  static constexpr StringView kControlType = u"StackLayout";

  StackLayout();
  CRU_DELETE_COPY(StackLayout)
  CRU_DELETE_MOVE(StackLayout)
  ~StackLayout() override;

  String GetControlType() const final { return kControlType.ToString(); }
};
}  // namespace cru::ui::controls
