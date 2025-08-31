#include "cru/base/Exception.h"

#include "cru/base/Format.h"

#include <cerrno>

namespace cru {
Exception::Exception(std::string message, std::unique_ptr<std::exception> inner)
    : message_(std::move(message)), inner_(std::move(inner)) {}

Exception::Exception(StringView message, std::unique_ptr<std::exception> inner)
    : message_(message.ToUtf8()), inner_(std::move(inner)) {}

Exception::~Exception() {}

const char* Exception::what() const noexcept {
  return message_.c_str();
}

void Exception::AppendMessage(StringView additional_message) {
  message_ += " ";
  message_ += additional_message.ToUtf8();
}

void Exception::AppendMessage(std::optional<StringView> additional_message) {
  if (additional_message) AppendMessage(*additional_message);
}

ErrnoException::ErrnoException(String message)
    : ErrnoException(message, errno) {}

ErrnoException::ErrnoException(String message, int errno_code)
    : Exception(Format(u"{}. Errno is {}.", message, errno_code)),
      errno_code_(errno_code) {}
}  // namespace cru
