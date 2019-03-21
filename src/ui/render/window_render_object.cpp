#include "window_render_object.hpp"

#include <cassert>

#include "graph/graph.hpp"
#include "ui/window.hpp"

namespace cru::ui::render {
void WindowRenderObject::MeasureAndLayout() {
  const auto client_size = window_->GetClientSize();
  Measure(client_size);
  Layout(Rect{Point::Zero(), client_size});
}

void WindowRenderObject::Draw(ID2D1RenderTarget* render_target) {
  if (const auto child = GetChild()) {
    auto offset = child->GetOffset();
    graph::WithTransform(render_target,
                         D2D1::Matrix3x2F::Translation(offset.x, offset.y),
                         [child](auto rt) { child->Draw(rt); });
  }
}

RenderObject* WindowRenderObject::HitTest(const Point& point) {
  if (const auto child = GetChild()) {
    auto offset = child->GetOffset();
    Point p{point.x - offset.x, point.y - offset.y};
    const auto result = child->HitTest(point);
    if (result != nullptr) {
      return result;
    }
  }
  return Rect{Point::Zero(), GetSize()}.IsPointInside(point) ? this : nullptr;
}

void WindowRenderObject::OnAddChild(RenderObject* new_child, int position) {
  assert(GetChildren().size() == 1);
}

Size WindowRenderObject::OnMeasureContent(const Size& available_size) {
  if (const auto child = GetChild()) child->Measure(available_size);
  return available_size;
}

void WindowRenderObject::OnLayoutContent(const Rect& content_rect) {
  if (const auto child = GetChild()) child->Layout(content_rect);
}
}  // namespace cru::ui::render
