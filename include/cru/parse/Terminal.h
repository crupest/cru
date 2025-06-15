#pragma once
#include "Symbol.h"

namespace cru::parse {
class CRU_PARSE_API Terminal : public Symbol {
 public:
  Terminal(Grammar* grammar, String name);


  ~Terminal() override;
};
}  // namespace cru::parse
