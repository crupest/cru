#include "cru/platform/graphics/Image.h"

#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/ImageFactory.h"
#include "cru/platform/graphics/Painter.h"

namespace cru::platform::graphics {
std::unique_ptr<IImage> IImage::CloneToBitmap() {
  auto image = GetGraphicsFactory()->GetImageFactory()->CreateBitmap(
      GetWidth(), GetHeight());
  auto painter = image->CreatePainter();
  painter->DrawImage(Point{}, this);
  return image;
}
}  // namespace cru::platform::graphics
