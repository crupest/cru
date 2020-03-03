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

// The measure logic for `CanvasRenderObject` is that you set a desired size by
// `SetDesiredSize` (not `SetPreferredSize`) and it will compare desired size
// and available size and use the smaller one (by `Min`).
class CanvasRenderObject : public RenderObject {
 public:
  CanvasRenderObject();

  CRU_DELETE_COPY(CanvasRenderObject)
  CRU_DELETE_MOVE(CanvasRenderObject)

  ~CanvasRenderObject();

 public:
  void Draw(platform::graph::IPainter* painter) override;

  RenderObject* HitTest(const Point& point) override;

  Size GetDesiredSize() const { return desired_size_; }

  // Set the desired size. This is the content size excluding padding and
  // margin.
  void SetDesiredSize(const Size& new_size) { desired_size_ = new_size; }

  IEvent<CanvasPaintEventArgs>* PaintEvent() { return &paint_event_; }

 protected:
  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  Size desired_size_{};

  Event<CanvasPaintEventArgs> paint_event_;
};
}  // namespace cru::ui::render
