#pragma once
#include "SingleChildControl.h"

#include "cru/ui/render/ScrollRenderObject.h"

namespace cru::ui::controls {
class CRU_UI_API ScrollView
    : public SingleChildControl<render::ScrollRenderObject> {
 public:
  static constexpr StringView kControlType = u"ScrollView";

  ScrollView();
  CRU_DELETE_COPY(ScrollView)
  CRU_DELETE_MOVE(ScrollView)
  ~ScrollView() override;

 public:
  String GetControlType() const override { return kControlType.ToString(); }
};
}  // namespace cru::ui::controls
