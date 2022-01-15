#include "cru/common/Exception.hpp"

namespace cru {
Exception::Exception() {}

Exception::Exception(String message) : message_(std::move(message)) {}

Exception::~Exception() {}

const char* Exception::what() const noexcept {
  if (!message_.empty() && utf8_message_.empty()) {
    utf8_message_ = message_.ToUtf8();
  }

  return utf8_message_.c_str();
}
}  // namespace cru
