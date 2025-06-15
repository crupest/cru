#pragma once
#include "Resource.h"
#include "cru/graphics/Image.h"

namespace cru::graphics::direct2d {
class CRU_WIN_GRAPHICS_DIRECT_API Direct2DImage : public DirectGraphicsResource,
                                                  public virtual IImage {
 public:
  Direct2DImage(DirectGraphicsFactory* graphics_factory,
                Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2d_bitmap);


  ~Direct2DImage() override;

 public:
  float GetWidth() override;
  float GetHeight() override;

  std::unique_ptr<IImage> CreateWithRect(const Rect& rect) override;

  std::unique_ptr<IPainter> CreatePainter() override;

  const Microsoft::WRL::ComPtr<ID2D1Bitmap1>& GetD2DBitmap() const {
    return d2d_bitmap_;
  }

 private:
  Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2d_bitmap_;
};

}  // namespace cru::graphics::direct2d
