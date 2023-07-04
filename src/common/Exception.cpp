#include "cru/common/Exception.h"

#include "cru/common/Format.h"

#include <cerrno>

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

ErrnoException::ErrnoException(const String& message)
    : ErrnoException(message, errno) {}

ErrnoException::ErrnoException(const String& message, int errno_code)
    : Exception(Format(u"{}. Errno is {}.", message, errno_code)),
      errno_code_(errno_code) {}
}  // namespace cru
