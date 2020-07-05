#pragma once
#include "Base.hpp"

namespace cru {
using CodePoint = std::int32_t;
constexpr CodePoint k_code_point_end = -1;

class TextEncodeException : public std::runtime_error {
 public:
  using runtime_error::runtime_error;
};

class Utf8Iterator : public Object {
 public:
  explicit Utf8Iterator(const std::string_view& string) : string_(string) {}
  Utf8Iterator(const std::string_view& string, Index position)
      : string_(string), position_(position) {}

  CRU_DEFAULT_COPY(Utf8Iterator)
  CRU_DEFAULT_MOVE(Utf8Iterator)

  ~Utf8Iterator() = default;

 public:
  void SetToHead() { position_ = 0; }
  void SetPosition(Index position) { position_ = position; }

  // Advance current position and get next code point. Return k_code_point_end
  // if there is no next code unit(point). Throw TextEncodeException if decoding
  // fails.
  CodePoint Next();

  Index CurrentPosition() const { return this->position_; }

 private:
  std::string_view string_;
  Index position_ = 0;
};
}  // namespace cru
