#pragma once
#include "Symbol.hpp"

namespace cru::parse {
class Terminal : public Symbol {
 public:
  explicit Terminal(Grammar* grammar, String name);

  CRU_DELETE_COPY(Terminal)
  CRU_DELETE_MOVE(Terminal)

  ~Terminal() override;
};
}  // namespace cru::parse
