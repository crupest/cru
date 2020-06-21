#pragma once
#include "RenderObject.hpp"

namespace cru::ui::render {
class WindowRenderObject : public RenderObject {
 public:
  WindowRenderObject(UiHost* host);
  WindowRenderObject(const WindowRenderObject& other) = delete;
  WindowRenderObject(WindowRenderObject&& other) = delete;
  WindowRenderObject& operator=(const WindowRenderObject& other) = delete;
  WindowRenderObject& operator=(WindowRenderObject&& other) = delete;
  ~WindowRenderObject() override = default;

  void Draw(platform::graph::IPainter* painter) override;

  RenderObject* HitTest(const Point& point) override;

 protected:
  Size OnMeasureContent(const MeasureRequirement& requirement) override;
  void OnLayoutContent(const Rect& content_rect) override;

 private:
  RenderObject* GetChild() const {
    return GetChildren().empty() ? nullptr : GetChildren()[0];
  }

 private:
  EventRevokerGuard after_layout_event_guard_;
};
}  // namespace cru::ui::render
