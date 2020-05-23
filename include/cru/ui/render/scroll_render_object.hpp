#pragma once
#include "render_object.hpp"

#include "cru/platform/graph/util/painter.hpp"

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
  void OnAddChild(RenderObject* new_child, Index position) override;
  void OnRemoveChild(RenderObject* removed_child, Index position) override;

 private:
  Point scroll_offset_;
};
}  // namespace cru::ui::render
