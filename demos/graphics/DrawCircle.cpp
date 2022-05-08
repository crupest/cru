#include "cru/common/io/FileStream.h"
#include "cru/common/io/OpenFileFlag.h"
#include "cru/platform/Color.h"
#include "cru/platform/bootstrap/Bootstrap.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/graphics/ImageFactory.h"
#include "cru/platform/graphics/Painter.h"

#include <memory>

int main() {
  std::unique_ptr<cru::platform::graphics::IGraphicsFactory> graphics_factory(
      cru::platform::bootstrap::CreateGraphicsFactory());

  auto image = graphics_factory->GetImageFactory()->CreateBitmap(500, 500);

  {
    auto brush =
        graphics_factory->CreateSolidColorBrush(cru::platform::colors::black);
    auto painter = image->CreatePainter();
    painter->FillEllipse(cru::platform::Rect{200, 200, 100, 100}, brush.get());
    painter->EndDraw();
  }

  cru::io::FileStream file_stream(
      u"./test_image.png",
      cru::io::OpenFileFlags::Write | cru::io::OpenFileFlags::Create);

  graphics_factory->GetImageFactory()->EncodeToStream(
      image.get(), &file_stream, cru::platform::graphics::ImageFormat::Png,
      1.0f);

  return 0;
}
