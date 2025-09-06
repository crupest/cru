#pragma once
#include "String.h"

#include <exception>
#include <optional>
#include <string_view>

namespace cru {
#ifdef _MSC_VER
#pragma warning(disable : 4275)
#endif
class CRU_BASE_API Exception : public std::exception {
 public:
  explicit Exception(std::string message = "",
                     std::unique_ptr<std::exception> inner = nullptr);
  explicit Exception(StringView message,
                     std::unique_ptr<std::exception> inner = nullptr);

  ~Exception() override;

 public:
  [[deprecated("Use GetUtf8Message.")]] String GetMessage() const {
    return String::FromUtf8(message_);
  }

  std::string GetUtf8Message() const { return this->message_; }

  std::exception* GetInner() const noexcept { return inner_.get(); }

  const char* what() const noexcept override;

 protected:
  void SetMessage(std::string message) { message_ = std::move(message); }
  void AppendMessage(std::string_view additional_message);
  void AppendMessage(std::optional<std::string_view> additional_message);

  [[deprecated("Use void SetMessage(std::string message) instead.")]]
  void SetMessage(StringView message);
  [[deprecated("Use void AppendMessage(std::string_view additional_message) instead.")]]
  void AppendMessage(StringView additional_message);
  [[deprecated("Use void AppendMessage(std::optional<std::string_view> additional_message) instead.")]]
  void AppendMessage(std::optional<StringView> additional_message);

 private:
  std::string message_;
  std::unique_ptr<std::exception> inner_;
};

class CRU_BASE_API PlatformException : public Exception {
 public:
  using Exception::Exception;  // inherit constructors

  CRU_DEFAULT_DESTRUCTOR(PlatformException)
};

class CRU_BASE_API TextEncodeException : public Exception {
 public:
  using Exception::Exception;
};

class ErrnoException : public Exception {
 public:
  ErrnoException();
  explicit ErrnoException(int error_code);
  /**
   * @brief will retrieve errno automatically.
   */
  explicit ErrnoException(std::string_view message);
  ErrnoException(std::string_view message, int errno_code);
  /**
   * @brief will retrieve errno automatically.
   */
  explicit ErrnoException(StringView message);
  ErrnoException(StringView message, int errno_code);

  int GetErrnoCode() const { return errno_code_; }

 private:
  int errno_code_;
};
}  // namespace cru
