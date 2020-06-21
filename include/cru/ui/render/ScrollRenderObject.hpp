#pragma once
#include "RenderObject.hpp"

#include "cru/platform/graph/util/Painter.hpp"

namespace cru::ui::render {
class ScrollRenderObject : public RenderObject {
 public:
  ScrollRenderObject() : RenderObject(ChildMode::Single) {}

  CRU_DELETE_COPY(ScrollRenderObject)
  CRU_DELETE_MOVE(ScrollRenderObject)

  ~ScrollRenderObject() override = default;

  void Draw(platform::graph::IPainter* painter) override;

  RenderObject* HitTest(const Point& point) override;

  Point GetScrollOffset() { return scroll_offset_; }
  void SetScrollOffset(const Point& offset);

 protected:
  // Logic:
  // If available size is bigger than child's preferred size, then child's
  // preferred size is taken.
  // If not, all available size is taken while forming a scroll area.
  Size OnMeasureContent(const MeasureRequirement& requirement) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  Point scroll_offset_;
};
}  // namespace cru::ui::render
