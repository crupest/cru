#include <cru/CFileStream.h>
#include "cru/graphics/Color.h"
#include "cru/graphics/Factory.h"
#include "cru/graphics/ImageFactory.h"
#include "cru/graphics/Painter.h"

#include <memory>

namespace cru::demos::graphics {
  void DrawCircle(graphics::IGraphicsFactory* graphics_factory) {

  auto image = graphics_factory->GetImageFactory()->CreateBitmap(500, 500);

  {
    auto brush =
        graphics_factory->CreateSolidColorBrush(cru::platform::colors::skyblue);
    auto painter = image->CreatePainter();
    painter->FillEllipse(cru::platform::Rect{200, 200, 100, 100}, brush.get());
    painter->EndDraw();
  }

  cru::CFileStream file_stream("./test_image.png", "w");

  graphics_factory->GetImageFactory()->EncodeToStream(
      image.get(), &file_stream, cru::graphics::ImageFormat::Png,
      1.0f);
  }
}

