#include "Base.h"
#include <cru/base/io/CFileStream.h>
#include <cru/platform/bootstrap/GraphicsBootstrap.h>
#include <cru/platform/graphics/Factory.h>
#include <cru/platform/graphics/Image.h>
#include <cru/platform/graphics/ImageFactory.h>

CruPlatformGraphicsDemo::CruPlatformGraphicsDemo(std::string file_name,
                                                 int width, int height)
    : file_name_(std::move(file_name)) {
  factory_.reset(cru::platform::bootstrap::CreateGraphicsFactory());
  image_ = factory_->GetImageFactory()->CreateBitmap(width, height);
  painter_ = image_->CreatePainter();
}

CruPlatformGraphicsDemo::~CruPlatformGraphicsDemo() {
  painter_->EndDraw();

  cru::io::CFileStream file_stream(file_name_.c_str(), "wb");

  factory_->GetImageFactory()->EncodeToStream(
      image_.get(), &file_stream, cru::platform::graphics::ImageFormat::Png,
      1.0f);
}

cru::platform::graphics::IGraphicsFactory*
CruPlatformGraphicsDemo::GetFactory() {
  return factory_.get();
}

cru::platform::graphics::IImage* CruPlatformGraphicsDemo::GetImage() {
  return image_.get();
}

cru::platform::graphics::IPainter* CruPlatformGraphicsDemo::GetPainter() {
  return painter_.get();
}
