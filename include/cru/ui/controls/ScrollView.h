#pragma once
#include "SingleChildControl.h"

#include "cru/ui/render/ScrollRenderObject.h"

namespace cru::ui::controls {
class CRU_UI_API ScrollView
    : public SingleChildControl<render::ScrollRenderObject> {
 public:
  static ScrollView* Create() { return new ScrollView(); }

  static constexpr StringView kControlType = u"ScrollView";

 protected:
  ScrollView();

 public:
  CRU_DELETE_COPY(ScrollView)
  CRU_DELETE_MOVE(ScrollView)

  ~ScrollView() override;

 public:
  String GetControlType() const override { return kControlType.ToString(); }
};
}  // namespace cru::ui::controls
