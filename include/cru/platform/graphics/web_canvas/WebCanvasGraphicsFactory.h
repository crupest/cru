#pragma once

#include "../Factory.h"
#include "WebCanvasResource.h"

namespace cru::platform::graphics::web_canvas {
class WebCanvasGraphicsFactory : public WebCanvasResource,
                                 public virtual IGraphicsFactory {
 public:
  WebCanvasGraphicsFactory();

  ~WebCanvasGraphicsFactory() override;
};
}  // namespace cru::platform::graphics::web_canvas
