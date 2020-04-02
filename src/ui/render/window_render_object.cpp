#include "cru/ui/render/window_render_object.hpp"

#include "../helper.hpp"
#include "cru/common/logger.hpp"
#include "cru/platform/graph/util/painter.hpp"
#include "cru/platform/native/ui_application.hpp"
#include "cru/platform/native/window.hpp"
#include "cru/ui/window.hpp"

namespace cru::ui::render {
class WindowRenderHost : public IRenderHost,
                         public SelfResolvable<WindowRenderHost> {
 public:
  WindowRenderHost(WindowRenderObject* render_object)
      : render_object_(render_object) {
    Expects(render_object != nullptr);
  }

  void InvalidateLayout() override;

  void InvalidatePaint() override {
    if (const auto native_window =
            render_object_->GetWindow()->ResolveNativeWindow())
      native_window->RequestRepaint();
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
  const auto native_window = window_->ResolveNativeWindow();
  const auto client_size = native_window
                               ? native_window->GetClientSize()
                               : Size{100, 100};  // a reasonable assumed size
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

Size WindowRenderObject::OnMeasureContent(const Size& available_size) {
  if (const auto child = GetChild()) child->Measure(available_size);
  return available_size;
}

void WindowRenderObject::OnLayoutContent(const Rect& content_rect) {
  if (const auto child = GetChild()) child->Layout(content_rect);
}
}  // namespace cru::ui::render
