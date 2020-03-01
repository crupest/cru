#pragma once
#include "layout_render_object.hpp"

namespace cru::ui::render {
struct StackChildLayoutData {
  Alignment horizontal = Alignment::Start;
  Alignment vertical = Alignment::Start;
};

class StackLayoutRenderObject
    : public LayoutRenderObject<StackChildLayoutData> {
 public:
  StackLayoutRenderObject() = default;
  CRU_DELETE_COPY(StackLayoutRenderObject)
  CRU_DELETE_MOVE(StackLayoutRenderObject)
  ~StackLayoutRenderObject() = default;

 protected:
  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;
};
}  // namespace cru::ui::render
