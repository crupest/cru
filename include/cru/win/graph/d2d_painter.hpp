#pragma once
#include "../win_pre_config.hpp"

#include "cru/platform/graph/painter.hpp"

namespace cru::win::graph {
class GraphManager;

class D2DPainter : public Object, public virtual platform::graph::Painter {
 public:
  explicit D2DPainter(ID2D1RenderTarget* render_target);
  D2DPainter(const D2DPainter& other) = delete;
  D2DPainter(D2DPainter&& other) = delete;
  D2DPainter& operator=(const D2DPainter& other) = delete;
  D2DPainter& operator=(D2DPainter&& other) = delete;
  ~D2DPainter() override = default;

  platform::Matrix GetTransform() override;
  void SetTransform(const platform::Matrix& matrix) override;
  void Clear(const ui::Color& color) override;
  void StrokeRectangle(const ui::Rect& rectangle, platform::graph::Brush* brush,
                       float width) override;
  void FillRectangle(const ui::Rect& rectangle,
                     platform::graph::Brush* brush) override;
  void StrokeGeometry(platform::graph::Geometry* geometry,
                      platform::graph::Brush* brush, float width) override;
  void FillGeometry(platform::graph::Geometry* geometry,
                    platform::graph::Brush* brush) override;
  void DrawText(const ui::Point& offset,
                platform::graph::TextLayout* text_layout,
                platform::graph::Brush* brush) override;
  void EndDraw() override final;
  bool IsDisposed() override final { return is_disposed_; }

  void EndDrawAndDeleteThis() {
    EndDraw();
    delete this;
  }

 protected:
  virtual void DoEndDraw() = 0;

 private:
  ID2D1RenderTarget* render_target_;

  bool is_disposed_ = false;
};
}  // namespace cru::win::graph
