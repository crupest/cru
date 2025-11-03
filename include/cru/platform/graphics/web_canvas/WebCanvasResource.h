#pragma once

#include "cru/base/Base.h"
#include "cru/platform/Base.h"

namespace cru::platform::graphics::web_canvas {
class WebCanvasGraphicsFactory;

class WebCanvasResource : public Object, public virtual IPlatformResource {
 public:
  static const std::string kPlatformId;

  explicit WebCanvasResource(WebCanvasGraphicsFactory* factory);
  ~WebCanvasResource() override;

 public:
  std::string GetPlatformId() const override;

  WebCanvasGraphicsFactory* GetFactory() const { return factory_; }

 private:
  WebCanvasGraphicsFactory* factory_;
};
}  // namespace cru::platform::graphics::web_canvas
