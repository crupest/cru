#pragma once
#include "Symbol.h"

namespace cru::parse {
class CRU_PARSE_API Nonterminal : public Symbol {
 public:
  Nonterminal(Grammar* grammar, String name);


  ~Nonterminal() override;
};
}  // namespace cru::parse
