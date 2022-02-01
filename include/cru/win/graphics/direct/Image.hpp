#pragma once
#include "Resource.hpp"
#include "cru/platform/graphics/Image.hpp"

namespace cru::platform::graphics::win::direct {
class CRU_WIN_GRAPHICS_DIRECT_API Direct2DImage : public DirectGraphicsResource,
                                                  public virtual IImage {
 public:
  explicit Direct2DImage(DirectGraphicsFactory* graphics_factory,
                         ID2D1Image* d2d_image, bool auto_release);

  CRU_DELETE_COPY(Direct2DImage)
  CRU_DELETE_MOVE(Direct2DImage)

  ~Direct2DImage() override;

 public:
  ID2D1Image* GetD2DImage() const { return d2d_image_; }

 private:
  ID2D1Image* d2d_image_;
  bool auto_release_;
};

}  // namespace cru::platform::graphics::win::direct
