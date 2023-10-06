#pragma once

#include "cru/common/Base.h"
#include "cru/platform/Resource.h"

namespace cru::platform::graphics::web_canvas {
class WebCanvasResource : public Object, public virtual IPlatformResource {
 public:
  static const String kPlatformId;

  WebCanvasResource();

  CRU_DELETE_COPY(WebCanvasResource)
  CRU_DELETE_MOVE(WebCanvasResource)

  ~WebCanvasResource() override;

 public:
  String GetPlatformId() const override;
};
}  // namespace cru::platform::graphics::web_canvas
