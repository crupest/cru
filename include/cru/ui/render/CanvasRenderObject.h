#pragma once
#include "RenderObject.h"

#include "cru/base/Event.h"

namespace cru::ui::render {
class CanvasPaintEventArgs {
 public:
  CanvasPaintEventArgs(platform::graphics::IPainter* painter,
                       const Size& paint_size)
      : painter_(painter), paint_size_(paint_size) {}

  platform::graphics::IPainter* GetPainter() const { return painter_; }
  Size GetPaintSize() const { return paint_size_; }

 private:
  platform::graphics::IPainter* painter_;
  Size paint_size_;
};

/**
 * Layout logic:
 * Preferred
 * If no preferred size is set. Then (100, 100) is used and then coerced to
 * required range.
 */
class CRU_UI_API CanvasRenderObject : public RenderObject {
 public:
  static constexpr auto kRenderObjectName = "CanvasRenderObject";

  CanvasRenderObject();

 public:
  RenderObject* HitTest(const Point& point) override;

  CRU_DEFINE_EVENT(Paint, const CanvasPaintEventArgs&)

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement) override;
  void OnLayoutContent(const Rect& content_rect) override;
  void OnDraw(RenderObjectDrawContext& context) override;
};
}  // namespace cru::ui::render
