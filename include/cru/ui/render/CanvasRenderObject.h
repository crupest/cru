#pragma once
#include "RenderObject.h"

#include "cru/base/Event.h"

namespace cru::ui::render {
class CanvasPaintEventArgs {
 public:
  CanvasPaintEventArgs(platform::graphics::IPainter* painter,
                       const Size& paint_size)
      : painter_(painter), paint_size_(paint_size) {}
  CRU_DEFAULT_COPY(CanvasPaintEventArgs)
  CRU_DEFAULT_MOVE(CanvasPaintEventArgs)
  ~CanvasPaintEventArgs() = default;

  platform::graphics::IPainter* GetPainter() const { return painter_; }
  Size GetPaintSize() const { return paint_size_; }

 private:
  platform::graphics::IPainter* painter_;
  Size paint_size_;
};

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

  Size OnMeasureContent1(const BoxConstraint& constraint) override;

 private:
  Size desired_size_{};

  Event<CanvasPaintEventArgs> paint_event_;
};
}  // namespace cru::ui::render
