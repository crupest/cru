#include "cru/ui/render/CanvasRenderObject.h"
#include "cru/ui/render/RenderObject.h"

namespace cru::ui::render {
CanvasRenderObject::CanvasRenderObject() : RenderObject(kRenderObjectName) {}

RenderObject* CanvasRenderObject::HitTest(const Point& point) {
  const auto padding_rect = GetPaddingRect();
  return padding_rect.IsPointInside(point) ? this : nullptr;
}

Size CanvasRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement) {
  return requirement.Coerce(requirement.suggest.GetSizeOr({100, 100}));
}

void CanvasRenderObject::OnLayoutContent(const Rect& content_rect) {
  CRU_UNUSED(content_rect)
}

void CanvasRenderObject::OnDraw(RenderObjectDrawContext& context) {
  const auto rect = GetContentRect();
  CanvasPaintEventArgs args{context.painter, rect.GetSize()};
  PaintEvent_.Raise(args);
}
}  // namespace cru::ui::render
