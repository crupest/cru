#include "cru/ui/render/CanvasRenderObject.h"
#include "cru/ui/render/RenderObject.h"

namespace cru::ui::render {
CanvasRenderObject::CanvasRenderObject() : RenderObject(kRenderObjectName) {}

RenderObject* CanvasRenderObject::HitTest(const Point& point) {
  const auto padding_rect = GetPaddingRect();
  return padding_rect.IsPointInside(point) ? this : nullptr;
}

void CanvasRenderObject::Draw(platform::graphics::IPainter* painter) {
  const auto rect = GetContentRect();
  CanvasPaintEventArgs args{painter, rect.GetSize()};
  PaintEvent_.Raise(args);
}

Size CanvasRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement) {
  return requirement.Coerce(requirement.suggest.GetSizeOr({100, 100}));
}

void CanvasRenderObject::OnLayoutContent(const Rect& content_rect) {
  CRU_UNUSED(content_rect)
}
}  // namespace cru::ui::render
