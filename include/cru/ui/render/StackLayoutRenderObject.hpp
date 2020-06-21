#pragma once
#include "LayoutRenderObject.hpp"

namespace cru::ui::render {
class StackLayoutRenderObject
    : public LayoutRenderObject<StackChildLayoutData> {
 public:
  StackLayoutRenderObject() = default;
  CRU_DELETE_COPY(StackLayoutRenderObject)
  CRU_DELETE_MOVE(StackLayoutRenderObject)
  ~StackLayoutRenderObject() = default;

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement) override;
  void OnLayoutContent(const Rect& content_rect) override;
};
}  // namespace cru::ui::render
