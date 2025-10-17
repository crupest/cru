#pragma once
#include "Base.h"

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
                     std::shared_ptr<std::exception> inner = nullptr);

  ~Exception() override;

 public:
  std::string GetUtf8Message() const { return this->message_; }

  std::exception* GetInner() const noexcept { return inner_.get(); }

  const char* what() const noexcept override;

 protected:
  void SetMessage(std::string message) { message_ = std::move(message); }
  void AppendMessage(const std::string& additional_message);
  void AppendMessage(std::string_view additional_message);
  void AppendMessage(std::optional<std::string_view> additional_message);

 private:
  std::string message_;
  std::shared_ptr<std::exception> inner_;
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

  int GetErrnoCode() const { return errno_code_; }

 private:
  int errno_code_;
};
}  // namespace cru
