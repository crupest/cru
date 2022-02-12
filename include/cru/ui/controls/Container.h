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
  std::shared_ptr<platform::graphics::IBrush> GetForegroundBrush() const {
    return GetContainerRenderObject()->GetForegroundBrush();
  }
  void SetForegroundBrush(
      const std::shared_ptr<platform::graphics::IBrush>& brush) {
    GetContainerRenderObject()->SetForegroundBrush(brush);
  }

  std::shared_ptr<platform::graphics::IBrush> GetBackgroundBrush() const {
    return GetContainerRenderObject()->GetBackgroundBrush();
  }
  void SetBackgroundBrush(
      const std::shared_ptr<platform::graphics::IBrush>& brush) {
    GetContainerRenderObject()->SetBackgroundBrush(brush);
  }

 public:
  String GetControlType() const final { return kControlType.ToString(); }
};
}  // namespace cru::ui::controls
