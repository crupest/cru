#pragma once
#include "SingleChildControl.h"

#include "cru/ui/render/ScrollRenderObject.h"

namespace cru::ui::controls {
class CRU_UI_API ScrollView
    : public SingleChildControl<render::ScrollRenderObject> {
 public:
  static constexpr std::string_view kControlType = "ScrollView";

  ScrollView();
  CRU_DELETE_COPY(ScrollView)
  CRU_DELETE_MOVE(ScrollView)
  ~ScrollView() override;

 public:
  std::string GetControlType() const override { return std::string(kControlType); }
};
}  // namespace cru::ui::controls
