#pragma once
#include "../win_pre_config.hpp"

#include "cru/platform/graph/painter.hpp"

namespace cru::win::graph {
class GraphManager;

class WinPainter : public Object, public virtual platform::graph::IPainter {
 public:
  explicit WinPainter(ID2D1RenderTarget* render_target);
  WinPainter(const WinPainter& other) = delete;
  WinPainter(WinPainter&& other) = delete;
  WinPainter& operator=(const WinPainter& other) = delete;
  WinPainter& operator=(WinPainter&& other) = delete;
  ~WinPainter() override = default;

  platform::Matrix GetTransform() override;
  void SetTransform(const platform::Matrix& matrix) override;
  void Clear(const ui::Color& color) override;
  void StrokeRectangle(const ui::Rect& rectangle, platform::graph::IBrush* brush,
                       float width) override;
  void FillRectangle(const ui::Rect& rectangle,
                     platform::graph::IBrush* brush) override;
  void StrokeGeometry(platform::graph::IGeometry* geometry,
                      platform::graph::IBrush* brush, float width) override;
  void FillGeometry(platform::graph::IGeometry* geometry,
                    platform::graph::IBrush* brush) override;
  void DrawText(const ui::Point& offset,
                platform::graph::ITextLayout* text_layout,
                platform::graph::IBrush* brush) override;
  void End() override final;
  bool IsEnded() const override final { return is_draw_ended_; }

 protected:
  virtual void DoEndDraw() = 0;

 private:
  ID2D1RenderTarget* render_target_;

  bool is_draw_ended_ = false;
};
}  // namespace cru::win::graph
