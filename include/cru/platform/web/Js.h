#pragma once

#include <emscripten/val.h>
#include <utility>

namespace cru::platform::web::js {
bool IsNotNullAndInstanceOf(const emscripten::val& value,
                            const emscripten::val& type);
bool IsNotNullAndInstanceOf(const emscripten::val& value,
                            const char* global_type);

template <typename... Args>
emscripten::val Construct(const char* class_name, Args&&... args) {
  emscripten::val constructor = emscripten::val::global(class_name);
  return constructor.new_(std::forward<Args>(args)...);
}
}  // namespace cru::platform::web
