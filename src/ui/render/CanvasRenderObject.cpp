#include "cru/ui/render/CanvasRenderObject.h"

namespace cru::ui::render {
CanvasRenderObject::CanvasRenderObject()  {}

CanvasRenderObject::~CanvasRenderObject() = default;

RenderObject* CanvasRenderObject::HitTest(const Point& point) {
  const auto padding_rect = GetPaddingRect();
  return padding_rect.IsPointInside(point) ? this : nullptr;
}

void CanvasRenderObject::Draw(platform::graphics::IPainter* painter) {
  const auto rect = GetContentRect();
  CanvasPaintEventArgs args{painter, rect.GetSize()};
  paint_event_.Raise(args);
}

Size CanvasRenderObject::OnMeasureContent(const MeasureRequirement& requirement,
                                          const MeasureSize& preferred_size) {
  return requirement.Coerce(Size{preferred_size.width.GetLengthOr(100),
                                 preferred_size.height.GetLengthOr(100)});
}

void CanvasRenderObject::OnLayoutContent(const Rect& content_rect) {
  CRU_UNUSED(content_rect)
}
}  // namespace cru::ui::render
