#pragma once
#include "SingleChildControl.h"

#include "../render/BorderRenderObject.h"

namespace cru::ui::controls {
class CRU_UI_API Container
    : public SingleChildControl<render::BorderRenderObject> {
  static constexpr StringView kControlType = u"Container";

 public:
  Container();
  CRU_DELETE_COPY(Container)
  CRU_DELETE_MOVE(Container)

  ~Container() override;

 public:
  String GetControlType() const final { return kControlType.ToString(); }
};
}  // namespace cru::ui::controls
