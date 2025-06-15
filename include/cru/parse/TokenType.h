#pragma once
#include "Base.h"

#include <cru/Base.h>
#include <cru/String.h>

namespace cru::parse {
class CRU_PARSE_API TokenType : public Object {
 public:
  explicit TokenType(String name);


  ~TokenType() override;

 public:
  String GetName() const { return name_; }
  void SetName(String name) { name_ = std::move(name); }

 private:
  String name_;
};
}  // namespace cru::parse
