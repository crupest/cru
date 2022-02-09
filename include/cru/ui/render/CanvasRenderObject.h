#pragma once
#include "RenderObject.h"

#include "cru/common/Event.h"

namespace cru::ui::render {
// Layout logic:
// If no preferred size is set. Then (100, 100) is used and then coerced to
// required range.
class CRU_UI_API CanvasRenderObject : public RenderObject {
 public:
  CanvasRenderObject();

  CRU_DELETE_COPY(CanvasRenderObject)
  CRU_DELETE_MOVE(CanvasRenderObject)

  ~CanvasRenderObject();

 public:
  RenderObject* HitTest(const Point& point) override;

  Size GetDesiredSize() const { return desired_size_; }

  IEvent<CanvasPaintEventArgs>* PaintEvent() { return &paint_event_; }

  void Draw(platform::graphics::IPainter* painter) override;

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement,
                        const MeasureSize& preferred_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  Size desired_size_{};

  Event<CanvasPaintEventArgs> paint_event_;
};
}  // namespace cru::ui::render
