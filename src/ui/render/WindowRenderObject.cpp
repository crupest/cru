#include "cru/ui/render/WindowRenderObject.hpp"

#include "../Helper.hpp"
#include "cru/platform/graph/util/Painter.hpp"
#include "cru/ui/UiHost.hpp"

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

Size WindowRenderObject::OnMeasureContent(
    const MeasureRequirement& requirement) {
  if (const auto child = GetChild()) {
    child->Measure(requirement);
    return child->GetMeasuredSize();
  } else {
    return Size{};
  }
}

void WindowRenderObject::OnLayoutContent(const Rect& content_rect) {
  if (const auto child = GetChild()) child->Layout(content_rect);
}
}  // namespace cru::ui::render
