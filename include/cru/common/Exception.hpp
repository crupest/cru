#pragma once
#include "String.hpp"

namespace cru {
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
}  // namespace cru
