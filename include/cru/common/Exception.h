#pragma once
#include "String.h"

namespace cru {
#ifdef _MSC_VER
#pragma warning(disable : 4275)
#endif
class CRU_BASE_API Exception : public std::exception {
 public:
  Exception();
  explicit Exception(String message);

  CRU_DEFAULT_COPY(Exception)
  CRU_DEFAULT_MOVE(Exception)

  ~Exception() override;

 public:
  String GetMessage() const { return message_; }

  const char* what() const noexcept override;

 private:
  String message_;
  mutable std::string utf8_message_;
};

class CRU_BASE_API TextEncodeException : public Exception {
 public:
  using Exception::Exception;
};

class ErrnoException : public Exception {
 public:
  ErrnoException() : ErrnoException(String{}) {}
  explicit ErrnoException(const String& message);
  ErrnoException(const String& message, int errno_code);

  CRU_DELETE_COPY(ErrnoException)
  CRU_DELETE_MOVE(ErrnoException)

  ~ErrnoException() override = default;

  int GetErrnoCode() const { return errno_code_; }

 private:
  int errno_code_;
};
}  // namespace cru
