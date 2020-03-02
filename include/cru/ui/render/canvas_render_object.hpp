#pragma once
#include "render_object.hpp"

#include "cru/common/event.hpp"

namespace cru::ui::render {
class CanvasPaintEventArgs {
 public:
  CanvasPaintEventArgs(platform::graph::IPainter* painter,
                       const Rect& paint_rect)
      : painter_(painter), paint_rect_(paint_rect) {}
  CRU_DEFAULT_COPY(CanvasPaintEventArgs)
  CRU_DEFAULT_MOVE(CanvasPaintEventArgs)
  ~CanvasPaintEventArgs() = default;

  platform::graph::IPainter* GetPainter() const { return painter_; }
  Rect GetPaintRect() const { return paint_rect_; }

 private:
  platform::graph::IPainter* painter_;
  Rect paint_rect_;
};

class CanvasRenderObject : public RenderObject {
 public:
  CanvasRenderObject();

  CRU_DELETE_COPY(CanvasRenderObject)
  CRU_DELETE_MOVE(CanvasRenderObject)

  ~CanvasRenderObject();

 public:
  void Draw(platform::graph::IPainter* painter) override;

  RenderObject* HitTest(const Point& point) override;

  IEvent<CanvasPaintEventArgs>* PaintEvent() { return &paint_event_; }

 private:
  Event<CanvasPaintEventArgs> paint_event_;
};
}  // namespace cru::ui::render
