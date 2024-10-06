#pragma once

#include "cru/base/Base.h"

#include <emscripten/val.h>

namespace cru::platform::graphics::web_canvas {
  class WebCanvasRef {
    public:
      explicit WebCanvasRef(emscripten::val canvas_val);

      CRU_DEFAULT_COPY(WebCanvasRef)
      CRU_DEFAULT_MOVE(WebCanvasRef)

      CRU_DEFAULT_DESTRUCTOR(WebCanvasRef)

    public:
      int GetWidth() const;
      int GetHeight() const;

      void Save() const;
      void Restore() const;

    private:
      emscripten::val val_;
  };
}
