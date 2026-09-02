#include "cru/ui/render/CanvasRenderObject.h"
#include "cru/ui/render/RenderObject.h"

namespace cru::ui::render {
CanvasRenderObject::CanvasRenderObject() : RenderObject(kRenderObjectName) {}

Size CanvasRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement) {
  return requirement.Coerce(requirement.suggest.GetSizeOr({100, 100}));
}

void CanvasRenderObject::OnDraw(RenderObjectDrawContext& context) {
  const auto rect = GetContentRect();
  CanvasPaintEventArgs args{context.painter, rect.GetSize()};
  PaintEvent_.Raise(args);
}
}  // namespace cru::ui::render
