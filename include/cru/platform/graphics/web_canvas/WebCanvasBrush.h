#pragma once
#include "../Brush.h"
#include "WebCanvasResource.h"

namespace cru::platform::graphics::web_canvas {
class WebCanvasBrush : public WebCanvasResource, public virtual IBrush {
 public:
  explicit WebCanvasBrush(WebCanvasGraphicsFactory* factory);

  ~WebCanvasBrush() override;
};

class WebCanvasSolidColorBrush : public WebCanvasBrush,
                                 public virtual ISolidColorBrush {
 public:
  WebCanvasSolidColorBrush(WebCanvasGraphicsFactory* factory,
                           const Color& color);

  ~WebCanvasSolidColorBrush() override;

 public:
  Color GetColor() override;
  void SetColor(const Color& color) override;

  String GetCssColor() const;

 private:
  Color color_;
};
}  // namespace cru::platform::graphics::web_canvas
