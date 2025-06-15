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
  explicit Exception(String message = {},
                     std::unique_ptr<std::exception> inner = nullptr);

  ~Exception() override;

 public:
  String GetMessage() const { return message_; }

  std::exception* GetInner() const noexcept { return inner_.get(); }

  const char* what() const noexcept override;

 protected:
  void SetMessage(String message) { message_ = std::move(message); }

  void AppendMessage(StringView additional_message);
  void AppendMessage(std::optional<StringView> additional_message);

 private:
  String message_;
  mutable std::string utf8_message_;
  std::unique_ptr<std::exception> inner_;
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


  ~ErrnoException() override = default;

  int GetErrnoCode() const { return errno_code_; }

 private:
  int errno_code_;
};

// This exception is thrown when a resource has been disposed and not usable
// again.
// For example, calling Build twice on a GeometryBuilder::Build will lead to
// this exception.
class CRU_BASE_API ReuseException : public Exception {
 public:
  using Exception::Exception;  // inherit constructors

  CRU_DEFAULT_DESTRUCTOR(ReuseException)
};
}  // namespace cru
