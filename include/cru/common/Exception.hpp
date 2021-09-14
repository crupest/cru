#pragma once
#include "String.hpp"

namespace cru {
class CRU_BASE_API Exception {
 public:
  Exception() = default;
  explicit Exception(String message) : message_(std::move(message)) {}

  CRU_DEFAULT_COPY(Exception)
  CRU_DEFAULT_MOVE(Exception)

  virtual ~Exception() = default;

 public:
  String GetMessage() const { return message_; }

 private:
  String message_;
};

class CRU_BASE_API TextEncodeException : public Exception {
 public:
  using Exception::Exception;
};
}  // namespace cru
