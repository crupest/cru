#pragma once
#include "Base.h"

#include <cru/platform/graphics/Painter.h>

#include <vector>

namespace cru::platform::graphics::direct2d {
class CRU_WIN_GRAPHICS_DIRECT_API D2DDeviceContextPainter
    : public DirectGraphicsResource,
      public virtual IPainter,
      public virtual IComResource<ID2D1DeviceContext1> {
  CRU_DEFINE_CLASS_LOG_TAG("D2DDeviceContextPainter")
 public:
  explicit D2DDeviceContextPainter(DirectGraphicsFactory* graphics_factory,
                                   ID2D1DeviceContext1* device_context,
                                   bool release = false);

  ~D2DDeviceContextPainter() override;

 public:
  ID2D1DeviceContext1* GetComInterface() const override {
    return device_context_;
  }

 public:
  Matrix GetTransform() override;
  void SetTransform(const platform::Matrix& matrix) override;
  void ConcatTransform(const Matrix& matrix) override;

  void Clear(const Color& color) override;

  void DrawLine(const Point& start, const Point& end, IBrush* brush,
                float width) override;
  void StrokeRectangle(const Rect& rectangle, IBrush* brush,
                       float width) override;
  void FillRectangle(const Rect& rectangle, IBrush* brush) override;
  void StrokeEllipse(const Rect& outline_rect, IBrush* brush,
                     float width) override;
  void FillEllipse(const Rect& outline_rect, IBrush* brush) override;

  void StrokeGeometry(IGeometry* geometry, IBrush* brush, float width) override;
  void FillGeometry(IGeometry* geometry, IBrush* brush) override;

  void DrawText(const Point& offset, ITextLayout* text_layout,
                IBrush* brush) override;

  void DrawImage(const Point& offset, IImage* image) override;

  void PushLayer(const Rect& bounds) override;
  void PopLayer() override;

  void PushState() override;
  void PopState() override;

  void EndDraw() override final;

 protected:
  virtual void DoEndDraw() {}

 private:
  bool IsValid() { return is_drawing_; }
  void CheckValidation();

 private:
  ID2D1DeviceContext1* device_context_;

  std::vector<Microsoft::WRL::ComPtr<ID2D1Layer>> layers_;
  std::vector<Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>>
      drawing_state_stack_;

  bool is_drawing_ = true;

  bool release_;
};
}  // namespace cru::platform::graphics::direct2d
