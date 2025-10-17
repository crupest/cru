#pragma once
#include "LayoutControl.h"

#include "../render/StackLayoutRenderObject.h"

namespace cru::ui::controls {
using render::StackChildLayoutData;

class CRU_UI_API StackLayout
    : public LayoutControl<render::StackLayoutRenderObject> {
 public:
  static constexpr std::string_view kControlType = "StackLayout";

  StackLayout();
  CRU_DELETE_COPY(StackLayout)
  CRU_DELETE_MOVE(StackLayout)
  ~StackLayout() override;

  std::string GetControlType() const final { return std::string(kControlType); }
};
}  // namespace cru::ui::controls
