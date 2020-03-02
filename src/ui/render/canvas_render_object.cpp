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

Size CanvasRenderObject::OnMeasureContent(const Size& available_size) {
  return Size{};
}

void CanvasRenderObject::OnLayoutContent(const Rect& content_rect) {}
}  // namespace cru::ui::render
