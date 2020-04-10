#include "cru/ui/render/window_render_object.hpp"

#include "../helper.hpp"
#include "cru/platform/graph/util/painter.hpp"
#include "cru/ui/ui_host.hpp"

namespace cru::ui::render {
WindowRenderObject::WindowRenderObject(UiHost* host) {
  SetChildMode(ChildMode::Single);
  ui_host_ = host;
  after_layout_event_guard_.Reset(host->AfterLayoutEvent()->AddHandler(
      [this](auto) { NotifyAfterLayoutRecursive(this); }));
}

void WindowRenderObject::Draw(platform::graph::IPainter* painter) {
  painter->Clear(colors::white);
  if (const auto child = GetChild()) {
    auto offset = child->GetOffset();
    platform::graph::util::WithTransform(
        painter, platform::Matrix::Translation(offset.x, offset.y),
        [child](platform::graph::IPainter* p) { child->Draw(p); });
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

Size WindowRenderObject::OnMeasureContent(const Size& available_size) {
  if (const auto child = GetChild()) child->Measure(available_size);
  return available_size;
}

void WindowRenderObject::OnLayoutContent(const Rect& content_rect) {
  if (const auto child = GetChild()) child->Layout(content_rect);
}
}  // namespace cru::ui::render
