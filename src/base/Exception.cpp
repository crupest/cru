#include "cru/base/Exception.h"

#include <cerrno>
#include <format>
#include <string_view>

constexpr auto NO_MESSAGE = "No message.";

namespace cru {
Exception::Exception(std::string message, std::unique_ptr<std::exception> inner)
    : message_(std::move(message)), inner_(std::move(inner)) {}

Exception::Exception(StringView message, std::unique_ptr<std::exception> inner)
    : message_(message.ToUtf8()), inner_(std::move(inner)) {}

Exception::~Exception() {}

const char* Exception::what() const noexcept { return message_.c_str(); }

void Exception::AppendMessage(StringView additional_message) {
  message_ += " ";
  message_ += additional_message.ToUtf8();
}

void Exception::AppendMessage(std::optional<StringView> additional_message) {
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

ErrnoException::ErrnoException(StringView message)
    : ErrnoException(message.ToUtf8()) {}

ErrnoException::ErrnoException(StringView message, int errno_code)
    : ErrnoException(message.ToUtf8(), errno_code) {}
}  // namespace cru
