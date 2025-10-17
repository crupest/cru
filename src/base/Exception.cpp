#include "cru/base/Exception.h"

#include <cerrno>
#include <format>
#include <string_view>

constexpr auto NO_MESSAGE = "No message.";

namespace cru {
Exception::Exception(std::string message, std::shared_ptr<std::exception> inner)
    : message_(std::move(message)), inner_(std::move(inner)) {}

Exception::~Exception() {}

const char* Exception::what() const noexcept { return message_.c_str(); }

void Exception::AppendMessage(const std::string& additional_message) {
  AppendMessage(std::string_view(additional_message));
}

void Exception::AppendMessage(std::string_view additional_message) {
  message_ += ' ';
  message_ += additional_message;
}

void Exception::AppendMessage(
    std::optional<std::string_view> additional_message) {
  if (additional_message) AppendMessage(*additional_message);
}

ErrnoException::ErrnoException() : ErrnoException(NO_MESSAGE) {}

ErrnoException::ErrnoException(int errno_code)
    : ErrnoException(NO_MESSAGE, errno_code) {}

ErrnoException::ErrnoException(std::string_view message)
    : ErrnoException(message, errno) {}

ErrnoException::ErrnoException(std::string_view message, int errno_code)
    : Exception(std::format("{} Errno is {}.", message, errno_code)),
      errno_code_(errno_code) {}
}  // namespace cru
