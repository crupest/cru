#pragma once
#include "Symbol.h"

namespace cru::parse {
class CRU_PARSE_API Nonterminal : public Symbol {
 public:
  Nonterminal(Grammar* grammar, std::string name);

  CRU_DELETE_COPY(Nonterminal)
  CRU_DELETE_MOVE(Nonterminal)

  ~Nonterminal() override;
};
}  // namespace cru::parse
