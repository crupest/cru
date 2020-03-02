#include "cru/ui/render/canvas_render_object.hpp"

namespace cru::ui::render {
CanvasRenderObject::CanvasRenderObject() : RenderObject(ChildMode::None) {}

CanvasRenderObject::~CanvasRenderObject() = default;

void CanvasRenderObject::Draw(platform::graph::IPainter* painter) {
  const auto rect = GetContentRect();
  CanvasPaintEventArgs args{painter, rect};
  paint_event_.Raise(args);
}

RenderObject* CanvasRenderObject::HitTest(const Point& point) {
  const auto padding_rect = GetPaddingRect();
  return padding_rect.IsPointInside(point) ? this : nullptr;
}
}  // namespace cru::ui::render
