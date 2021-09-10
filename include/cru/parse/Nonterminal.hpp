#pragma once
#include "Symbol.hpp"

namespace cru::parse {
class Nonterminal : public Symbol {
 public:
  Nonterminal(Grammar* grammar, String name);

  CRU_DELETE_COPY(Nonterminal)
  CRU_DELETE_MOVE(Nonterminal)

  ~Nonterminal() override;
};
}  // namespace cru::parse
