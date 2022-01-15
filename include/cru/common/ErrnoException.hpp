#pragma once

#include "PreConfig.hpp"

#ifdef CRU_PLATFORM_UNIX

#include "Exception.hpp"

namespace cru {
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

#endif
