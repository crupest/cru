#include "cru/platform/web/JsUtility.h"

namespace cru::platform::web {
bool IsNotNullAndInstanceOf(const emscripten::val& value, const emscripten::val& type) {
  return !value.isNull() && !type.isNull() && value.instanceof(type);
}

bool IsNotNullAndInstanceOf(const emscripten::val& value, const char* global_type) {
  return IsNotNullAndInstanceOf(value, emscripten::val::global(global_type));
}
}
