#pragma once
#include "pre.hpp"

#include <chrono>
#include <stdexcept>
#include <string>
#include <string_view>

namespace cru {
template <typename T>
struct type_tag {};

// typedefs
using String = std::wstring;
using MultiByteString = std::string;

using StringView = std::wstring_view;
using MultiByteStringView = std::string_view;

using FloatSecond = std::chrono::duration<double, std::chrono::seconds::period>;

enum class FlowControl { Continue, Break };

class Object {
 public:
  Object() = default;
  Object(const Object&) = default;
  Object& operator=(const Object&) = default;
  Object(Object&&) = default;
  Object& operator=(Object&&) = default;
  virtual ~Object() = default;
};

struct Interface {
  virtual ~Interface() = default;
};

[[noreturn]] inline void UnreachableCode() {
  throw std::logic_error("Unreachable code.");
}

inline void Require(const bool condition,
                    const MultiByteStringView& error_message) {
  if (!condition) throw std::invalid_argument(error_message.data());
}
}  // namespace cru
