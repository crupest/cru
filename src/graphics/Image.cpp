#include "cru/graphics/Image.h"

#include "cru/graphics/Factory.h"
#include "cru/graphics/ImageFactory.h"
#include "cru/graphics/Painter.h"

namespace cru::graphics {
std::unique_ptr<IImage> IImage::CloneToBitmap() {
  auto image = GetGraphicsFactory()->GetImageFactory()->CreateBitmap(
      GetWidth(), GetHeight());
  auto painter = image->CreatePainter();
  painter->DrawImage(Point{}, this);
  return image;
}
}  // namespace cru::graphics
