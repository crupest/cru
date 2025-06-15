#pragma once
#include "RenderObject.h"

#include <cru/Event.h>

namespace cru::ui::render {
class CanvasPaintEventArgs {
 public:
  CanvasPaintEventArgs(graphics::IPainter* painter,
                       const Size& paint_size)
      : painter_(painter), paint_size_(paint_size) {}

  graphics::IPainter* GetPainter() const { return painter_; }
  Size GetPaintSize() const { return paint_size_; }

 private:
  graphics::IPainter* painter_;
  Size paint_size_;
};

// Layout logic:
// If no preferred size is set. Then (100, 100) is used and then coerced to
// required range.
class CRU_UI_API CanvasRenderObject : public RenderObject {
 public:
  CanvasRenderObject();


  ~CanvasRenderObject();

 public:
  RenderObject* HitTest(const Point& point) override;

  Size GetDesiredSize() const { return desired_size_; }

  IEvent<CanvasPaintEventArgs>* PaintEvent() { return &paint_event_; }

  void Draw(graphics::IPainter* painter) override;

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement,
                        const MeasureSize& preferred_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

  Size OnMeasureContent1(const BoxConstraint& constraint) override;

 private:
  Size desired_size_{};

  Event<CanvasPaintEventArgs> paint_event_;
};
}  // namespace cru::ui::render
