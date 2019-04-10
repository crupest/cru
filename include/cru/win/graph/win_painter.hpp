#pragma once
#include "../win_pre_config.hpp"

#include "cru/platform/graph/painter.hpp"

namespace cru::win::graph {
class GraphManager;

class WinPainter : public Object, public virtual platform::graph::Painter {
 public:
  explicit WinPainter(ID2D1RenderTarget* render_target);
  WinPainter(const WinPainter& other) = delete;
  WinPainter(WinPainter&& other) = delete;
  WinPainter& operator=(const WinPainter& other) = delete;
  WinPainter& operator=(WinPainter&& other) = delete;
  ~WinPainter() override;

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
  void EndDraw() override;
  bool IsDisposed() override { return is_disposed_; }

  void EndDrawAndDeleteThis() {
    EndDraw();
    delete this;
  }

 protected:
  virtual void DoEndDraw();

 private:
  ID2D1RenderTarget* render_target_;

  bool is_disposed_ = false;
};
}  // namespace cru::win::graph
