#pragma once

#include <cru/Base.h>
#include "cru/Resource.h"

namespace cru::graphics::web_canvas {
class WebCanvasGraphicsFactory;

class WebCanvasResource : public Object, public virtual IPlatformResource {
 public:
  static const String kPlatformId;

  explicit WebCanvasResource(WebCanvasGraphicsFactory* factory);


  ~WebCanvasResource() override;

 public:
  String GetPlatformId() const override;

  WebCanvasGraphicsFactory* GetFactory() const { return factory_; }

 private:
  WebCanvasGraphicsFactory* factory_;
};
}  // namespace cru::graphics::web_canvas
