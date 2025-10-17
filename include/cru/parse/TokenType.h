#pragma once
#include "Base.h"

#include <cru/base/Base.h>
#include <string>

namespace cru::parse {
class CRU_PARSE_API TokenType : public Object {
 public:
  explicit TokenType(std::string name);

  CRU_DELETE_COPY(TokenType)
  CRU_DELETE_MOVE(TokenType)

  ~TokenType() override;

 public:
  std::string GetName() const { return name_; }
  void SetName(std::string name) { name_ = std::move(name); }

 private:
  std::string name_;
};
}  // namespace cru::parse
