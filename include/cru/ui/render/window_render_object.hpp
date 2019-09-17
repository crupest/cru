#pragma once
#include "render_object.hpp"

#include <memory>

namespace cru::ui {
class Window;
}

namespace cru::ui::render {
class WindowRenderObject : public RenderObject {
 public:
  WindowRenderObject(Window* window);
  WindowRenderObject(const WindowRenderObject& other) = delete;
  WindowRenderObject(WindowRenderObject&& other) = delete;
  WindowRenderObject& operator=(const WindowRenderObject& other) = delete;
  WindowRenderObject& operator=(WindowRenderObject&& other) = delete;
  ~WindowRenderObject() override = default;

  Window* GetWindow() const { return window_; }

  void Relayout();

  void Draw(platform::graph::Painter* painter) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  void OnAddChild(RenderObject* new_child, int position) override;
  void OnRemoveChild(RenderObject* new_child, int position) override;

  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  RenderObject* GetChild() const {
    return GetChildren().empty() ? nullptr : GetChildren()[0];
  }

 private:
  Window* window_;

  EventRevokerGuard after_layout_event_guard_;

  std::unique_ptr<IRenderHost> render_host_;
};
}  // namespace cru::ui::render
