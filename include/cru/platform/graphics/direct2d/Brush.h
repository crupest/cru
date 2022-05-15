#pragma once
#include "ComResource.h"
#include "Resource.h"

#include "cru/platform/graphics/Brush.h"

namespace cru::platform::graphics::direct2d {
struct CRU_WIN_GRAPHICS_DIRECT_API ID2DBrush : virtual IBrush {
  virtual ID2D1Brush* GetD2DBrushInterface() const = 0;
};

class CRU_WIN_GRAPHICS_DIRECT_API D2DSolidColorBrush
    : public DirectGraphicsResource,
      public virtual ISolidColorBrush,
      public virtual ID2DBrush,
      public virtual IComResource<ID2D1SolidColorBrush> {
 public:
  explicit D2DSolidColorBrush(DirectGraphicsFactory* factory);

  CRU_DELETE_COPY(D2DSolidColorBrush)
  CRU_DELETE_MOVE(D2DSolidColorBrush)

  ~D2DSolidColorBrush() override = default;

 public:
  Color GetColor() override { return color_; }
  void SetColor(const Color& color) override;

  ID2D1Brush* GetD2DBrushInterface() const override { return brush_.Get(); }

  ID2D1SolidColorBrush* GetComInterface() const override {
    return brush_.Get();
  }

 private:
  Color color_ = colors::black;

  Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush_;
};
}  // namespace cru::platform::graphics::direct2d
