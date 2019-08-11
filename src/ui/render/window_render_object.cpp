#include "cru/ui/render/window_render_object.hpp"

#include "cru/platform/graph/util/painter_util.hpp"
#include "cru/platform/native/native_window.hpp"
#include "cru/ui/window.hpp"

#include <cassert>

namespace cru::ui::render {
void WindowRenderObject::MeasureAndLayout() {
  const auto client_size = window_->GetNativeWindow()->GetClientSize();
  Measure(client_size);
  Layout(Rect{Point{}, client_size});
}

void WindowRenderObject::Draw(platform::graph::Painter* painter) {
  painter->Clear(colors::white);
  if (const auto child = GetChild()) {
    auto offset = child->GetOffset();
    platform::graph::util::WithTransform(
        painter, platform::Matrix::Translation(offset.x, offset.y),
        [child](platform::graph::Painter* p) { child->Draw(p); });
  }
}

RenderObject* WindowRenderObject::HitTest(const Point& point) {
  if (const auto child = GetChild()) {
    auto offset = child->GetOffset();
    Point p{point.x - offset.x, point.y - offset.y};
    const auto result = child->HitTest(p);
    if (result != nullptr) {
      return result;
    }
  }
  return Rect{Point{}, GetSize()}.IsPointInside(point) ? this : nullptr;
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
