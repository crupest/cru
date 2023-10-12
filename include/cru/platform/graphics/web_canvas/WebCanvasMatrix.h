#pragma once

#include "../../Matrix.h"

#include <emscripten/val.h>

namespace cru::platform::graphics::web_canvas {
emscripten::val CreateDomMatrix(const Matrix& matrix);
}
