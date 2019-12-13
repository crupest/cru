#include "cru/ui/render/window_render_object.hpp"

#include "../helper.hpp"
#include "cru/common/logger.hpp"
#include "cru/platform/graph/util/painter.hpp"
#include "cru/platform/native/ui_application.hpp"
#include "cru/platform/native/window.hpp"
#include "cru/ui/window.hpp"

#include <cassert>

namespace cru::ui::render {
class WindowRenderHost : public IRenderHost,
                         public SelfResolvable<WindowRenderHost> {
 public:
  WindowRenderHost(WindowRenderObject* render_object)
      : render_object_(render_object) {
    assert(render_object != nullptr);
  }

  void InvalidateLayout() override;

  void InvalidatePaint() override {
    render_object_->GetWindow()->GetNativeWindow()->RequestRepaint();
  }

  IEvent<AfterLayoutEventArgs>* AfterLayoutEvent() override {
    return &after_layout_event_;
  }

 private:
  WindowRenderObject* render_object_;

  bool need_layout_ = false;

  Event<AfterLayoutEventArgs> after_layout_event_;
};

void WindowRenderHost::InvalidateLayout() {
  if (!need_layout_) {
    log::Debug("A relayout is required.");
    GetUiApplication()->InvokeLater([resolver = this->CreateResolver()] {
      if (const auto host = resolver.Resolve()) {
        host->render_object_->Relayout();
        host->need_layout_ = false;
        host->after_layout_event_.Raise(AfterLayoutEventArgs{});
        log::Debug("A relayout finished.");
        host->InvalidatePaint();
      }
    });
    need_layout_ = true;
  }
}

WindowRenderObject::WindowRenderObject(Window* window)
    : window_(window), render_host_(new WindowRenderHost(this)) {
  SetChildMode(ChildMode::Single);
  SetRenderHost(render_host_.get());
  after_layout_event_guard_.Reset(render_host_->AfterLayoutEvent()->AddHandler(
      [this](auto) { NotifyAfterLayoutRecursive(this); }));
}

void WindowRenderObject::Relayout() {
  const auto client_size = window_->GetNativeWindow()->GetClientSize();
  Measure(client_size);
  Layout(Rect{Point{}, client_size});
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

namespace {
void SetRenderHostRecursive(RenderObject* render_object, IRenderHost* host) {
  render_object->SetRenderHost(host);
  for (const auto child : render_object->GetChildren()) {
    SetRenderHostRecursive(child, host);
  }
}
}  // namespace

void WindowRenderObject::OnAddChild(RenderObject* new_child, int position) {
  CRU_UNUSED(position)

  SetRenderHostRecursive(new_child, render_host_.get());
}

void WindowRenderObject::OnRemoveChild(RenderObject* new_child, int position) {
  CRU_UNUSED(position)

  SetRenderHostRecursive(new_child, nullptr);
}

Size WindowRenderObject::OnMeasureContent(const Size& available_size) {
  if (const auto child = GetChild()) child->Measure(available_size);
  return available_size;
}

void WindowRenderObject::OnLayoutContent(const Rect& content_rect) {
  if (const auto child = GetChild()) child->Layout(content_rect);
}
}  // namespace cru::ui::render
