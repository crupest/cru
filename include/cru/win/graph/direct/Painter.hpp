#pragma once
#include "ComResource.hpp"
#include "Resource.hpp"

#include "cru/platform/graph/Painter.hpp"

#include <vector>

namespace cru::platform::graph::win::direct {
class D2DPainter : public DirectResource,
                   public virtual IPainter,
                   public virtual IComResource<ID2D1RenderTarget> {
 public:
  explicit D2DPainter(ID2D1RenderTarget* render_target);

  CRU_DELETE_COPY(D2DPainter)
  CRU_DELETE_MOVE(D2DPainter)

  ~D2DPainter() override = default;

 public:
  ID2D1RenderTarget* GetComInterface() const override { return render_target_; }

 public:
  Matrix GetTransform() override;
  void SetTransform(const platform::Matrix& matrix) override;

  void Clear(const Color& color) override;

  void StrokeRectangle(const Rect& rectangle, IBrush* brush,
                       float width) override;
  void FillRectangle(const Rect& rectangle, IBrush* brush) override;

  void StrokeGeometry(IGeometry* geometry, IBrush* brush, float width) override;
  void FillGeometry(IGeometry* geometry, IBrush* brush) override;

  void DrawText(const Point& offset, ITextLayout* text_layout,
                IBrush* brush) override;

  void PushLayer(const Rect& bounds) override;

  void PopLayer() override;

  void EndDraw() override final;

 protected:
  virtual void DoEndDraw() = 0;

 private:
  bool IsValid() { return is_drawing_; }
  void CheckValidation();

 private:
  ID2D1RenderTarget* render_target_;

  std::vector<Microsoft::WRL::ComPtr<ID2D1Layer>> layers_;

  bool is_drawing_ = true;
};
}  // namespace cru::platform::graph::win::direct
