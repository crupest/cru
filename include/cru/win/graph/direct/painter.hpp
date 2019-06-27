#pragma once
#include "com_resource.hpp"
#include "platform_id.hpp"

#include "cru/platform/graph/painter.hpp"

namespace cru::platform::graph::win::direct {
class D2DPainter : public Painter, public IComResource<ID2D1RenderTarget> {
 public:
  explicit D2DPainter(ID2D1RenderTarget* render_target);

  D2DPainter(const D2DPainter& other) = delete;
  D2DPainter& operator=(const D2DPainter& other) = delete;

  D2DPainter(D2DPainter&& other) = delete;
  D2DPainter& operator=(D2DPainter&& other) = delete;

  ~D2DPainter() override = default;

  CRU_PLATFORMID_IMPLEMENT_DIRECT

 public:
  ID2D1RenderTarget* GetComInterface() const override { return render_target_; }

 public:
  Matrix GetTransform() override;
  void SetTransform(const platform::Matrix& matrix) override;

  void Clear(const Color& color) override;

  void StrokeRectangle(const Rect& rectangle, Brush* brush,
                       float width) override;
  void FillRectangle(const Rect& rectangle, Brush* brush) override;

  void StrokeGeometry(Geometry* geometry, Brush* brush, float width) override;
  void FillGeometry(Geometry* geometry, Brush* brush) override;

  void DrawText(const Point& offset, TextLayout* text_layout,
                Brush* brush) override;

  void EndDraw() override final;

 protected:
  virtual void DoEndDraw() = 0;

 private:
  bool IsValid() { return is_drawing_; }

 private:
  ID2D1RenderTarget* render_target_;

  bool is_drawing_ = true;
};
}  // namespace cru::platform::graph::win::direct
