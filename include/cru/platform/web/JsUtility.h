#pragma once

#include <emscripten/val.h>

namespace cru::platform::web {
  bool IsNotNullAndInstanceOf(const emscripten::val& value, const emscripten::val& type);
  bool IsNotNullAndInstanceOf(const emscripten::val& value, const char* global_type);
}

