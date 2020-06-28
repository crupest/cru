#pragma once
#include "RenderObject.hpp"

namespace cru::ui::render {
// Layout logic:
// If no preferred size is set. Then (100, 100) is used and then coerced to
// required range.
class CanvasRenderObject : public RenderObject {
 public:
  CanvasRenderObject();

  CRU_DELETE_COPY(CanvasRenderObject)
  CRU_DELETE_MOVE(CanvasRenderObject)

  ~CanvasRenderObject();

 public:
  RenderObject* HitTest(const Point& point) override;

  Size GetDesiredSize() const { return desired_size_; }

  IEvent<CanvasPaintEventArgs>* PaintEvent() { return &paint_event_; }

 protected:
  void OnDrawContent(platform::graph::IPainter* painter) override;

  Size OnMeasureContent(const MeasureRequirement& requirement,
                        const MeasureSize& preferred_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  Size desired_size_{};

  Event<CanvasPaintEventArgs> paint_event_;
};
}  // namespace cru::ui::render
