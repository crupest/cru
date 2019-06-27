#pragma once
#include "render_object.hpp"

namespace cru::ui {
class Window;
}

namespace cru::ui::render {
class WindowRenderObject : public RenderObject {
 public:
  WindowRenderObject(Window* window) : window_(window) {}
  WindowRenderObject(const WindowRenderObject& other) = delete;
  WindowRenderObject(WindowRenderObject&& other) = delete;
  WindowRenderObject& operator=(const WindowRenderObject& other) = delete;
  WindowRenderObject& operator=(WindowRenderObject&& other) = delete;
  ~WindowRenderObject() override = default;

  void MeasureAndLayout();

  void Draw(platform::graph::Painter* painter) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  void OnAddChild(RenderObject* new_child, int position) override;

  Size OnMeasureContent(const Size& available_size) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  RenderObject* GetChild() const {
    return GetChildren().empty() ? nullptr : GetChildren()[0];
  }

 private:
  Window* window_;
};
}  // namespace cru::ui::render
