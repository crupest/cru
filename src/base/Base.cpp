#include "cru/base/Base.h"

#include <cerrno>
#include <exception>
#include <format>
#include <string_view>

namespace cru {
void UnreachableCode() { std::terminate(); }
void NotImplemented() { std::terminate(); }

Exception::Exception(std::string message, std::shared_ptr<std::exception> inner)
    : message_(std::move(message)), inner_(std::move(inner)) {}

Exception::~Exception() {}

const char* Exception::what() const noexcept { return message_.c_str(); }

void Exception::AppendMessage(std::string_view additional_message) {
  if (additional_message.empty()) return;
  message_ += ' ';
  message_ += additional_message;
}

void Exception::AppendMessage(
    std::optional<std::string_view> additional_message) {
  if (additional_message) AppendMessage(*additional_message);
}

ErrnoException::ErrnoException() : ErrnoException("") {}

ErrnoException::ErrnoException(int errno_code)
    : ErrnoException("", errno_code) {}

ErrnoException::ErrnoException(std::string_view message)
    : ErrnoException(message, errno) {}

ErrnoException::ErrnoException(std::string_view message, int errno_code)
    : Exception(std::format("Errno is {}.", errno_code)),
      errno_code_(errno_code) {
  AppendMessage(message);
}
}  // namespace cru
