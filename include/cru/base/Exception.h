#pragma once
#include "String.h"

#include <exception>
#include <optional>

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
  void SetMessage(StringView message) { message_ = message.ToUtf8(); }

  void AppendMessage(StringView additional_message);
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
  /**
   * @brief will retrieve errno automatically.
   */
  explicit ErrnoException(String message = {});
  ErrnoException(String message, int errno_code);

  CRU_DELETE_COPY(ErrnoException)
  CRU_DELETE_MOVE(ErrnoException)

  ~ErrnoException() override = default;

  int GetErrnoCode() const { return errno_code_; }

 private:
  int errno_code_;
};
}  // namespace cru
