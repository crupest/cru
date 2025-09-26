#pragma once

#include <cru/base/Base.h>
#include <cru/platform/graphics/Factory.h>
#include <cru/platform/graphics/Image.h>
#include <cru/platform/graphics/Painter.h>

#include <memory>
#include <string>

class CruPlatformGraphicsDemo : public cru::Object2 {
 public:
  CruPlatformGraphicsDemo(std::string file_name, int width, int height);
  ~CruPlatformGraphicsDemo() override;

  cru::platform::graphics::IGraphicsFactory* GetFactory();
  cru::platform::graphics::IImage* GetImage();
  cru::platform::graphics::IPainter* GetPainter();

 private:
  std::string file_name_;
  std::unique_ptr<cru::platform::graphics::IGraphicsFactory> factory_;
  std::unique_ptr<cru::platform::graphics::IImage> image_;
  std::unique_ptr<cru::platform::graphics::IPainter> painter_;
};
